#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/SecurityId.hpp"

namespace cme::matchingengine::v0 {

namespace ipc_matchingengine = ::cme::matchingengine::v0;

#pragma pack(push, 1)

struct cancel_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::security_id security_id;

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
static_assert(offsetof(cancel_order_message, security_id) == 16, "unexpected offset of cancel_order_message::security_id");
static_assert(sizeof(cancel_order_message) == 20, "unexpected sizeof cancel_order_message");

#pragma pack(pop)
}
