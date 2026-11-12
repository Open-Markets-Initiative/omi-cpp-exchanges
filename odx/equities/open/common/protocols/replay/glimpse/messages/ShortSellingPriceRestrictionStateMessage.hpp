#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderbookId.hpp"
#include "../types/Group.hpp"
#include "../types/ShortSellingState.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::pts::glimpse::v2_0 {

namespace glimpse_pts = ::odx::odxequities::pts::glimpse::v2_0;

#pragma pack(push, 1)

// Short Selling Price Restriction State Message
struct short_selling_price_restriction_state_message {

    struct fields_type {
        glimpse_pts::nanoseconds nanoseconds;
        glimpse_pts::orderbook_id orderbook_id;
        glimpse_pts::group group;
        glimpse_pts::short_selling_state short_selling_state;
    };

    glimpse_pts::server_packet_header header = {std::uint16_t(sizeof(glimpse_pts::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_pts::sequenced_message_type sequenced_message_type = glimpse_pts::sequenced_message_type::enum_type::short_selling_price_restriction_state_message;

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
static_assert(sizeof(short_selling_price_restriction_state_message) == sizeof(glimpse_pts::server_packet_header) + sizeof(glimpse_pts::sequenced_message_type) + 13, "unexpected sizeof short_selling_price_restriction_state_message");

#pragma pack(pop)
}
