#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/ExecutionId.hpp"
#include "../types/BreakReason.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct break_trade_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::execution_id execution_id;
    ipc_matchingengine::break_reason break_reason;

    // parse method
    static break_trade_message* parse(std::byte* buffer) {
        return reinterpret_cast<break_trade_message*>(buffer);
    }

    // parse method const
    static const break_trade_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const break_trade_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(break_trade_message, timestamp) == 0, "unexpected offset of break_trade_message::timestamp");
static_assert(offsetof(break_trade_message, sequence) == 8, "unexpected offset of break_trade_message::sequence");
static_assert(offsetof(break_trade_message, execution_id) == 16, "unexpected offset of break_trade_message::execution_id");
static_assert(offsetof(break_trade_message, break_reason) == 24, "unexpected offset of break_trade_message::break_reason");
static_assert(sizeof(break_trade_message) == 25, "unexpected sizeof break_trade_message");

#pragma pack(pop)
}
