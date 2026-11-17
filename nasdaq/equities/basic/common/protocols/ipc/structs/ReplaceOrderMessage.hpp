#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/OrderId.hpp"
#include "../types/ReplacementId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/Display.hpp"
#include "../types/CrossType.hpp"
#include "../types/Peg.hpp"
#include "../types/MinQty.hpp"
#include "../types/PegOffset.hpp"
#include "../types/PostOnly.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct replace_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::replacement_id replacement_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::time_in_force time_in_force;
    ipc_matchingengine::display display;
    ipc_matchingengine::cross_type cross_type;
    ipc_matchingengine::peg peg;
    ipc_matchingengine::min_qty min_qty;
    ipc_matchingengine::peg_offset peg_offset;
    ipc_matchingengine::post_only post_only;

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
static_assert(offsetof(replace_order_message, symbol) == 32, "unexpected offset of replace_order_message::symbol");
static_assert(offsetof(replace_order_message, side) == 40, "unexpected offset of replace_order_message::side");
static_assert(offsetof(replace_order_message, price) == 41, "unexpected offset of replace_order_message::price");
static_assert(offsetof(replace_order_message, quantity) == 49, "unexpected offset of replace_order_message::quantity");
static_assert(offsetof(replace_order_message, time_in_force) == 53, "unexpected offset of replace_order_message::time_in_force");
static_assert(offsetof(replace_order_message, display) == 54, "unexpected offset of replace_order_message::display");
static_assert(offsetof(replace_order_message, cross_type) == 55, "unexpected offset of replace_order_message::cross_type");
static_assert(offsetof(replace_order_message, peg) == 56, "unexpected offset of replace_order_message::peg");
static_assert(offsetof(replace_order_message, min_qty) == 57, "unexpected offset of replace_order_message::min_qty");
static_assert(offsetof(replace_order_message, peg_offset) == 61, "unexpected offset of replace_order_message::peg_offset");
static_assert(offsetof(replace_order_message, post_only) == 65, "unexpected offset of replace_order_message::post_only");
static_assert(sizeof(replace_order_message) == 66, "unexpected sizeof replace_order_message");

#pragma pack(pop)
}
