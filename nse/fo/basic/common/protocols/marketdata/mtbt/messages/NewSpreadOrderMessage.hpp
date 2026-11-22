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

// New Spread Order Message
struct new_spread_order_message {

    struct fields_type {
        tbt_mtbt::timestamp timestamp;
        tbt_mtbt::order_id order_id;
        tbt_mtbt::token token;
        tbt_mtbt::order_type order_type;
        tbt_mtbt::price price;
        tbt_mtbt::quantity quantity;
    };

    tbt_mtbt::message_type message_type = tbt_mtbt::message_type::enum_type::new_spread_order_message;

    fields_type fields;

    // parse method
    static new_spread_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<new_spread_order_message*>(buffer);
    }

    // parse method const
    static const new_spread_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const new_spread_order_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(new_spread_order_message::fields_type, timestamp) == 0, "unexpected offset of new_spread_order_message::fields_type::timestamp");
static_assert(offsetof(new_spread_order_message::fields_type, order_id) == 8, "unexpected offset of new_spread_order_message::fields_type::order_id");
static_assert(offsetof(new_spread_order_message::fields_type, token) == 16, "unexpected offset of new_spread_order_message::fields_type::token");
static_assert(offsetof(new_spread_order_message::fields_type, order_type) == 20, "unexpected offset of new_spread_order_message::fields_type::order_type");
static_assert(offsetof(new_spread_order_message::fields_type, price) == 21, "unexpected offset of new_spread_order_message::fields_type::price");
static_assert(offsetof(new_spread_order_message::fields_type, quantity) == 25, "unexpected offset of new_spread_order_message::fields_type::quantity");
static_assert(sizeof(new_spread_order_message::fields_type) == 29, "unexpected sizeof new_spread_order_message::fields_type");
static_assert(sizeof(new_spread_order_message) == sizeof(tbt_mtbt::message_type) + 29, "unexpected sizeof new_spread_order_message");

#pragma pack(pop)
}
