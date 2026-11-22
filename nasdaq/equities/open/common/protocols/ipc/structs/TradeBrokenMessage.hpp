#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/AggressorOrderId.hpp"
#include "../types/RestingOrderId.hpp"
#include "../types/ExecutionId.hpp"
#include "../types/Symbol.hpp"
#include "../types/BreakReason.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct trade_broken_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::aggressor_order_id aggressor_order_id;
    ipc_matchingengine::resting_order_id resting_order_id;
    ipc_matchingengine::execution_id execution_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::break_reason break_reason;

    // parse method
    static trade_broken_message* parse(std::byte* buffer) {
        return reinterpret_cast<trade_broken_message*>(buffer);
    }

    // parse method const
    static const trade_broken_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const trade_broken_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(trade_broken_message, timestamp) == 0, "unexpected offset of trade_broken_message::timestamp");
static_assert(offsetof(trade_broken_message, aggressor_order_id) == 8, "unexpected offset of trade_broken_message::aggressor_order_id");
static_assert(offsetof(trade_broken_message, resting_order_id) == 16, "unexpected offset of trade_broken_message::resting_order_id");
static_assert(offsetof(trade_broken_message, execution_id) == 24, "unexpected offset of trade_broken_message::execution_id");
static_assert(offsetof(trade_broken_message, symbol) == 32, "unexpected offset of trade_broken_message::symbol");
static_assert(offsetof(trade_broken_message, break_reason) == 40, "unexpected offset of trade_broken_message::break_reason");
static_assert(sizeof(trade_broken_message) == 41, "unexpected sizeof trade_broken_message");

#pragma pack(pop)
}
