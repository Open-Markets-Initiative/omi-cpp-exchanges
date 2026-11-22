#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/Symbol.hpp"
#include "../types/PhaseChange.hpp"
#include "../types/Reason.hpp"
#include "../types/Release.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

// Phase Request Message
struct phase_request_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::sequence sequence;
        ipc_matchingengine::symbol symbol;
        ipc_matchingengine::phase_change phase_change;
        ipc_matchingengine::reason reason;
        ipc_matchingengine::release release;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

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
static_assert(offsetof(phase_request_message::fields_type, timestamp) == 0, "unexpected offset of phase_request_message::fields_type::timestamp");
static_assert(offsetof(phase_request_message::fields_type, sequence) == 8, "unexpected offset of phase_request_message::fields_type::sequence");
static_assert(offsetof(phase_request_message::fields_type, symbol) == 16, "unexpected offset of phase_request_message::fields_type::symbol");
static_assert(offsetof(phase_request_message::fields_type, phase_change) == 24, "unexpected offset of phase_request_message::fields_type::phase_change");
static_assert(offsetof(phase_request_message::fields_type, reason) == 25, "unexpected offset of phase_request_message::fields_type::reason");
static_assert(offsetof(phase_request_message::fields_type, release) == 29, "unexpected offset of phase_request_message::fields_type::release");
static_assert(sizeof(phase_request_message::fields_type) == 37, "unexpected sizeof phase_request_message::fields_type");
static_assert(sizeof(phase_request_message) == sizeof(ipc_matchingengine::message_header) + 37, "unexpected sizeof phase_request_message");

#pragma pack(pop)
}
