#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderbookId.hpp"
#include "../types/Group.hpp"
#include "../types/ShortSellingState.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::tcppts::itch::v2_2 {

namespace itch_tcppts = ::odx::odxequities::tcppts::itch::v2_2;

#pragma pack(push, 1)

// Short Selling Price Restriction State Message
struct short_selling_price_restriction_state_message {

    struct fields_type {
        itch_tcppts::nanoseconds nanoseconds;
        itch_tcppts::orderbook_id orderbook_id;
        itch_tcppts::group group;
        itch_tcppts::short_selling_state short_selling_state;
    };

    itch_tcppts::server_packet_header header = {std::uint16_t(sizeof(itch_tcppts::server_packet_header) + sizeof(fields_type) + sizeof(itch_tcppts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_tcppts::sequenced_message_type sequenced_message_type = itch_tcppts::sequenced_message_type::enum_type::short_selling_price_restriction_state_message;

    fields_type fields;

    // parse method
    static short_selling_price_restriction_state_message* parse(std::byte* buffer) {
        return reinterpret_cast<short_selling_price_restriction_state_message*>(buffer);
    }

    // parse method const
    static const short_selling_price_restriction_state_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const short_selling_price_restriction_state_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(short_selling_price_restriction_state_message::fields_type, nanoseconds) == 0, "unexpected offset of short_selling_price_restriction_state_message::fields_type::nanoseconds");
static_assert(offsetof(short_selling_price_restriction_state_message::fields_type, orderbook_id) == 4, "unexpected offset of short_selling_price_restriction_state_message::fields_type::orderbook_id");
static_assert(offsetof(short_selling_price_restriction_state_message::fields_type, group) == 8, "unexpected offset of short_selling_price_restriction_state_message::fields_type::group");
static_assert(offsetof(short_selling_price_restriction_state_message::fields_type, short_selling_state) == 12, "unexpected offset of short_selling_price_restriction_state_message::fields_type::short_selling_state");
static_assert(sizeof(short_selling_price_restriction_state_message::fields_type) == 13, "unexpected sizeof short_selling_price_restriction_state_message::fields_type");
static_assert(sizeof(short_selling_price_restriction_state_message) == sizeof(itch_tcppts::server_packet_header) + sizeof(itch_tcppts::sequenced_message_type) + 13, "unexpected sizeof short_selling_price_restriction_state_message");

#pragma pack(pop)
}
