#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Price.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_accepted_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::price price;

    // parse method
    static order_accepted_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_accepted_message*>(buffer);
    }

    // parse method const
    static const order_accepted_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_accepted_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_accepted_message, timestamp) == 0, "unexpected offset of order_accepted_message::timestamp");
static_assert(offsetof(order_accepted_message, order_id) == 8, "unexpected offset of order_accepted_message::order_id");
static_assert(offsetof(order_accepted_message, symbol) == 16, "unexpected offset of order_accepted_message::symbol");
static_assert(offsetof(order_accepted_message, price) == 24, "unexpected offset of order_accepted_message::price");
static_assert(sizeof(order_accepted_message) == 32, "unexpected sizeof order_accepted_message");

#pragma pack(pop)
}
