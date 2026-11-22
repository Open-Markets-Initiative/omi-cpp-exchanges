#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Token.hpp"
#include "../types/Announcement.hpp"

namespace nse::nsefo::basicmatchingengine::ipc::v2026 {

namespace ipc_basicmatchingengine = ::nse::nsefo::basicmatchingengine::ipc::v2026;

#pragma pack(push, 1)

struct phase_announced_message {

    ipc_basicmatchingengine::timestamp timestamp;
    ipc_basicmatchingengine::token token;
    ipc_basicmatchingengine::announcement announcement;

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
static_assert(offsetof(phase_announced_message, token) == 8, "unexpected offset of phase_announced_message::token");
static_assert(offsetof(phase_announced_message, announcement) == 12, "unexpected offset of phase_announced_message::announcement");
static_assert(sizeof(phase_announced_message) == 13, "unexpected sizeof phase_announced_message");

#pragma pack(pop)
}
