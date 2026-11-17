#pragma once

#include "Components.hpp"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace sim {

// A link between two processes: the same bounded ring of framed messages that Queue is, with
// its counters and its cells in a region both processes map.
//
// The layout is shared memory, so it is a plain aggregate: no vector, no string, nothing that
// holds a pointer into one process's address space. Both ends map the identical type, which is
// safe here because both ends are generated from the same models by the same generator — a ring
// is only ever read by a binary built from the same tree that wrote it.
template <std::size_t Slot, std::size_t Slots>
class SharedRing final : public Link {
    static_assert((Slots & (Slots - 1)) == 0, "slot count must be a power of two");

public:
    static constexpr std::size_t slot_bytes = Slot;

    // Which end of the link this process is. Nothing here enforces it — it names what the
    // process intends, so a mistake reads as one.
    enum class End { Writes, Reads };

    // Attaches to a ring that already exists. It has to already exist: the links between an
    // exchange's components are not a line but a cycle - the gateway writes to the engine and
    // the engine writes back, and a trader likewise - so there is no order to start them in
    // where every reader finds its writer already up. Whoever launches the venue creates every
    // ring first, with create() below, and each component then only ever attaches.
    //
    // `name` is the link's name as the single-process venue spells it ("gateway -> engine"); the
    // shared object is named after it, so the two versions of the venue talk about the same link
    // in the same words.
    SharedRing(std::string name, End end) : name_(std::move(name)), end_(end) {
        const auto object = object_name(name_);

        descriptor_ = ::shm_open(object.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        if (descriptor_ < 0)
            throw std::runtime_error("ring '" + name_ + "': " + object
                                     + " is not there — the venue's rings are created before its components start");

        void* mapped = ::mmap(nullptr, sizeof(Layout), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor_, 0);
        if (mapped == MAP_FAILED) {
            ::close(descriptor_);
            throw std::runtime_error("ring '" + name_ + "': cannot map " + object);
        }

        layout_ = static_cast<Layout*>(mapped);

        // One writer and one reader is the whole contract, and a ring that let a second of either
        // attach would break quietly: two readers each take some of the messages and neither sees
        // the stream. So the end is claimed, and a second claim fails here rather than later.
        auto& taken = end == End::Writes ? layout_->writer : layout_->reader;

        bool free = false;
        if (!taken.compare_exchange_strong(free, true, std::memory_order_acq_rel)) {
            ::munmap(layout_, sizeof(Layout));
            ::close(descriptor_);
            layout_ = nullptr;
            descriptor_ = -1;
            throw std::runtime_error("ring '" + name_ + "': its "
                                     + (end == End::Writes ? "writer" : "reader")
                                     + " end is already taken — a link joins two components, not three");
        }
    }

    SharedRing(const SharedRing&)            = delete;
    SharedRing& operator=(const SharedRing&) = delete;

    // The mapping goes; the object stays, so a component can be restarted against a ring its
    // peer is still holding. unlink() is how a run is cleaned up, and only the writer does it.
    ~SharedRing() override {
        if (layout_ != nullptr) {
            // Give the end back, so a component can be restarted against a ring its peer still holds.
            (end_ == End::Writes ? layout_->writer : layout_->reader).store(false, std::memory_order_release);
            ::munmap(layout_, sizeof(Layout));
        }
        if (descriptor_ >= 0) { ::close(descriptor_); }
    }

    const std::string& name() const override { return name_; }

    // Writer side, as Queue's is: false when the reader is far enough behind that the ring is
    // full, and the caller decides whether to retry or drop.
    bool write(const char* data, std::size_t length) {
        if (length > Slot) return false;

        const auto written = layout_->written.load(std::memory_order_relaxed);
        if (written - layout_->read.load(std::memory_order_acquire) >= Slots) return false;

        auto& slot = layout_->cells[written & (Slots - 1)];
        std::memcpy(slot.bytes, data, length);
        slot.length = length;
        layout_->written.store(written + 1, std::memory_order_release);
        return true;
    }

    // Reader side: hands one message to `reader`. False when the ring is empty.
    template <typename Reader>
    bool read(Reader&& reader) {
        const auto read = layout_->read.load(std::memory_order_relaxed);
        if (read == layout_->written.load(std::memory_order_acquire)) return false;

        const auto& slot = layout_->cells[read & (Slots - 1)];
        reader(std::string_view(slot.bytes, slot.length));
        layout_->read.store(read + 1, std::memory_order_release);
        return true;
    }

    bool          drained() const override { return layout_->read.load(std::memory_order_acquire) == layout_->written.load(std::memory_order_acquire); }
    std::uint64_t written() const { return layout_->written.load(std::memory_order_relaxed); }
    std::uint64_t read()    const { return layout_->read.load(std::memory_order_relaxed); }

    // Creates the ring, empty, and leaves it there for the components to attach to. Done once
    // per link before anything starts, which is also what makes a run reproducible: a ring left
    // over from a previous run would otherwise be attached to with its counters where they
    // stopped.
    static void create(const std::string& name) {
        const auto object = object_name(name);

        ::shm_unlink(object.c_str());

        const int descriptor = ::shm_open(object.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
        if (descriptor < 0)
            throw std::runtime_error("ring '" + name + "': cannot create " + object);

        // Zero filled by the kernel, so both counters start where a fresh Queue's do.
        if (::ftruncate(descriptor, sizeof(Layout)) != 0) {
            ::close(descriptor);
            ::shm_unlink(object.c_str());
            throw std::runtime_error("ring '" + name + "': cannot size " + object);
        }

        ::close(descriptor);
    }

    // Removes the shared object. A mapping already taken stays valid until its holder unmaps,
    // which is what lets a run clean up without waiting for every component to have exited.
    static void discard(const std::string& name) { ::shm_unlink(object_name(name).c_str()); }

private:
    // The region both processes map. Trivially copyable throughout, and the counters sit on
    // their own cache lines exactly as Queue's do.
    struct Cell {
        char        bytes[Slot];
        std::size_t length;
    };

    struct Layout {
        alignas(64) std::atomic<std::uint64_t> written;
        alignas(64) std::atomic<std::uint64_t> read;

        // Which ends are claimed. Zero filled on creation, so both start free.
        alignas(64) std::atomic<bool>          writer;
        std::atomic<bool>                      reader;

        alignas(64) Cell                       cells[Slots];
    };

    static_assert(std::atomic<std::uint64_t>::is_always_lock_free,
                  "a ring between processes needs lock-free counters: a lock would live in one process's memory");

    // A link's name as a shared object: "/" and then the name with anything but a letter or a
    // digit turned into an underscore, because a shared object name is one path segment.
    static std::string object_name(const std::string& name) {
        std::string object = "/";
        for (const char letter : name)
            object += (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z')
                   || (letter >= '0' && letter <= '9') ? letter : '_';
        return object;
    }

    std::string name_;
    End         end_;
    int         descriptor_ = -1;
    Layout*     layout_     = nullptr;
};

// The same two link kinds the single-process venue has, between processes instead.
using IpcRing     = SharedRing<ipc_slot_bytes, 4096>;
using MessageRing = SharedRing<512, 4096>;

}