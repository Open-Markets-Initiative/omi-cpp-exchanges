#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Display.hpp"
#include "../types/Prevented.hpp"
#include "../types/PreventedLiquidity.hpp"
#include "../types/PreventedQuantity.hpp"
#include "../types/PreventedPrice.hpp"
#include "../types/PreventedStrategy.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

// Order Canceled Message
struct order_canceled_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::order_id order_id;
        ipc_matchingengine::symbol symbol;
        ipc_matchingengine::display display;
        ipc_matchingengine::prevented prevented;
        ipc_matchingengine::prevented_liquidity prevented_liquidity;
        ipc_matchingengine::prevented_quantity prevented_quantity;
        ipc_matchingengine::prevented_price prevented_price;
        ipc_matchingengine::prevented_strategy prevented_strategy;
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
static_assert(offsetof(order_canceled_message::fields_type, symbol) == 16, "unexpected offset of order_canceled_message::fields_type::symbol");
static_assert(offsetof(order_canceled_message::fields_type, display) == 24, "unexpected offset of order_canceled_message::fields_type::display");
static_assert(offsetof(order_canceled_message::fields_type, prevented) == 25, "unexpected offset of order_canceled_message::fields_type::prevented");
static_assert(offsetof(order_canceled_message::fields_type, prevented_liquidity) == 26, "unexpected offset of order_canceled_message::fields_type::prevented_liquidity");
static_assert(offsetof(order_canceled_message::fields_type, prevented_quantity) == 27, "unexpected offset of order_canceled_message::fields_type::prevented_quantity");
static_assert(offsetof(order_canceled_message::fields_type, prevented_price) == 31, "unexpected offset of order_canceled_message::fields_type::prevented_price");
static_assert(offsetof(order_canceled_message::fields_type, prevented_strategy) == 39, "unexpected offset of order_canceled_message::fields_type::prevented_strategy");
static_assert(sizeof(order_canceled_message::fields_type) == 40, "unexpected sizeof order_canceled_message::fields_type");
static_assert(sizeof(order_canceled_message) == sizeof(ipc_matchingengine::message_header) + 40, "unexpected sizeof order_canceled_message");

#pragma pack(pop)
}
