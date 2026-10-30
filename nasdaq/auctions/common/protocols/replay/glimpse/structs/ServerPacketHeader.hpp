#pragma once

#include <cstddef>
#include "../types/PacketLength.hpp"
#include "../types/ServerPacketType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

struct server_packet_header {

    glimpse_totalview::packet_length packet_length;
    glimpse_totalview::server_packet_type server_packet_type;

    // parse method
    static server_packet_header* parse(std::byte* buffer) {
        return reinterpret_cast<server_packet_header*>(buffer);
    }

    // parse method const
    static const server_packet_header* parse(const std::byte* buffer) {
        return reinterpret_cast<const server_packet_header*>(buffer);
    }
};

// layout verification
static_assert(offsetof(server_packet_header, packet_length) == 0, "unexpected offset of server_packet_header::packet_length");
static_assert(offsetof(server_packet_header, server_packet_type) == 2, "unexpected offset of server_packet_header::server_packet_type");
static_assert(sizeof(server_packet_header) == 3, "unexpected sizeof server_packet_header");

#pragma pack(pop)
}
