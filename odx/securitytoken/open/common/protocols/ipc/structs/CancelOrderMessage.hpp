#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/OrderId.hpp"

namespace odx::odxsecuritytoken::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxsecuritytoken::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct cancel_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
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
static_assert(offsetof(cancel_order_message, sequence) == 8, "unexpected offset of cancel_order_message::sequence");
static_assert(offsetof(cancel_order_message, order_id) == 16, "unexpected offset of cancel_order_message::order_id");
static_assert(sizeof(cancel_order_message) == 24, "unexpected sizeof cancel_order_message");

#pragma pack(pop)
}
