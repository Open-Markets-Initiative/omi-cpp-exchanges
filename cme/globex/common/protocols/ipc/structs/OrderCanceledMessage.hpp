#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/SecurityId.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"

namespace cme::matchingengine::v0 {

namespace ipc_matchingengine = ::cme::matchingengine::v0;

#pragma pack(push, 1)

struct order_canceled_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::security_id security_id;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;

    // parse method
    static order_canceled_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_canceled_message*>(buffer);
    }

    // parse method const
    static const order_canceled_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_canceled_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_canceled_message, timestamp) == 0, "unexpected offset of order_canceled_message::timestamp");
static_assert(offsetof(order_canceled_message, order_id) == 8, "unexpected offset of order_canceled_message::order_id");
static_assert(offsetof(order_canceled_message, security_id) == 16, "unexpected offset of order_canceled_message::security_id");
static_assert(offsetof(order_canceled_message, side) == 20, "unexpected offset of order_canceled_message::side");
static_assert(offsetof(order_canceled_message, price) == 21, "unexpected offset of order_canceled_message::price");
static_assert(offsetof(order_canceled_message, quantity) == 29, "unexpected offset of order_canceled_message::quantity");
static_assert(sizeof(order_canceled_message) == 33, "unexpected sizeof order_canceled_message");

#pragma pack(pop)
}
