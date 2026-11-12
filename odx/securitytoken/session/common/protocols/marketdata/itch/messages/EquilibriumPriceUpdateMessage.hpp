#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderbookId.hpp"
#include "../types/EquilibriumPrice.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxsecuritytoken::pts::itch::v1_2 {

namespace itch_pts = ::odx::odxsecuritytoken::pts::itch::v1_2;

#pragma pack(push, 1)

// Equilibrium Price Update Message
struct equilibrium_price_update_message {

    struct fields_type {
        itch_pts::nanoseconds nanoseconds;
        itch_pts::orderbook_id orderbook_id;
        itch_pts::equilibrium_price equilibrium_price;
    };

    itch_pts::server_packet_header header = {std::uint16_t(sizeof(itch_pts::server_packet_header) + sizeof(fields_type) + sizeof(itch_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_pts::sequenced_message_type sequenced_message_type = itch_pts::sequenced_message_type::enum_type::equilibrium_price_update_message;

    fields_type fields;

    // parse method
    static equilibrium_price_update_message* parse(std::byte* buffer) {
        return reinterpret_cast<equilibrium_price_update_message*>(buffer);
    }

    // parse method const
    static const equilibrium_price_update_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const equilibrium_price_update_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(equilibrium_price_update_message::fields_type, nanoseconds) == 0, "unexpected offset of equilibrium_price_update_message::fields_type::nanoseconds");
static_assert(offsetof(equilibrium_price_update_message::fields_type, orderbook_id) == 4, "unexpected offset of equilibrium_price_update_message::fields_type::orderbook_id");
static_assert(offsetof(equilibrium_price_update_message::fields_type, equilibrium_price) == 8, "unexpected offset of equilibrium_price_update_message::fields_type::equilibrium_price");
static_assert(sizeof(equilibrium_price_update_message::fields_type) == 12, "unexpected sizeof equilibrium_price_update_message::fields_type");
static_assert(sizeof(equilibrium_price_update_message) == sizeof(itch_pts::server_packet_header) + sizeof(itch_pts::sequenced_message_type) + 12, "unexpected sizeof equilibrium_price_update_message");

#pragma pack(pop)
}
