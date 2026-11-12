#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/Group.hpp"
#include "../types/SystemEvent.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::tcppts::itch::v2_2 {

namespace itch_tcppts = ::odx::odxequities::tcppts::itch::v2_2;

#pragma pack(push, 1)

// System Event Message
struct system_event_message {

    struct fields_type {
        itch_tcppts::nanoseconds nanoseconds;
        itch_tcppts::group group;
        itch_tcppts::system_event system_event;
    };

    itch_tcppts::server_packet_header header = {std::uint16_t(sizeof(itch_tcppts::server_packet_header) + sizeof(fields_type) + sizeof(itch_tcppts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_tcppts::sequenced_message_type sequenced_message_type = itch_tcppts::sequenced_message_type::enum_type::system_event_message;

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
static_assert(offsetof(system_event_message::fields_type, nanoseconds) == 0, "unexpected offset of system_event_message::fields_type::nanoseconds");
static_assert(offsetof(system_event_message::fields_type, group) == 4, "unexpected offset of system_event_message::fields_type::group");
static_assert(offsetof(system_event_message::fields_type, system_event) == 8, "unexpected offset of system_event_message::fields_type::system_event");
static_assert(sizeof(system_event_message::fields_type) == 9, "unexpected sizeof system_event_message::fields_type");
static_assert(sizeof(system_event_message) == sizeof(itch_tcppts::server_packet_header) + sizeof(itch_tcppts::sequenced_message_type) + 9, "unexpected sizeof system_event_message");

#pragma pack(pop)
}
