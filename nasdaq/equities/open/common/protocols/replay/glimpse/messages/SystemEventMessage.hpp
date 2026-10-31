#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/StockLocate.hpp"
#include "../types/TrackingNumber.hpp"
#include "../types/Timestamp.hpp"
#include "../types/EventCode.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// System Event Message
struct system_event_message {

    struct fields_type {
        glimpse_totalview::stock_locate stock_locate;
        glimpse_totalview::tracking_number tracking_number;
        glimpse_totalview::timestamp timestamp;
        glimpse_totalview::event_code event_code;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::system_event_message;

    fields_type fields;

    // parse method
    static system_event_message* parse(std::byte* buffer) {
        return reinterpret_cast<system_event_message*>(buffer);
    }

    // parse method const
    static const system_event_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const system_event_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(system_event_message::fields_type, stock_locate) == 0, "unexpected offset of system_event_message::fields_type::stock_locate");
static_assert(offsetof(system_event_message::fields_type, tracking_number) == 2, "unexpected offset of system_event_message::fields_type::tracking_number");
static_assert(offsetof(system_event_message::fields_type, timestamp) == 4, "unexpected offset of system_event_message::fields_type::timestamp");
static_assert(offsetof(system_event_message::fields_type, event_code) == 10, "unexpected offset of system_event_message::fields_type::event_code");
static_assert(sizeof(system_event_message::fields_type) == 11, "unexpected sizeof system_event_message::fields_type");
static_assert(sizeof(system_event_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 11, "unexpected sizeof system_event_message");

#pragma pack(pop)
}
