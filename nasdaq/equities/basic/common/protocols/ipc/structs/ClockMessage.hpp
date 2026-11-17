#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct clock_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;

    // parse method
    static clock_message* parse(std::byte* buffer) {
        return reinterpret_cast<clock_message*>(buffer);
    }

    // parse method const
    static const clock_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const clock_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(clock_message, timestamp) == 0, "unexpected offset of clock_message::timestamp");
static_assert(offsetof(clock_message, sequence) == 8, "unexpected offset of clock_message::sequence");
static_assert(sizeof(clock_message) == 16, "unexpected sizeof clock_message");

#pragma pack(pop)
}
