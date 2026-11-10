#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Announcement.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

struct phase_announced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::announcement announcement;

    // parse method
    static phase_announced_message* parse(std::byte* buffer) {
        return reinterpret_cast<phase_announced_message*>(buffer);
    }

    // parse method const
    static const phase_announced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const phase_announced_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(phase_announced_message, timestamp) == 0, "unexpected offset of phase_announced_message::timestamp");
static_assert(offsetof(phase_announced_message, symbol) == 8, "unexpected offset of phase_announced_message::symbol");
static_assert(offsetof(phase_announced_message, announcement) == 16, "unexpected offset of phase_announced_message::announcement");
static_assert(sizeof(phase_announced_message) == 17, "unexpected sizeof phase_announced_message");

#pragma pack(pop)
}
