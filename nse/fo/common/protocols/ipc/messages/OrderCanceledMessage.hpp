#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Token.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"

namespace nse::nsefo::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::nse::nsefo::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

// Order Canceled Message
struct order_canceled_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::order_id order_id;
        ipc_matchingengine::token token;
        ipc_matchingengine::side side;
        ipc_matchingengine::price price;
        ipc_matchingengine::quantity quantity;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

    // parse method
    static order_canceled_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_canceled_message*>(buffer);
    }

    // parse method const
    static const order_canceled_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_canceled_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_canceled_message::fields_type, timestamp) == 0, "unexpected offset of order_canceled_message::fields_type::timestamp");
static_assert(offsetof(order_canceled_message::fields_type, order_id) == 8, "unexpected offset of order_canceled_message::fields_type::order_id");
static_assert(offsetof(order_canceled_message::fields_type, token) == 16, "unexpected offset of order_canceled_message::fields_type::token");
static_assert(offsetof(order_canceled_message::fields_type, side) == 20, "unexpected offset of order_canceled_message::fields_type::side");
static_assert(offsetof(order_canceled_message::fields_type, price) == 21, "unexpected offset of order_canceled_message::fields_type::price");
static_assert(offsetof(order_canceled_message::fields_type, quantity) == 29, "unexpected offset of order_canceled_message::fields_type::quantity");
static_assert(sizeof(order_canceled_message::fields_type) == 33, "unexpected sizeof order_canceled_message::fields_type");
static_assert(sizeof(order_canceled_message) == sizeof(ipc_matchingengine::message_header) + 33, "unexpected sizeof order_canceled_message");

#pragma pack(pop)
}
