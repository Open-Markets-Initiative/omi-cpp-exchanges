#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_canceled_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;

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
static_assert(sizeof(order_canceled_message) == 16, "unexpected sizeof order_canceled_message");

#pragma pack(pop)
}
