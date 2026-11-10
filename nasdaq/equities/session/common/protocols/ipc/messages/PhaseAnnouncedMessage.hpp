#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Announcement.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

// Phase Announced Message
struct phase_announced_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::symbol symbol;
        ipc_matchingengine::announcement announcement;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

    // parse method
    static phase_announced_message* parse(std::byte* buffer) {
        return reinterpret_cast<phase_announced_message*>(buffer);
    }

    // parse method const
    static const phase_announced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const phase_announced_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(phase_announced_message::fields_type, timestamp) == 0, "unexpected offset of phase_announced_message::fields_type::timestamp");
static_assert(offsetof(phase_announced_message::fields_type, symbol) == 8, "unexpected offset of phase_announced_message::fields_type::symbol");
static_assert(offsetof(phase_announced_message::fields_type, announcement) == 16, "unexpected offset of phase_announced_message::fields_type::announcement");
static_assert(sizeof(phase_announced_message::fields_type) == 17, "unexpected sizeof phase_announced_message::fields_type");
static_assert(sizeof(phase_announced_message) == sizeof(ipc_matchingengine::message_header) + 17, "unexpected sizeof phase_announced_message");

#pragma pack(pop)
}
