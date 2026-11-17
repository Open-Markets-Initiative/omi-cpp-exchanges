#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/Display.hpp"
#include "../types/Attribution.hpp"
#include "../types/CrossType.hpp"
#include "../types/Peg.hpp"
#include "../types/MinQty.hpp"
#include "../types/PegOffset.hpp"
#include "../types/PostOnly.hpp"
#include "../types/TradeNow.hpp"
#include "../types/ShortSale.hpp"
#include "../types/MaxFloor.hpp"
#include "../types/SelfMatchGroup.hpp"
#include "../types/SelfMatchLevel.hpp"
#include "../types/SelfMatchStrategy.hpp"
#include "../types/AiqStrategy.hpp"
#include "../types/ExpireTime.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct new_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::time_in_force time_in_force;
    ipc_matchingengine::display display;
    ipc_matchingengine::attribution attribution;
    ipc_matchingengine::cross_type cross_type;
    ipc_matchingengine::peg peg;
    ipc_matchingengine::min_qty min_qty;
    ipc_matchingengine::peg_offset peg_offset;
    ipc_matchingengine::post_only post_only;
    ipc_matchingengine::trade_now trade_now;
    ipc_matchingengine::short_sale short_sale;
    ipc_matchingengine::max_floor max_floor;
    ipc_matchingengine::self_match_group self_match_group;
    ipc_matchingengine::self_match_level self_match_level;
    ipc_matchingengine::self_match_strategy self_match_strategy;
    ipc_matchingengine::aiq_strategy aiq_strategy;
    ipc_matchingengine::expire_time expire_time;

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
static_assert(offsetof(new_order_message, time_in_force) == 45, "unexpected offset of new_order_message::time_in_force");
static_assert(offsetof(new_order_message, display) == 46, "unexpected offset of new_order_message::display");
static_assert(offsetof(new_order_message, attribution) == 47, "unexpected offset of new_order_message::attribution");
static_assert(offsetof(new_order_message, cross_type) == 51, "unexpected offset of new_order_message::cross_type");
static_assert(offsetof(new_order_message, peg) == 52, "unexpected offset of new_order_message::peg");
static_assert(offsetof(new_order_message, min_qty) == 53, "unexpected offset of new_order_message::min_qty");
static_assert(offsetof(new_order_message, peg_offset) == 57, "unexpected offset of new_order_message::peg_offset");
static_assert(offsetof(new_order_message, post_only) == 61, "unexpected offset of new_order_message::post_only");
static_assert(offsetof(new_order_message, trade_now) == 62, "unexpected offset of new_order_message::trade_now");
static_assert(offsetof(new_order_message, short_sale) == 63, "unexpected offset of new_order_message::short_sale");
static_assert(offsetof(new_order_message, max_floor) == 64, "unexpected offset of new_order_message::max_floor");
static_assert(offsetof(new_order_message, self_match_group) == 68, "unexpected offset of new_order_message::self_match_group");
static_assert(offsetof(new_order_message, self_match_level) == 76, "unexpected offset of new_order_message::self_match_level");
static_assert(offsetof(new_order_message, self_match_strategy) == 77, "unexpected offset of new_order_message::self_match_strategy");
static_assert(offsetof(new_order_message, aiq_strategy) == 78, "unexpected offset of new_order_message::aiq_strategy");
static_assert(offsetof(new_order_message, expire_time) == 79, "unexpected offset of new_order_message::expire_time");
static_assert(sizeof(new_order_message) == 83, "unexpected sizeof new_order_message");

#pragma pack(pop)
}
