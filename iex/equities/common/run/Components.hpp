#pragma once

#include "protocols/ipc/IpcBus.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace sim {

// The widest framed ipc message, so a queue slot always holds one whole message. Sized from
// the generated ipc structs: change the ipc model and the slots follow it.
inline constexpr std::size_t ipc_slot_bytes = 3 + std::max({
    sizeof(ipc_ns::new_order_message),
    sizeof(ipc_ns::cancel_order_message),
    sizeof(ipc_ns::trade_message),
    sizeof(ipc_ns::order_rested_message),
    sizeof(ipc_ns::order_canceled_message),
});

// What a bystander needs of a link: whether everything written has been read. Components never
// go through this — it is how quiesce inspects links whose messages are different sizes.
class Link {
public:
    virtual ~Link() = default;
    virtual bool               drained() const = 0;
    virtual const std::string& name()    const = 0;

    // Carry on delivering whatever is still waiting. Nothing ever waits in a queue or a ring - a
    // write either fits or the writer is told it does not - so for them this is nothing to do. A
    // link between machines is the one that can hold a message back, because its peer may not be
    // there yet, and a writer with nothing new to say would otherwise never touch it again: a
    // trader's login, written while the gateway was still coming up, waited for ever. So every
    // component calls this on the links it writes to, once a turn, whichever deployment it is in.
    virtual void deliver() {}
};

// A link between two components: a bounded ring of framed ipc messages, written by one
// component's thread and read by another's. Lock-free — the writer only advances `written_`,
// the reader only advances `read_`, and the release/acquire pair between them publishes the
// bytes. One writer and one reader is the whole contract; fan-out is one queue per consumer.
template <std::size_t Slot, std::size_t Slots>
class Queue final : public Link {
    static_assert((Slots & (Slots - 1)) == 0, "slot count must be a power of two");

public:
    // The widest message a slot holds, framing included.
    static constexpr std::size_t slot_bytes = Slot;

    explicit Queue(std::string name) : name_(std::move(name)) {}

    Queue(const Queue&)            = delete;
    Queue& operator=(const Queue&) = delete;

    const std::string& name() const override { return name_; }

    // Writer side. False when the reader is far enough behind that the ring is full; the
    // caller decides whether to retry (backpressure) or drop.
    bool write(const char* data, std::size_t length) {
        if (length > Slot) return false;

        const auto written = written_.load(std::memory_order_relaxed);
        if (written - read_.load(std::memory_order_acquire) >= Slots) return false;

        auto& slot = slots_[written & (Slots - 1)];
        std::memcpy(slot.bytes.data(), data, length);
        slot.length = length;
        written_.store(written + 1, std::memory_order_release);
        return true;
    }

    // Reader side: hands one message to `reader`. False when the queue is empty.
    template <typename Reader>
    bool read(Reader&& reader) {
        const auto read = read_.load(std::memory_order_relaxed);
        if (read == written_.load(std::memory_order_acquire)) return false;

        const auto& slot = slots_[read & (Slots - 1)];
        reader(std::string_view(slot.bytes.data(), slot.length));
        read_.store(read + 1, std::memory_order_release);
        return true;
    }

    bool          drained() const override { return read_.load(std::memory_order_acquire) == written_.load(std::memory_order_acquire); }
    std::uint64_t written() const { return written_.load(std::memory_order_relaxed); }
    std::uint64_t read()    const { return read_.load(std::memory_order_relaxed); }

private:
    struct Cell {
        std::array<char, Slot> bytes{};
        std::size_t            length = 0;
    };

    std::string                          name_;
    std::vector<Cell>                    slots_ = std::vector<Cell>(Slots);
    alignas(64) std::atomic<std::uint64_t> written_{0};
    alignas(64) std::atomic<std::uint64_t> read_{0};
};

// The links between the exchange's own components carry ipc frames.
using IpcQueue = Queue<ipc_slot_bytes, 4096>;

// A link carrying a participant protocol's messages instead — a trader's FIX order on its way to
// the gateway, say — framed the same way: [len u16][type u8][bytes].
using MessageQueue = Queue<512, 4096>;

