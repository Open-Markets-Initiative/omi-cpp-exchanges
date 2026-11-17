#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/Display.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_eliminated_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::time_in_force time_in_force;
    ipc_matchingengine::display display;

    // parse method
    static order_eliminated_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_eliminated_message*>(buffer);
    }

    // parse method const
    static const order_eliminated_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_eliminated_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_eliminated_message, timestamp) == 0, "unexpected offset of order_eliminated_message::timestamp");
static_assert(offsetof(order_eliminated_message, order_id) == 8, "unexpected offset of order_eliminated_message::order_id");
static_assert(offsetof(order_eliminated_message, side) == 16, "unexpected offset of order_eliminated_message::side");
static_assert(offsetof(order_eliminated_message, price) == 17, "unexpected offset of order_eliminated_message::price");
static_assert(offsetof(order_eliminated_message, quantity) == 25, "unexpected offset of order_eliminated_message::quantity");
static_assert(offsetof(order_eliminated_message, time_in_force) == 29, "unexpected offset of order_eliminated_message::time_in_force");
static_assert(offsetof(order_eliminated_message, display) == 30, "unexpected offset of order_eliminated_message::display");
static_assert(sizeof(order_eliminated_message) == 31, "unexpected sizeof order_eliminated_message");

#pragma pack(pop)
}
