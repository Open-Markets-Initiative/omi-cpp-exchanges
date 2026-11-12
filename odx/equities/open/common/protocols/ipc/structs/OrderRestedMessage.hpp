#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Orderbook.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"

namespace odx::odxequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_rested_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::orderbook orderbook;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;

    // parse method
    static order_rested_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_rested_message*>(buffer);
    }

    // parse method const
    static const order_rested_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_rested_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_rested_message, timestamp) == 0, "unexpected offset of order_rested_message::timestamp");
static_assert(offsetof(order_rested_message, order_id) == 8, "unexpected offset of order_rested_message::order_id");
static_assert(offsetof(order_rested_message, orderbook) == 16, "unexpected offset of order_rested_message::orderbook");
static_assert(offsetof(order_rested_message, side) == 20, "unexpected offset of order_rested_message::side");
static_assert(offsetof(order_rested_message, price) == 21, "unexpected offset of order_rested_message::price");
static_assert(offsetof(order_rested_message, quantity) == 25, "unexpected offset of order_rested_message::quantity");
static_assert(sizeof(order_rested_message) == 29, "unexpected sizeof order_rested_message");

#pragma pack(pop)
}
