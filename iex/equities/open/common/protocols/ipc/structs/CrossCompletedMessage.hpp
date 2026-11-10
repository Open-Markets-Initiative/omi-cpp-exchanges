#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Cross.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/ExecutionId.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

struct cross_completed_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::cross cross;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::execution_id execution_id;

    // parse method
    static cross_completed_message* parse(std::byte* buffer) {
        return reinterpret_cast<cross_completed_message*>(buffer);
    }

    // parse method const
    static const cross_completed_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const cross_completed_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(cross_completed_message, timestamp) == 0, "unexpected offset of cross_completed_message::timestamp");
static_assert(offsetof(cross_completed_message, symbol) == 8, "unexpected offset of cross_completed_message::symbol");
static_assert(offsetof(cross_completed_message, cross) == 16, "unexpected offset of cross_completed_message::cross");
static_assert(offsetof(cross_completed_message, price) == 17, "unexpected offset of cross_completed_message::price");
static_assert(offsetof(cross_completed_message, quantity) == 25, "unexpected offset of cross_completed_message::quantity");
static_assert(offsetof(cross_completed_message, execution_id) == 29, "unexpected offset of cross_completed_message::execution_id");
static_assert(sizeof(cross_completed_message) == 37, "unexpected sizeof cross_completed_message");

#pragma pack(pop)
}
