#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/RefusalReason.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_refused_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::refusal_reason refusal_reason;

    // parse method
    static order_refused_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_refused_message*>(buffer);
    }

    // parse method const
    static const order_refused_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_refused_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_refused_message, timestamp) == 0, "unexpected offset of order_refused_message::timestamp");
static_assert(offsetof(order_refused_message, order_id) == 8, "unexpected offset of order_refused_message::order_id");
static_assert(offsetof(order_refused_message, refusal_reason) == 16, "unexpected offset of order_refused_message::refusal_reason");
static_assert(sizeof(order_refused_message) == 17, "unexpected sizeof order_refused_message");

#pragma pack(pop)
}
