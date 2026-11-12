#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Orderbook.hpp"
#include "../types/Announcement.hpp"

namespace odx::odxsecuritytoken::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxsecuritytoken::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct phase_announced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::orderbook orderbook;
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
static_assert(offsetof(phase_announced_message, orderbook) == 8, "unexpected offset of phase_announced_message::orderbook");
static_assert(offsetof(phase_announced_message, announcement) == 12, "unexpected offset of phase_announced_message::announcement");
static_assert(sizeof(phase_announced_message) == 13, "unexpected sizeof phase_announced_message");

#pragma pack(pop)
}
