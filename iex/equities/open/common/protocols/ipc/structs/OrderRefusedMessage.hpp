#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

struct order_refused_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;

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
static_assert(sizeof(order_refused_message) == 16, "unexpected sizeof order_refused_message");

#pragma pack(pop)
}
