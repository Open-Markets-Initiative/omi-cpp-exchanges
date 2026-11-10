#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"
#include "../types/PhaseChange.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

// Phase Request Message
struct phase_request_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::sequence sequence;
        ipc_matchingengine::phase_change phase_change;
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
static_assert(offsetof(phase_request_message::fields_type, phase_change) == 16, "unexpected offset of phase_request_message::fields_type::phase_change");
static_assert(sizeof(phase_request_message::fields_type) == 17, "unexpected sizeof phase_request_message::fields_type");
static_assert(sizeof(phase_request_message) == sizeof(ipc_matchingengine::message_header) + 17, "unexpected sizeof phase_request_message");

#pragma pack(pop)
}
