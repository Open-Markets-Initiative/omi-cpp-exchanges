#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Seconds.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::pts::glimpse::v2_0 {

namespace glimpse_pts = ::odx::odxequities::pts::glimpse::v2_0;

#pragma pack(push, 1)

// Seconds Message
struct seconds_message {

    struct fields_type {
        glimpse_pts::seconds seconds;
    };

    glimpse_pts::server_packet_header header = {std::uint16_t(sizeof(glimpse_pts::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_pts::sequenced_message_type sequenced_message_type = glimpse_pts::sequenced_message_type::enum_type::timestamp_seconds_message;

    fields_type fields;

    // parse method
    static seconds_message* parse(std::byte* buffer) {
        return reinterpret_cast<seconds_message*>(buffer);
    }

    // parse method const
    static const seconds_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const seconds_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(seconds_message::fields_type, seconds) == 0, "unexpected offset of seconds_message::fields_type::seconds");
static_assert(sizeof(seconds_message::fields_type) == 4, "unexpected sizeof seconds_message::fields_type");
static_assert(sizeof(seconds_message) == sizeof(glimpse_pts::server_packet_header) + sizeof(glimpse_pts::sequenced_message_type) + 4, "unexpected sizeof seconds_message");

#pragma pack(pop)
}
