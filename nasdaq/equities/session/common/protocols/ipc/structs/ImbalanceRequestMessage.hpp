#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct imbalance_request_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;

    // parse method
    static imbalance_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<imbalance_request_message*>(buffer);
    }

    // parse method const
    static const imbalance_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const imbalance_request_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(imbalance_request_message, timestamp) == 0, "unexpected offset of imbalance_request_message::timestamp");
static_assert(offsetof(imbalance_request_message, sequence) == 8, "unexpected offset of imbalance_request_message::sequence");
static_assert(sizeof(imbalance_request_message) == 16, "unexpected sizeof imbalance_request_message");

#pragma pack(pop)
}
