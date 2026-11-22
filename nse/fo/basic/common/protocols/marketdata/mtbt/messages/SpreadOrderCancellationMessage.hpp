#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Token.hpp"
#include "../types/OrderType.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/MessageType.hpp"

namespace nse::nsefo::mtbt::tbt::v7_1 {

namespace tbt_mtbt = ::nse::nsefo::mtbt::tbt::v7_1;

#pragma pack(push, 1)

// Spread Order Cancellation Message
struct spread_order_cancellation_message {

    struct fields_type {
        tbt_mtbt::timestamp timestamp;
        tbt_mtbt::order_id order_id;
        tbt_mtbt::token token;
        tbt_mtbt::order_type order_type;
        tbt_mtbt::price price;
        tbt_mtbt::quantity quantity;
    };

    tbt_mtbt::message_type message_type = tbt_mtbt::message_type::enum_type::spread_order_cancellation_message;

    fields_type fields;

    // parse method
    static spread_order_cancellation_message* parse(std::byte* buffer) {
        return reinterpret_cast<spread_order_cancellation_message*>(buffer);
    }

    // parse method const
    static const spread_order_cancellation_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_order_cancellation_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(spread_order_cancellation_message::fields_type, timestamp) == 0, "unexpected offset of spread_order_cancellation_message::fields_type::timestamp");
static_assert(offsetof(spread_order_cancellation_message::fields_type, order_id) == 8, "unexpected offset of spread_order_cancellation_message::fields_type::order_id");
static_assert(offsetof(spread_order_cancellation_message::fields_type, token) == 16, "unexpected offset of spread_order_cancellation_message::fields_type::token");
static_assert(offsetof(spread_order_cancellation_message::fields_type, order_type) == 20, "unexpected offset of spread_order_cancellation_message::fields_type::order_type");
static_assert(offsetof(spread_order_cancellation_message::fields_type, price) == 21, "unexpected offset of spread_order_cancellation_message::fields_type::price");
static_assert(offsetof(spread_order_cancellation_message::fields_type, quantity) == 25, "unexpected offset of spread_order_cancellation_message::fields_type::quantity");
static_assert(sizeof(spread_order_cancellation_message::fields_type) == 29, "unexpected sizeof spread_order_cancellation_message::fields_type");
static_assert(sizeof(spread_order_cancellation_message) == sizeof(tbt_mtbt::message_type) + 29, "unexpected sizeof spread_order_cancellation_message");

#pragma pack(pop)
}
