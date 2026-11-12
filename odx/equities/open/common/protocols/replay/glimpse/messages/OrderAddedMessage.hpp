#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderNumber.hpp"
#include "../types/BuySellIndicator.hpp"
#include "../types/Quantity.hpp"
#include "../types/OrderbookId.hpp"
#include "../types/Group.hpp"
#include "../types/Price.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::pts::glimpse::v2_0 {

namespace glimpse_pts = ::odx::odxequities::pts::glimpse::v2_0;

#pragma pack(push, 1)

// Order Added Message
struct order_added_message {

    struct fields_type {
        glimpse_pts::nanoseconds nanoseconds;
        glimpse_pts::order_number order_number;
        glimpse_pts::buy_sell_indicator buy_sell_indicator;
        glimpse_pts::quantity quantity;
        glimpse_pts::orderbook_id orderbook_id;
        glimpse_pts::group group;
        glimpse_pts::price price;
    };

    glimpse_pts::server_packet_header header = {std::uint16_t(sizeof(glimpse_pts::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_pts::sequenced_message_type sequenced_message_type = glimpse_pts::sequenced_message_type::enum_type::order_added_message;

    fields_type fields;

    // parse method
    static order_added_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_added_message*>(buffer);
    }

    // parse method const
    static const order_added_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_added_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_added_message::fields_type, nanoseconds) == 0, "unexpected offset of order_added_message::fields_type::nanoseconds");
static_assert(offsetof(order_added_message::fields_type, order_number) == 4, "unexpected offset of order_added_message::fields_type::order_number");
static_assert(offsetof(order_added_message::fields_type, buy_sell_indicator) == 12, "unexpected offset of order_added_message::fields_type::buy_sell_indicator");
static_assert(offsetof(order_added_message::fields_type, quantity) == 13, "unexpected offset of order_added_message::fields_type::quantity");
static_assert(offsetof(order_added_message::fields_type, orderbook_id) == 17, "unexpected offset of order_added_message::fields_type::orderbook_id");
static_assert(offsetof(order_added_message::fields_type, group) == 21, "unexpected offset of order_added_message::fields_type::group");
static_assert(offsetof(order_added_message::fields_type, price) == 25, "unexpected offset of order_added_message::fields_type::price");
static_assert(sizeof(order_added_message::fields_type) == 29, "unexpected sizeof order_added_message::fields_type");
static_assert(sizeof(order_added_message) == sizeof(glimpse_pts::server_packet_header) + sizeof(glimpse_pts::sequenced_message_type) + 29, "unexpected sizeof order_added_message");

#pragma pack(pop)
}
