#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/Symbol.hpp"
#include "../types/PhaseChange.hpp"
#include "../types/Reason.hpp"
#include "../types/Release.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct phase_request_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::phase_change phase_change;
    ipc_matchingengine::reason reason;
    ipc_matchingengine::release release;

    // parse method
    static phase_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<phase_request_message*>(buffer);
    }

    // parse method const
    static const phase_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const phase_request_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(phase_request_message, timestamp) == 0, "unexpected offset of phase_request_message::timestamp");
static_assert(offsetof(phase_request_message, sequence) == 8, "unexpected offset of phase_request_message::sequence");
static_assert(offsetof(phase_request_message, symbol) == 16, "unexpected offset of phase_request_message::symbol");
static_assert(offsetof(phase_request_message, phase_change) == 24, "unexpected offset of phase_request_message::phase_change");
static_assert(offsetof(phase_request_message, reason) == 25, "unexpected offset of phase_request_message::reason");
static_assert(offsetof(phase_request_message, release) == 29, "unexpected offset of phase_request_message::release");
static_assert(sizeof(phase_request_message) == 37, "unexpected sizeof phase_request_message");

#pragma pack(pop)
}
