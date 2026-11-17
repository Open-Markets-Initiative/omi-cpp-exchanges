#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct cancel_pending_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;

    // parse method
    static cancel_pending_message* parse(std::byte* buffer) {
        return reinterpret_cast<cancel_pending_message*>(buffer);
    }

    // parse method const
    static const cancel_pending_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const cancel_pending_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(cancel_pending_message, timestamp) == 0, "unexpected offset of cancel_pending_message::timestamp");
static_assert(offsetof(cancel_pending_message, order_id) == 8, "unexpected offset of cancel_pending_message::order_id");
static_assert(offsetof(cancel_pending_message, symbol) == 16, "unexpected offset of cancel_pending_message::symbol");
static_assert(sizeof(cancel_pending_message) == 24, "unexpected sizeof cancel_pending_message");

#pragma pack(pop)
}
