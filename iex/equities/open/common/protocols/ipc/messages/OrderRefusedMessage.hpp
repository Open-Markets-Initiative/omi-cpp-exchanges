#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

// Order Refused Message
struct order_refused_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::order_id order_id;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

    // parse method
    static order_refused_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_refused_message*>(buffer);
    }

    // parse method const
    static const order_refused_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_refused_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_refused_message::fields_type, timestamp) == 0, "unexpected offset of order_refused_message::fields_type::timestamp");
static_assert(offsetof(order_refused_message::fields_type, order_id) == 8, "unexpected offset of order_refused_message::fields_type::order_id");
static_assert(sizeof(order_refused_message::fields_type) == 16, "unexpected sizeof order_refused_message::fields_type");
static_assert(sizeof(order_refused_message) == sizeof(ipc_matchingengine::message_header) + 16, "unexpected sizeof order_refused_message");

#pragma pack(pop)
}
