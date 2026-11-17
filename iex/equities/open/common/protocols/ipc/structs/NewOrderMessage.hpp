#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/OrderType.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/Display.hpp"
#include "../types/CrossType.hpp"
#include "../types/Peg.hpp"
#include "../types/Instruction.hpp"
#include "../types/ExpireTime.hpp"
#include "../types/PegDifference.hpp"
#include "../types/MinQty.hpp"
#include "../types/SelfMatchGroup.hpp"
#include "../types/SelfMatchModifier.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct new_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::order_type order_type;
    ipc_matchingengine::time_in_force time_in_force;
    ipc_matchingengine::display display;
    ipc_matchingengine::cross_type cross_type;
    ipc_matchingengine::peg peg;
    ipc_matchingengine::instruction instruction;
    ipc_matchingengine::expire_time expire_time;
    ipc_matchingengine::peg_difference peg_difference;
    ipc_matchingengine::min_qty min_qty;
    ipc_matchingengine::self_match_group self_match_group;
    ipc_matchingengine::self_match_modifier self_match_modifier;

    // parse method
    static new_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<new_order_message*>(buffer);
    }

    // parse method const
    static const new_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const new_order_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(new_order_message, timestamp) == 0, "unexpected offset of new_order_message::timestamp");
static_assert(offsetof(new_order_message, sequence) == 8, "unexpected offset of new_order_message::sequence");
static_assert(offsetof(new_order_message, order_id) == 16, "unexpected offset of new_order_message::order_id");
static_assert(offsetof(new_order_message, symbol) == 24, "unexpected offset of new_order_message::symbol");
static_assert(offsetof(new_order_message, side) == 32, "unexpected offset of new_order_message::side");
static_assert(offsetof(new_order_message, price) == 33, "unexpected offset of new_order_message::price");
static_assert(offsetof(new_order_message, quantity) == 41, "unexpected offset of new_order_message::quantity");
static_assert(offsetof(new_order_message, order_type) == 45, "unexpected offset of new_order_message::order_type");
static_assert(offsetof(new_order_message, time_in_force) == 46, "unexpected offset of new_order_message::time_in_force");
static_assert(offsetof(new_order_message, display) == 47, "unexpected offset of new_order_message::display");
static_assert(offsetof(new_order_message, cross_type) == 48, "unexpected offset of new_order_message::cross_type");
static_assert(offsetof(new_order_message, peg) == 49, "unexpected offset of new_order_message::peg");
static_assert(offsetof(new_order_message, instruction) == 50, "unexpected offset of new_order_message::instruction");
static_assert(offsetof(new_order_message, expire_time) == 51, "unexpected offset of new_order_message::expire_time");
static_assert(offsetof(new_order_message, peg_difference) == 59, "unexpected offset of new_order_message::peg_difference");
static_assert(offsetof(new_order_message, min_qty) == 67, "unexpected offset of new_order_message::min_qty");
static_assert(offsetof(new_order_message, self_match_group) == 71, "unexpected offset of new_order_message::self_match_group");
static_assert(offsetof(new_order_message, self_match_modifier) == 79, "unexpected offset of new_order_message::self_match_modifier");
static_assert(sizeof(new_order_message) == 80, "unexpected sizeof new_order_message");

#pragma pack(pop)
}
