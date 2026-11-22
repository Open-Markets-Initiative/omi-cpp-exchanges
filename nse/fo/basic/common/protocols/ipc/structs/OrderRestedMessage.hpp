#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Token.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"

namespace nse::nsefo::basicmatchingengine::ipc::v2026 {

namespace ipc_basicmatchingengine = ::nse::nsefo::basicmatchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_rested_message {

    ipc_basicmatchingengine::timestamp timestamp;
    ipc_basicmatchingengine::order_id order_id;
    ipc_basicmatchingengine::token token;
    ipc_basicmatchingengine::side side;
    ipc_basicmatchingengine::price price;
    ipc_basicmatchingengine::quantity quantity;

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
static_assert(offsetof(order_rested_message, token) == 16, "unexpected offset of order_rested_message::token");
static_assert(offsetof(order_rested_message, side) == 20, "unexpected offset of order_rested_message::side");
static_assert(offsetof(order_rested_message, price) == 21, "unexpected offset of order_rested_message::price");
static_assert(offsetof(order_rested_message, quantity) == 29, "unexpected offset of order_rested_message::quantity");
static_assert(sizeof(order_rested_message) == 33, "unexpected sizeof order_rested_message");

#pragma pack(pop)
}
