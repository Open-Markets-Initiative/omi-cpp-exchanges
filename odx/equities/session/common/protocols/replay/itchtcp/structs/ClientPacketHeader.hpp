#pragma once

#include <cstddef>
#include "../types/PacketLength.hpp"
#include "../types/ClientPacketType.hpp"

namespace odx::odxequities::tcppts::itch::v2_2 {

namespace itch_tcppts = ::odx::odxequities::tcppts::itch::v2_2;

#pragma pack(push, 1)

struct client_packet_header {

    itch_tcppts::packet_length packet_length;
    itch_tcppts::client_packet_type client_packet_type;

    // parse method
    static client_packet_header* parse(std::byte* buffer) {
        return reinterpret_cast<client_packet_header*>(buffer);
    }

    // parse method const
    static const client_packet_header* parse(const std::byte* buffer) {
        return reinterpret_cast<const client_packet_header*>(buffer);
    }
};

// layout verification
static_assert(offsetof(client_packet_header, packet_length) == 0, "unexpected offset of client_packet_header::packet_length");
static_assert(offsetof(client_packet_header, client_packet_type) == 2, "unexpected offset of client_packet_header::client_packet_type");
static_assert(sizeof(client_packet_header) == 3, "unexpected sizeof client_packet_header");

#pragma pack(pop)
}
