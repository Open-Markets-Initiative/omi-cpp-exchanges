#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
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

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

// New Order Message
struct new_order_message {

    struct fields_type {
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
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

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
static_assert(offsetof(new_order_message::fields_type, timestamp) == 0, "unexpected offset of new_order_message::fields_type::timestamp");
static_assert(offsetof(new_order_message::fields_type, sequence) == 8, "unexpected offset of new_order_message::fields_type::sequence");
static_assert(offsetof(new_order_message::fields_type, order_id) == 16, "unexpected offset of new_order_message::fields_type::order_id");
static_assert(offsetof(new_order_message::fields_type, symbol) == 24, "unexpected offset of new_order_message::fields_type::symbol");
static_assert(offsetof(new_order_message::fields_type, side) == 32, "unexpected offset of new_order_message::fields_type::side");
static_assert(offsetof(new_order_message::fields_type, price) == 33, "unexpected offset of new_order_message::fields_type::price");
static_assert(offsetof(new_order_message::fields_type, quantity) == 41, "unexpected offset of new_order_message::fields_type::quantity");
static_assert(offsetof(new_order_message::fields_type, time_in_force) == 45, "unexpected offset of new_order_message::fields_type::time_in_force");
static_assert(offsetof(new_order_message::fields_type, display) == 46, "unexpected offset of new_order_message::fields_type::display");
static_assert(offsetof(new_order_message::fields_type, attribution) == 47, "unexpected offset of new_order_message::fields_type::attribution");
static_assert(offsetof(new_order_message::fields_type, cross_type) == 51, "unexpected offset of new_order_message::fields_type::cross_type");
static_assert(offsetof(new_order_message::fields_type, peg) == 52, "unexpected offset of new_order_message::fields_type::peg");
static_assert(offsetof(new_order_message::fields_type, min_qty) == 53, "unexpected offset of new_order_message::fields_type::min_qty");
static_assert(offsetof(new_order_message::fields_type, peg_offset) == 57, "unexpected offset of new_order_message::fields_type::peg_offset");
static_assert(offsetof(new_order_message::fields_type, post_only) == 61, "unexpected offset of new_order_message::fields_type::post_only");
static_assert(offsetof(new_order_message::fields_type, trade_now) == 62, "unexpected offset of new_order_message::fields_type::trade_now");
static_assert(offsetof(new_order_message::fields_type, short_sale) == 63, "unexpected offset of new_order_message::fields_type::short_sale");
static_assert(offsetof(new_order_message::fields_type, max_floor) == 64, "unexpected offset of new_order_message::fields_type::max_floor");
static_assert(offsetof(new_order_message::fields_type, random_reserve) == 68, "unexpected offset of new_order_message::fields_type::random_reserve");
static_assert(offsetof(new_order_message::fields_type, discretion_price) == 72, "unexpected offset of new_order_message::fields_type::discretion_price");
static_assert(offsetof(new_order_message::fields_type, discretion_type) == 80, "unexpected offset of new_order_message::fields_type::discretion_type");
static_assert(offsetof(new_order_message::fields_type, self_match_group) == 81, "unexpected offset of new_order_message::fields_type::self_match_group");
static_assert(offsetof(new_order_message::fields_type, self_match_level) == 89, "unexpected offset of new_order_message::fields_type::self_match_level");
static_assert(offsetof(new_order_message::fields_type, self_match_strategy) == 90, "unexpected offset of new_order_message::fields_type::self_match_strategy");
static_assert(offsetof(new_order_message::fields_type, aiq_strategy) == 91, "unexpected offset of new_order_message::fields_type::aiq_strategy");
static_assert(offsetof(new_order_message::fields_type, expire_time) == 92, "unexpected offset of new_order_message::fields_type::expire_time");
static_assert(offsetof(new_order_message::fields_type, imbalance_only) == 96, "unexpected offset of new_order_message::fields_type::imbalance_only");
static_assert(sizeof(new_order_message::fields_type) == 97, "unexpected sizeof new_order_message::fields_type");
static_assert(sizeof(new_order_message) == sizeof(ipc_matchingengine::message_header) + 97, "unexpected sizeof new_order_message");

#pragma pack(pop)
}
