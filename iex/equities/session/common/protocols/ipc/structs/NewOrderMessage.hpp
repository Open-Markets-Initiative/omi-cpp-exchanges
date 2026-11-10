#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

struct new_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;

    // parse method
    static new_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<new_order_message*>(buffer);
    }

    // parse method const
    static const new_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const new_order_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(new_order_message, timestamp) == 0, "unexpected offset of new_order_message::timestamp");
static_assert(offsetof(new_order_message, sequence) == 8, "unexpected offset of new_order_message::sequence");
static_assert(offsetof(new_order_message, order_id) == 16, "unexpected offset of new_order_message::order_id");
static_assert(offsetof(new_order_message, symbol) == 24, "unexpected offset of new_order_message::symbol");
static_assert(offsetof(new_order_message, side) == 32, "unexpected offset of new_order_message::side");
static_assert(offsetof(new_order_message, price) == 33, "unexpected offset of new_order_message::price");
static_assert(offsetof(new_order_message, quantity) == 41, "unexpected offset of new_order_message::quantity");
static_assert(sizeof(new_order_message) == 45, "unexpected sizeof new_order_message");

#pragma pack(pop)
}
