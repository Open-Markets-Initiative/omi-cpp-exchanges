#pragma once

#include "protocols/ipc/IpcBus.hpp"

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <utility>

namespace sim::components {

// The sequencer of one matching engine: everything bound for the engine - each gateway's orders and
// cancels, the day's requests - put into one order, the order it reads them in, each stamped with its
// place in that order and the time it was put there, and handed to the engine on one link. Whatever
// reached it from many producers at once reaches the engine one after another, in one order the
// engine and everything after it agree on.
template <typename Emit>
class Sequencer {
public:
    explicit Sequencer(Emit emit) : emit_(std::move(emit)) {}

    Sequencer(const Sequencer&)            = delete;
    Sequencer& operator=(const Sequencer&) = delete;

    // One pass: at most one message off each link it reads, in the order they are given, so no
    // producer waits behind another - each numbered and handed on. False when none had one, which
    // is what tells a component thread to back off.
    template <typename... Inbound>
    bool step(Inbound&... inbound) {
        bool worked = false;
        ((worked = take(inbound) || worked), ...);
        return worked;
    }

    // The last place it gave: how many messages it has put in its engine's order.
    std::uint64_t sequenced() const { return sequence_; }

private:
    template <typename Inbound>
    bool take(Inbound& inbound) {
        return inbound.read([this](std::string_view message) {
            frame_.assign(message.data(), message.size());
            if (!sim::stamp_sequence(frame_.data(), frame_.size(), sequence_ + 1, sim::send_time_nanos())) {
                // A frame of no kind an engine is sent: handed on as it is, and said so.
                std::printf("UNSEQUENCED frame of kind '%c' handed to the engine as it is\n", message.size() > 2 ? message[2] : '?');
                emit_(frame_.data(), frame_.size());
                return;
            }
            ++sequence_;
            emit_(frame_.data(), frame_.size());
        });
    }

    Emit          emit_;
    std::string   frame_;          // the message being stamped, kept so it costs no allocation once grown
    std::uint64_t sequence_ = 0;   // the last place it gave
};

}