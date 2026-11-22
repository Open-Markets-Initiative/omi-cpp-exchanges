#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/PhaseChange.hpp"

namespace nse::nsefo::basicmatchingengine::ipc::v2026 {

namespace ipc_basicmatchingengine = ::nse::nsefo::basicmatchingengine::ipc::v2026;

#pragma pack(push, 1)

struct phase_request_message {

    ipc_basicmatchingengine::timestamp timestamp;
    ipc_basicmatchingengine::phase_change phase_change;

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
static_assert(offsetof(phase_request_message, phase_change) == 8, "unexpected offset of phase_request_message::phase_change");
static_assert(sizeof(phase_request_message) == 9, "unexpected sizeof phase_request_message");

#pragma pack(pop)
}
