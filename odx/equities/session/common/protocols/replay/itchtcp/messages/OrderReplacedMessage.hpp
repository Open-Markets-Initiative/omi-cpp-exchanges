#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OriginalOrderNumber.hpp"
#include "../types/NewOrderNumber.hpp"
#include "../types/Quantity.hpp"
#include "../types/Price.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::tcppts::itch::v2_2 {

namespace itch_tcppts = ::odx::odxequities::tcppts::itch::v2_2;

#pragma pack(push, 1)

// Order Replaced Message
struct order_replaced_message {

    struct fields_type {
        itch_tcppts::nanoseconds nanoseconds;
        itch_tcppts::original_order_number original_order_number;
        itch_tcppts::new_order_number new_order_number;
        itch_tcppts::quantity quantity;
        itch_tcppts::price price;
    };

    itch_tcppts::server_packet_header header = {std::uint16_t(sizeof(itch_tcppts::server_packet_header) + sizeof(fields_type) + sizeof(itch_tcppts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_tcppts::sequenced_message_type sequenced_message_type = itch_tcppts::sequenced_message_type::enum_type::order_replaced_message;

    fields_type fields;

    // parse method
    static order_replaced_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_replaced_message*>(buffer);
    }

    // parse method const
    static const order_replaced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_replaced_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_replaced_message::fields_type, nanoseconds) == 0, "unexpected offset of order_replaced_message::fields_type::nanoseconds");
static_assert(offsetof(order_replaced_message::fields_type, original_order_number) == 4, "unexpected offset of order_replaced_message::fields_type::original_order_number");
static_assert(offsetof(order_replaced_message::fields_type, new_order_number) == 12, "unexpected offset of order_replaced_message::fields_type::new_order_number");
static_assert(offsetof(order_replaced_message::fields_type, quantity) == 20, "unexpected offset of order_replaced_message::fields_type::quantity");
static_assert(offsetof(order_replaced_message::fields_type, price) == 24, "unexpected offset of order_replaced_message::fields_type::price");
static_assert(sizeof(order_replaced_message::fields_type) == 28, "unexpected sizeof order_replaced_message::fields_type");
static_assert(sizeof(order_replaced_message) == sizeof(itch_tcppts::server_packet_header) + sizeof(itch_tcppts::sequenced_message_type) + 28, "unexpected sizeof order_replaced_message");

#pragma pack(pop)
}
