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

namespace odx::odxequities::tcppts::itch::v2_2 {

namespace itch_tcppts = ::odx::odxequities::tcppts::itch::v2_2;

#pragma pack(push, 1)

// Order Added Message
struct order_added_message {

    struct fields_type {
        itch_tcppts::nanoseconds nanoseconds;
        itch_tcppts::order_number order_number;
        itch_tcppts::buy_sell_indicator buy_sell_indicator;
        itch_tcppts::quantity quantity;
        itch_tcppts::orderbook_id orderbook_id;
        itch_tcppts::group group;
        itch_tcppts::price price;
    };

    itch_tcppts::server_packet_header header = {std::uint16_t(sizeof(itch_tcppts::server_packet_header) + sizeof(fields_type) + sizeof(itch_tcppts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_tcppts::sequenced_message_type sequenced_message_type = itch_tcppts::sequenced_message_type::enum_type::order_added_message;

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
static_assert(sizeof(order_added_message) == sizeof(itch_tcppts::server_packet_header) + sizeof(itch_tcppts::sequenced_message_type) + 29, "unexpected sizeof order_added_message");

#pragma pack(pop)
}
