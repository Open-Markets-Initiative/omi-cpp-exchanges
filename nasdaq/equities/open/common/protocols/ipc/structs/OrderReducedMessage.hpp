#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Quantity.hpp"
#include "../types/Display.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct order_reduced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::display display;

    // parse method
    static order_reduced_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_reduced_message*>(buffer);
    }

    // parse method const
    static const order_reduced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_reduced_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_reduced_message, timestamp) == 0, "unexpected offset of order_reduced_message::timestamp");
static_assert(offsetof(order_reduced_message, order_id) == 8, "unexpected offset of order_reduced_message::order_id");
static_assert(offsetof(order_reduced_message, symbol) == 16, "unexpected offset of order_reduced_message::symbol");
static_assert(offsetof(order_reduced_message, quantity) == 24, "unexpected offset of order_reduced_message::quantity");
static_assert(offsetof(order_reduced_message, display) == 28, "unexpected offset of order_reduced_message::display");
static_assert(sizeof(order_reduced_message) == 29, "unexpected sizeof order_reduced_message");

#pragma pack(pop)
}
