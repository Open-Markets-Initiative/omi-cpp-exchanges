#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/PriceTickSizeTableId.hpp"
#include "../types/PriceTickSize.hpp"
#include "../types/PriceStart.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::pts::glimpse::v2_0 {

namespace glimpse_pts = ::odx::odxequities::pts::glimpse::v2_0;

#pragma pack(push, 1)

// Price Tick Size Message
struct price_tick_size_message {

    struct fields_type {
        glimpse_pts::nanoseconds nanoseconds;
        glimpse_pts::price_tick_size_table_id price_tick_size_table_id;
        glimpse_pts::price_tick_size price_tick_size;
        glimpse_pts::price_start price_start;
    };

    glimpse_pts::server_packet_header header = {std::uint16_t(sizeof(glimpse_pts::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_pts::sequenced_message_type sequenced_message_type = glimpse_pts::sequenced_message_type::enum_type::price_tick_size_message;

    fields_type fields;

    // parse method
    static price_tick_size_message* parse(std::byte* buffer) {
        return reinterpret_cast<price_tick_size_message*>(buffer);
    }

    // parse method const
    static const price_tick_size_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const price_tick_size_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(price_tick_size_message::fields_type, nanoseconds) == 0, "unexpected offset of price_tick_size_message::fields_type::nanoseconds");
static_assert(offsetof(price_tick_size_message::fields_type, price_tick_size_table_id) == 4, "unexpected offset of price_tick_size_message::fields_type::price_tick_size_table_id");
static_assert(offsetof(price_tick_size_message::fields_type, price_tick_size) == 8, "unexpected offset of price_tick_size_message::fields_type::price_tick_size");
static_assert(offsetof(price_tick_size_message::fields_type, price_start) == 12, "unexpected offset of price_tick_size_message::fields_type::price_start");
static_assert(sizeof(price_tick_size_message::fields_type) == 16, "unexpected sizeof price_tick_size_message::fields_type");
static_assert(sizeof(price_tick_size_message) == sizeof(glimpse_pts::server_packet_header) + sizeof(glimpse_pts::sequenced_message_type) + 16, "unexpected sizeof price_tick_size_message");

#pragma pack(pop)
}
