#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/AggressorOrderId.hpp"
#include "../types/RestingOrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

struct trade_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::aggressor_order_id aggressor_order_id;
    ipc_matchingengine::resting_order_id resting_order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;

    // parse method
    static trade_message* parse(std::byte* buffer) {
        return reinterpret_cast<trade_message*>(buffer);
    }

    // parse method const
    static const trade_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const trade_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(trade_message, timestamp) == 0, "unexpected offset of trade_message::timestamp");
static_assert(offsetof(trade_message, aggressor_order_id) == 8, "unexpected offset of trade_message::aggressor_order_id");
static_assert(offsetof(trade_message, resting_order_id) == 16, "unexpected offset of trade_message::resting_order_id");
static_assert(offsetof(trade_message, symbol) == 24, "unexpected offset of trade_message::symbol");
static_assert(offsetof(trade_message, price) == 32, "unexpected offset of trade_message::price");
static_assert(offsetof(trade_message, quantity) == 40, "unexpected offset of trade_message::quantity");
static_assert(sizeof(trade_message) == 44, "unexpected sizeof trade_message");

#pragma pack(pop)
}
