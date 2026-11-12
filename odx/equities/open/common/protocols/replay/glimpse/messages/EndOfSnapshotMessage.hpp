#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/SequenceNumber.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::pts::glimpse::v2_0 {

namespace glimpse_pts = ::odx::odxequities::pts::glimpse::v2_0;

#pragma pack(push, 1)

// End Of Snapshot Message
struct end_of_snapshot_message {

    struct fields_type {
        glimpse_pts::sequence_number sequence_number;
    };

    glimpse_pts::server_packet_header header = {std::uint16_t(sizeof(glimpse_pts::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_pts::sequenced_message_type sequenced_message_type = glimpse_pts::sequenced_message_type::enum_type::end_of_snapshot_message;

    fields_type fields;

    // parse method
    static end_of_snapshot_message* parse(std::byte* buffer) {
        return reinterpret_cast<end_of_snapshot_message*>(buffer);
    }

    // parse method const
    static const end_of_snapshot_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const end_of_snapshot_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(end_of_snapshot_message::fields_type, sequence_number) == 0, "unexpected offset of end_of_snapshot_message::fields_type::sequence_number");
static_assert(sizeof(end_of_snapshot_message::fields_type) == 8, "unexpected sizeof end_of_snapshot_message::fields_type");
static_assert(sizeof(end_of_snapshot_message) == sizeof(glimpse_pts::server_packet_header) + sizeof(glimpse_pts::sequenced_message_type) + 8, "unexpected sizeof end_of_snapshot_message");

#pragma pack(pop)
}
