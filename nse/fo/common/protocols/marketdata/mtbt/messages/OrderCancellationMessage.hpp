#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Token.hpp"
#include "../types/OrderType.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/MessageType.hpp"

namespace nse::nsefo::mtbt::binary::v7_1 {

namespace binary_mtbt = ::nse::nsefo::mtbt::binary::v7_1;

#pragma pack(push, 1)

// Order Cancellation Message
struct order_cancellation_message {

    struct fields_type {
        binary_mtbt::timestamp timestamp;
        binary_mtbt::order_id order_id;
        binary_mtbt::token token;
        binary_mtbt::order_type order_type;
        binary_mtbt::price price;
        binary_mtbt::quantity quantity;
    };

    binary_mtbt::message_type message_type = binary_mtbt::message_type::enum_type::order_cancellation_message;

    fields_type fields;

    // parse method
    static order_cancellation_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_cancellation_message*>(buffer);
    }

    // parse method const
    static const order_cancellation_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_cancellation_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_cancellation_message::fields_type, timestamp) == 0, "unexpected offset of order_cancellation_message::fields_type::timestamp");
static_assert(offsetof(order_cancellation_message::fields_type, order_id) == 8, "unexpected offset of order_cancellation_message::fields_type::order_id");
static_assert(offsetof(order_cancellation_message::fields_type, token) == 16, "unexpected offset of order_cancellation_message::fields_type::token");
static_assert(offsetof(order_cancellation_message::fields_type, order_type) == 20, "unexpected offset of order_cancellation_message::fields_type::order_type");
static_assert(offsetof(order_cancellation_message::fields_type, price) == 21, "unexpected offset of order_cancellation_message::fields_type::price");
static_assert(offsetof(order_cancellation_message::fields_type, quantity) == 25, "unexpected offset of order_cancellation_message::fields_type::quantity");
static_assert(sizeof(order_cancellation_message::fields_type) == 29, "unexpected sizeof order_cancellation_message::fields_type");
static_assert(sizeof(order_cancellation_message) == sizeof(binary_mtbt::message_type) + 29, "unexpected sizeof order_cancellation_message");

#pragma pack(pop)
}
