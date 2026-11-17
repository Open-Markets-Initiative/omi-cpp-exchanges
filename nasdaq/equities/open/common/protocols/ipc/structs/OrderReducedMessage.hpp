#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Quantity.hpp"
#include "../types/Display.hpp"
#include "../types/Prevented.hpp"
#include "../types/PreventedLiquidity.hpp"
#include "../types/PreventedQuantity.hpp"
#include "../types/PreventedPrice.hpp"
#include "../types/PreventedStrategy.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_reduced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::display display;
    ipc_matchingengine::prevented prevented;
    ipc_matchingengine::prevented_liquidity prevented_liquidity;
    ipc_matchingengine::prevented_quantity prevented_quantity;
    ipc_matchingengine::prevented_price prevented_price;
    ipc_matchingengine::prevented_strategy prevented_strategy;

    // parse method
    static order_reduced_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_reduced_message*>(buffer);
    }

    // parse method const
    static const order_reduced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_reduced_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_reduced_message, timestamp) == 0, "unexpected offset of order_reduced_message::timestamp");
static_assert(offsetof(order_reduced_message, order_id) == 8, "unexpected offset of order_reduced_message::order_id");
static_assert(offsetof(order_reduced_message, symbol) == 16, "unexpected offset of order_reduced_message::symbol");
static_assert(offsetof(order_reduced_message, quantity) == 24, "unexpected offset of order_reduced_message::quantity");
static_assert(offsetof(order_reduced_message, display) == 28, "unexpected offset of order_reduced_message::display");
static_assert(offsetof(order_reduced_message, prevented) == 29, "unexpected offset of order_reduced_message::prevented");
static_assert(offsetof(order_reduced_message, prevented_liquidity) == 30, "unexpected offset of order_reduced_message::prevented_liquidity");
static_assert(offsetof(order_reduced_message, prevented_quantity) == 31, "unexpected offset of order_reduced_message::prevented_quantity");
static_assert(offsetof(order_reduced_message, prevented_price) == 35, "unexpected offset of order_reduced_message::prevented_price");
static_assert(offsetof(order_reduced_message, prevented_strategy) == 43, "unexpected offset of order_reduced_message::prevented_strategy");
static_assert(sizeof(order_reduced_message) == 44, "unexpected sizeof order_reduced_message");

#pragma pack(pop)
}
