#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct reduce_refused_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;

    // parse method
    static reduce_refused_message* parse(std::byte* buffer) {
        return reinterpret_cast<reduce_refused_message*>(buffer);
    }

    // parse method const
    static const reduce_refused_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const reduce_refused_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(reduce_refused_message, timestamp) == 0, "unexpected offset of reduce_refused_message::timestamp");
static_assert(offsetof(reduce_refused_message, order_id) == 8, "unexpected offset of reduce_refused_message::order_id");
static_assert(offsetof(reduce_refused_message, symbol) == 16, "unexpected offset of reduce_refused_message::symbol");
static_assert(sizeof(reduce_refused_message) == 24, "unexpected sizeof reduce_refused_message");

#pragma pack(pop)
}
