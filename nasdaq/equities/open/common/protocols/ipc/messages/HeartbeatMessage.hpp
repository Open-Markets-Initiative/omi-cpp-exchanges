#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

// Heartbeat Message
struct heartbeat_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

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
static_assert(offsetof(heartbeat_message::fields_type, timestamp) == 0, "unexpected offset of heartbeat_message::fields_type::timestamp");
static_assert(sizeof(heartbeat_message::fields_type) == 8, "unexpected sizeof heartbeat_message::fields_type");
static_assert(sizeof(heartbeat_message) == sizeof(ipc_matchingengine::message_header) + 8, "unexpected sizeof heartbeat_message");

#pragma pack(pop)
}
