#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/RefreshedFrom.hpp"
#include "../types/Repriced.hpp"
#include "../types/Display.hpp"
#include "../types/Attribution.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_rested_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::refreshed_from refreshed_from;
    ipc_matchingengine::repriced repriced;
    ipc_matchingengine::display display;
    ipc_matchingengine::attribution attribution;

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
static_assert(offsetof(order_rested_message, symbol) == 16, "unexpected offset of order_rested_message::symbol");
static_assert(offsetof(order_rested_message, side) == 24, "unexpected offset of order_rested_message::side");
static_assert(offsetof(order_rested_message, price) == 25, "unexpected offset of order_rested_message::price");
static_assert(offsetof(order_rested_message, quantity) == 33, "unexpected offset of order_rested_message::quantity");
static_assert(offsetof(order_rested_message, refreshed_from) == 37, "unexpected offset of order_rested_message::refreshed_from");
static_assert(offsetof(order_rested_message, repriced) == 45, "unexpected offset of order_rested_message::repriced");
static_assert(offsetof(order_rested_message, display) == 46, "unexpected offset of order_rested_message::display");
static_assert(offsetof(order_rested_message, attribution) == 47, "unexpected offset of order_rested_message::attribution");
static_assert(sizeof(order_rested_message) == 51, "unexpected sizeof order_rested_message");

#pragma pack(pop)
}
