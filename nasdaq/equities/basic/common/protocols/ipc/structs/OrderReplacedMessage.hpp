#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/ReplacementId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/Display.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_replaced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::replacement_id replacement_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::display display;

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
static_assert(offsetof(order_replaced_message, symbol) == 24, "unexpected offset of order_replaced_message::symbol");
static_assert(offsetof(order_replaced_message, side) == 32, "unexpected offset of order_replaced_message::side");
static_assert(offsetof(order_replaced_message, price) == 33, "unexpected offset of order_replaced_message::price");
static_assert(offsetof(order_replaced_message, quantity) == 41, "unexpected offset of order_replaced_message::quantity");
static_assert(offsetof(order_replaced_message, display) == 45, "unexpected offset of order_replaced_message::display");
static_assert(sizeof(order_replaced_message) == 46, "unexpected sizeof order_replaced_message");

#pragma pack(pop)
}
