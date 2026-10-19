#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct cancel_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;

    // parse method
    static cancel_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<cancel_order_message*>(buffer);
    }

    // parse method const
    static const cancel_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const cancel_order_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(cancel_order_message, timestamp) == 0, "unexpected offset of cancel_order_message::timestamp");
static_assert(offsetof(cancel_order_message, order_id) == 8, "unexpected offset of cancel_order_message::order_id");
static_assert(sizeof(cancel_order_message) == 16, "unexpected sizeof cancel_order_message");

#pragma pack(pop)
}
