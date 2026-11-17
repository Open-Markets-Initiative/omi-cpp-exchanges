#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Display.hpp"
#include "../types/Prevented.hpp"
#include "../types/PreventedLiquidity.hpp"
#include "../types/PreventedDisplay.hpp"
#include "../types/PreventedQuantity.hpp"
#include "../types/PreventedPrice.hpp"
#include "../types/PreventedAgainst.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_canceled_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::display display;
    ipc_matchingengine::prevented prevented;
    ipc_matchingengine::prevented_liquidity prevented_liquidity;
    ipc_matchingengine::prevented_display prevented_display;
    ipc_matchingengine::prevented_quantity prevented_quantity;
    ipc_matchingengine::prevented_price prevented_price;
    ipc_matchingengine::prevented_against prevented_against;

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
static_assert(offsetof(order_canceled_message, timestamp) == 0, "unexpected offset of order_canceled_message::timestamp");
static_assert(offsetof(order_canceled_message, order_id) == 8, "unexpected offset of order_canceled_message::order_id");
static_assert(offsetof(order_canceled_message, display) == 16, "unexpected offset of order_canceled_message::display");
static_assert(offsetof(order_canceled_message, prevented) == 17, "unexpected offset of order_canceled_message::prevented");
static_assert(offsetof(order_canceled_message, prevented_liquidity) == 18, "unexpected offset of order_canceled_message::prevented_liquidity");
static_assert(offsetof(order_canceled_message, prevented_display) == 19, "unexpected offset of order_canceled_message::prevented_display");
static_assert(offsetof(order_canceled_message, prevented_quantity) == 20, "unexpected offset of order_canceled_message::prevented_quantity");
static_assert(offsetof(order_canceled_message, prevented_price) == 24, "unexpected offset of order_canceled_message::prevented_price");
static_assert(offsetof(order_canceled_message, prevented_against) == 32, "unexpected offset of order_canceled_message::prevented_against");
static_assert(sizeof(order_canceled_message) == 40, "unexpected sizeof order_canceled_message");

#pragma pack(pop)
}
