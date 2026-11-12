#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/AggressorOrderId.hpp"
#include "../types/RestingOrderId.hpp"
#include "../types/ExecutionId.hpp"
#include "../types/Orderbook.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/Auction.hpp"

namespace odx::odxsecuritytoken::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxsecuritytoken::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct trade_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::aggressor_order_id aggressor_order_id;
    ipc_matchingengine::resting_order_id resting_order_id;
    ipc_matchingengine::execution_id execution_id;
    ipc_matchingengine::orderbook orderbook;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::auction auction;

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
static_assert(offsetof(trade_message, execution_id) == 24, "unexpected offset of trade_message::execution_id");
static_assert(offsetof(trade_message, orderbook) == 32, "unexpected offset of trade_message::orderbook");
static_assert(offsetof(trade_message, price) == 36, "unexpected offset of trade_message::price");
static_assert(offsetof(trade_message, quantity) == 40, "unexpected offset of trade_message::quantity");
static_assert(offsetof(trade_message, auction) == 44, "unexpected offset of trade_message::auction");
static_assert(sizeof(trade_message) == 45, "unexpected sizeof trade_message");

#pragma pack(pop)
}
