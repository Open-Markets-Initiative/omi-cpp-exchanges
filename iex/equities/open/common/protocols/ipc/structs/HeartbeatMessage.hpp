#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

struct heartbeat_message {

    ipc_matchingengine::timestamp timestamp;

    // parse method
    static heartbeat_message* parse(std::byte* buffer) {
        return reinterpret_cast<heartbeat_message*>(buffer);
    }

    // parse method const
    static const heartbeat_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const heartbeat_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(heartbeat_message, timestamp) == 0, "unexpected offset of heartbeat_message::timestamp");
static_assert(sizeof(heartbeat_message) == 8, "unexpected sizeof heartbeat_message");

#pragma pack(pop)
}
