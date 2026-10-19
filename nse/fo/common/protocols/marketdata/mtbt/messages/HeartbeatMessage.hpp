#pragma once

#include <cstddef>
#include "../types/LastSequenceNo.hpp"
#include "../types/MessageType.hpp"

namespace nse::nsefo::mtbt::binary::v7_1 {

namespace binary_mtbt = ::nse::nsefo::mtbt::binary::v7_1;

#pragma pack(push, 1)

// Heartbeat Message
struct heartbeat_message {

    struct fields_type {
        binary_mtbt::last_sequence_no last_sequence_no;
    };

    binary_mtbt::message_type message_type = binary_mtbt::message_type::enum_type::heartbeat_message;

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
static_assert(offsetof(heartbeat_message::fields_type, last_sequence_no) == 0, "unexpected offset of heartbeat_message::fields_type::last_sequence_no");
static_assert(sizeof(heartbeat_message::fields_type) == 4, "unexpected sizeof heartbeat_message::fields_type");
static_assert(sizeof(heartbeat_message) == sizeof(binary_mtbt::message_type) + 4, "unexpected sizeof heartbeat_message");

#pragma pack(pop)
}
