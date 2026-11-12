#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Orderbook.hpp"
#include "../types/Cross.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/ExecutionId.hpp"

namespace odx::odxequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct cross_completed_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::orderbook orderbook;
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
static_assert(offsetof(cross_completed_message, orderbook) == 8, "unexpected offset of cross_completed_message::orderbook");
static_assert(offsetof(cross_completed_message, cross) == 12, "unexpected offset of cross_completed_message::cross");
static_assert(offsetof(cross_completed_message, price) == 13, "unexpected offset of cross_completed_message::price");
static_assert(offsetof(cross_completed_message, quantity) == 17, "unexpected offset of cross_completed_message::quantity");
static_assert(offsetof(cross_completed_message, execution_id) == 21, "unexpected offset of cross_completed_message::execution_id");
static_assert(sizeof(cross_completed_message) == 29, "unexpected sizeof cross_completed_message");

#pragma pack(pop)
}
