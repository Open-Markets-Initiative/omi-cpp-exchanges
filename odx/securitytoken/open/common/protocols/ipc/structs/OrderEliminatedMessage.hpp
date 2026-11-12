#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/TimeInForce.hpp"

namespace odx::odxsecuritytoken::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxsecuritytoken::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_eliminated_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::time_in_force time_in_force;

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
static_assert(offsetof(order_eliminated_message, quantity) == 21, "unexpected offset of order_eliminated_message::quantity");
static_assert(offsetof(order_eliminated_message, time_in_force) == 25, "unexpected offset of order_eliminated_message::time_in_force");
static_assert(sizeof(order_eliminated_message) == 26, "unexpected sizeof order_eliminated_message");

#pragma pack(pop)
}
