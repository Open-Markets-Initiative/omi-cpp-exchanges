#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/ReplacementId.hpp"
#include "../types/Orderbook.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"

namespace odx::odxequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_replaced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::replacement_id replacement_id;
    ipc_matchingengine::orderbook orderbook;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;

    // parse method
    static order_replaced_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_replaced_message*>(buffer);
    }

    // parse method const
    static const order_replaced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_replaced_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_replaced_message, timestamp) == 0, "unexpected offset of order_replaced_message::timestamp");
static_assert(offsetof(order_replaced_message, order_id) == 8, "unexpected offset of order_replaced_message::order_id");
static_assert(offsetof(order_replaced_message, replacement_id) == 16, "unexpected offset of order_replaced_message::replacement_id");
static_assert(offsetof(order_replaced_message, orderbook) == 24, "unexpected offset of order_replaced_message::orderbook");
static_assert(offsetof(order_replaced_message, side) == 28, "unexpected offset of order_replaced_message::side");
static_assert(offsetof(order_replaced_message, price) == 29, "unexpected offset of order_replaced_message::price");
static_assert(offsetof(order_replaced_message, quantity) == 33, "unexpected offset of order_replaced_message::quantity");
static_assert(sizeof(order_replaced_message) == 37, "unexpected sizeof order_replaced_message");

#pragma pack(pop)
}
