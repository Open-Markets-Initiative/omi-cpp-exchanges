#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/SequenceNumber.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// End Of Snapshot Message
struct end_of_snapshot_message {

    struct fields_type {
        glimpse_totalview::sequence_number sequence_number;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::end_of_snapshot_message;

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
static_assert(sizeof(end_of_snapshot_message::fields_type) == 20, "unexpected sizeof end_of_snapshot_message::fields_type");
static_assert(sizeof(end_of_snapshot_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 20, "unexpected sizeof end_of_snapshot_message");

#pragma pack(pop)
}
