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
#include "../types/RandomReserve.hpp"
#include "../types/DiscretionPrice.hpp"
#include "../types/DiscretionType.hpp"
#include "../types/SelfMatchGroup.hpp"
#include "../types/SelfMatchLevel.hpp"
#include "../types/SelfMatchStrategy.hpp"
#include "../types/AiqStrategy.hpp"
#include "../types/ExpireTime.hpp"
#include "../types/ImbalanceOnly.hpp"
#include "../types/LateLimit.hpp"
#include "../types/ProtectionPercent.hpp"
#include "../types/ProtectionAmount.hpp"
#include "../types/ExtendedClose.hpp"
#include "../types/ExtendedLifeBook.hpp"

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
    ipc_matchingengine::random_reserve random_reserve;
    ipc_matchingengine::discretion_price discretion_price;
    ipc_matchingengine::discretion_type discretion_type;
    ipc_matchingengine::self_match_group self_match_group;
    ipc_matchingengine::self_match_level self_match_level;
    ipc_matchingengine::self_match_strategy self_match_strategy;
    ipc_matchingengine::aiq_strategy aiq_strategy;
    ipc_matchingengine::expire_time expire_time;
    ipc_matchingengine::imbalance_only imbalance_only;
    ipc_matchingengine::late_limit late_limit;
    ipc_matchingengine::protection_percent protection_percent;
    ipc_matchingengine::protection_amount protection_amount;
    ipc_matchingengine::extended_close extended_close;
    ipc_matchingengine::extended_life_book extended_life_book;

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
static_assert(offsetof(new_order_message, random_reserve) == 68, "unexpected offset of new_order_message::random_reserve");
static_assert(offsetof(new_order_message, discretion_price) == 72, "unexpected offset of new_order_message::discretion_price");
static_assert(offsetof(new_order_message, discretion_type) == 80, "unexpected offset of new_order_message::discretion_type");
static_assert(offsetof(new_order_message, self_match_group) == 81, "unexpected offset of new_order_message::self_match_group");
static_assert(offsetof(new_order_message, self_match_level) == 89, "unexpected offset of new_order_message::self_match_level");
static_assert(offsetof(new_order_message, self_match_strategy) == 90, "unexpected offset of new_order_message::self_match_strategy");
static_assert(offsetof(new_order_message, aiq_strategy) == 91, "unexpected offset of new_order_message::aiq_strategy");
static_assert(offsetof(new_order_message, expire_time) == 92, "unexpected offset of new_order_message::expire_time");
static_assert(offsetof(new_order_message, imbalance_only) == 96, "unexpected offset of new_order_message::imbalance_only");
static_assert(offsetof(new_order_message, late_limit) == 97, "unexpected offset of new_order_message::late_limit");
static_assert(offsetof(new_order_message, protection_percent) == 98, "unexpected offset of new_order_message::protection_percent");
static_assert(offsetof(new_order_message, protection_amount) == 102, "unexpected offset of new_order_message::protection_amount");
static_assert(offsetof(new_order_message, extended_close) == 110, "unexpected offset of new_order_message::extended_close");
static_assert(offsetof(new_order_message, extended_life_book) == 111, "unexpected offset of new_order_message::extended_life_book");
static_assert(sizeof(new_order_message) == 112, "unexpected sizeof new_order_message");

#pragma pack(pop)
}
