#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Quantity.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct reduce_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::quantity quantity;

    // parse method
    static reduce_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<reduce_order_message*>(buffer);
    }

    // parse method const
    static const reduce_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const reduce_order_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(reduce_order_message, timestamp) == 0, "unexpected offset of reduce_order_message::timestamp");
static_assert(offsetof(reduce_order_message, sequence) == 8, "unexpected offset of reduce_order_message::sequence");
static_assert(offsetof(reduce_order_message, order_id) == 16, "unexpected offset of reduce_order_message::order_id");
static_assert(offsetof(reduce_order_message, symbol) == 24, "unexpected offset of reduce_order_message::symbol");
static_assert(offsetof(reduce_order_message, quantity) == 32, "unexpected offset of reduce_order_message::quantity");
static_assert(sizeof(reduce_order_message) == 36, "unexpected sizeof reduce_order_message");

#pragma pack(pop)
}
