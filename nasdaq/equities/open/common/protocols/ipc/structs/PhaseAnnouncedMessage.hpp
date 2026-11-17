#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Announcement.hpp"
#include "../types/Reason.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct phase_announced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::announcement announcement;
    ipc_matchingengine::reason reason;

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
static_assert(offsetof(phase_announced_message, reason) == 17, "unexpected offset of phase_announced_message::reason");
static_assert(sizeof(phase_announced_message) == 21, "unexpected sizeof phase_announced_message");

#pragma pack(pop)
}