// A component: a named thread running one step function until it is stopped. The step does
// as much as it can without blocking and reports whether it found work; when it finds none
// the component backs off, so an idle venue does not spin a core per component.
class Component {
public:
    template <typename Step>
    Component(std::string name, Step step) : name_(std::move(name)), step_(std::move(step)) {}

    ~Component() { stop(); }

    Component(const Component&)            = delete;
    Component& operator=(const Component&) = delete;

    void start() {
        if (thread_.joinable()) return;
        running_.store(true, std::memory_order_release);
        thread_ = std::thread([this] { run(); });
    }

    void stop() {
        running_.store(false, std::memory_order_release);
        if (thread_.joinable()) thread_.join();
    }

    const std::string& name()  const { return name_; }
    std::uint64_t      steps() const { return worked_.load(std::memory_order_relaxed); }

    // Between steps with nothing to do: the component holds no message part-way through.
    bool idle() const { return idle_.load(std::memory_order_acquire); }

private:
    void run() {
        unsigned quiet = 0;
        while (running_.load(std::memory_order_acquire)) {
            idle_.store(false, std::memory_order_release);
            const bool worked = step_();
            if (worked) {
                worked_.fetch_add(1, std::memory_order_relaxed);
                quiet = 0;
                idle_.store(false, std::memory_order_release);
                continue;
            }

            idle_.store(true, std::memory_order_release);
            // Backoff: spin briefly (a burst usually follows a message), then sleep, so a
            // quiet venue costs nothing while a busy one never pays the wakeup latency.
            if (++quiet < 64) std::this_thread::yield();
            else              std::this_thread::sleep_for(std::chrono::microseconds(quiet < 1024 ? 100 : 1000));
        }
    }

    std::string           name_;
    std::function<bool()> step_;
    std::thread           thread_;
    std::atomic<bool>     running_{false};
    std::atomic<bool>     idle_{true};
    std::atomic<std::uint64_t> worked_{0};
};

// A turn a component takes only when it is asked to. A run made of turns is the same run every
// time: one component acts, the venue settles, the next one acts — whatever order the threads
// would otherwise have raced into. A venue that is serving never takes turns; it is live.
class Turn {
public:
    // The component's side: is it my turn, and I have taken it.
    bool asked() const { return asked_.load(std::memory_order_acquire) != taken_.load(std::memory_order_acquire); }
    void taken()       { taken_.fetch_add(1, std::memory_order_release); }

    // The run's side: ask for one step, then wait for it to be taken.
    void ask() { asked_.fetch_add(1, std::memory_order_release); }

    bool wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(5000)) const {
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (asked()) {
            if (std::chrono::steady_clock::now() >= deadline) return false;
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
        return true;
    }

private:
    std::atomic<std::uint64_t> asked_{0};
    std::atomic<std::uint64_t> taken_{0};
};

// Wait until every queue is drained and every component is between steps — everything fed in
// has then reached the far end of every link.
//
// This is a quiescence check, not a barrier: it is exact only while nothing outside the
// process is still feeding the links, which is where it is used (the startup self-check and
// the tests). A serving venue never quiesces, and does not ask.
inline bool quiesce(std::initializer_list<const Link*>      queues,
                    std::initializer_list<const Component*> components,
                    std::chrono::milliseconds               timeout = std::chrono::milliseconds(2000)) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    unsigned   settled  = 0;

    while (std::chrono::steady_clock::now() < deadline) {
        bool quiet = true;
        for (const auto* queue : queues)         quiet = quiet && queue->drained();
        for (const auto* component : components) quiet = quiet && component->idle();

        // Stable across consecutive passes: one pass can catch a component that has taken a
        // message off its queue but has not yet written to the next one.
        settled = quiet ? settled + 1 : 0;
        if (settled >= 4) return true;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return false;
}

// Write to a link, waiting for room rather than losing the message: a slow consumer slows its
// producer down instead of leaving a gap in the stream.
//
// Any link with a write() will do, which is what lets the same call serve both versions of a
// venue: a Queue inside the single binary, and a SharedRing between the component processes.
template <typename Link>
inline void write_blocking(Link& link, const char* data, std::size_t length)
    requires requires(Link& to) { to.write(data, length); }
{
    while (!link.write(data, length)) std::this_thread::yield();
}

}