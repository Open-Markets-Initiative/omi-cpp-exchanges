#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/OrderId.hpp"
#include "../types/ReplacementId.hpp"
#include "../types/Orderbook.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/TimeInForce.hpp"

namespace odx::odxequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct replace_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::replacement_id replacement_id;
    ipc_matchingengine::orderbook orderbook;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::time_in_force time_in_force;

    // parse method
    static replace_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<replace_order_message*>(buffer);
    }

    // parse method const
    static const replace_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const replace_order_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(replace_order_message, timestamp) == 0, "unexpected offset of replace_order_message::timestamp");
static_assert(offsetof(replace_order_message, sequence) == 8, "unexpected offset of replace_order_message::sequence");
static_assert(offsetof(replace_order_message, order_id) == 16, "unexpected offset of replace_order_message::order_id");
static_assert(offsetof(replace_order_message, replacement_id) == 24, "unexpected offset of replace_order_message::replacement_id");
static_assert(offsetof(replace_order_message, orderbook) == 32, "unexpected offset of replace_order_message::orderbook");
static_assert(offsetof(replace_order_message, side) == 36, "unexpected offset of replace_order_message::side");
static_assert(offsetof(replace_order_message, price) == 37, "unexpected offset of replace_order_message::price");
static_assert(offsetof(replace_order_message, quantity) == 41, "unexpected offset of replace_order_message::quantity");
static_assert(offsetof(replace_order_message, time_in_force) == 45, "unexpected offset of replace_order_message::time_in_force");
static_assert(sizeof(replace_order_message) == 49, "unexpected sizeof replace_order_message");

#pragma pack(pop)
}
