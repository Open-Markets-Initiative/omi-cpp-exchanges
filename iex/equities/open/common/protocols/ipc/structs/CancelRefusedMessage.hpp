#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct cancel_refused_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;

    // parse method
    static cancel_refused_message* parse(std::byte* buffer) {
        return reinterpret_cast<cancel_refused_message*>(buffer);
    }

    // parse method const
    static const cancel_refused_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const cancel_refused_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(cancel_refused_message, timestamp) == 0, "unexpected offset of cancel_refused_message::timestamp");
static_assert(offsetof(cancel_refused_message, order_id) == 8, "unexpected offset of cancel_refused_message::order_id");
static_assert(sizeof(cancel_refused_message) == 16, "unexpected sizeof cancel_refused_message");

#pragma pack(pop)
}
