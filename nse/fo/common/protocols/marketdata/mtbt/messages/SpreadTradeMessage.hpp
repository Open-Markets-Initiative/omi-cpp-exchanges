#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/BuyOrderId.hpp"
#include "../types/SellOrderId.hpp"
#include "../types/Token.hpp"
#include "../types/TradePrice.hpp"
#include "../types/Quantity.hpp"
#include "../types/MessageType.hpp"

namespace nse::nsefo::mtbt::binary::v7_1 {

namespace binary_mtbt = ::nse::nsefo::mtbt::binary::v7_1;

#pragma pack(push, 1)

// Spread Trade Message
struct spread_trade_message {

    struct fields_type {
        binary_mtbt::timestamp timestamp;
        binary_mtbt::buy_order_id buy_order_id;
        binary_mtbt::sell_order_id sell_order_id;
        binary_mtbt::token token;
        binary_mtbt::trade_price trade_price;
        binary_mtbt::quantity quantity;
    };

    binary_mtbt::message_type message_type = binary_mtbt::message_type::enum_type::spread_trade_message;

    fields_type fields;

    // parse method
    static spread_trade_message* parse(std::byte* buffer) {
        return reinterpret_cast<spread_trade_message*>(buffer);
    }

    // parse method const
    static const spread_trade_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_trade_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(spread_trade_message::fields_type, timestamp) == 0, "unexpected offset of spread_trade_message::fields_type::timestamp");
static_assert(offsetof(spread_trade_message::fields_type, buy_order_id) == 8, "unexpected offset of spread_trade_message::fields_type::buy_order_id");
static_assert(offsetof(spread_trade_message::fields_type, sell_order_id) == 16, "unexpected offset of spread_trade_message::fields_type::sell_order_id");
static_assert(offsetof(spread_trade_message::fields_type, token) == 24, "unexpected offset of spread_trade_message::fields_type::token");
static_assert(offsetof(spread_trade_message::fields_type, trade_price) == 28, "unexpected offset of spread_trade_message::fields_type::trade_price");
static_assert(offsetof(spread_trade_message::fields_type, quantity) == 32, "unexpected offset of spread_trade_message::fields_type::quantity");
static_assert(sizeof(spread_trade_message::fields_type) == 36, "unexpected sizeof spread_trade_message::fields_type");
static_assert(sizeof(spread_trade_message) == sizeof(binary_mtbt::message_type) + 36, "unexpected sizeof spread_trade_message");

#pragma pack(pop)
}
