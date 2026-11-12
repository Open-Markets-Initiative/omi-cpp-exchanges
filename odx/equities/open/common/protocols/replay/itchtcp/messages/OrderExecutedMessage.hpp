#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderNumber.hpp"
#include "../types/ExecutedQuantity.hpp"
#include "../types/MatchNumber.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxequities::tcppts::itch::v2_2 {

namespace itch_tcppts = ::odx::odxequities::tcppts::itch::v2_2;

#pragma pack(push, 1)

// Order Executed Message
struct order_executed_message {

    struct fields_type {
        itch_tcppts::nanoseconds nanoseconds;
        itch_tcppts::order_number order_number;
        itch_tcppts::executed_quantity executed_quantity;
        itch_tcppts::match_number match_number;
    };

    itch_tcppts::server_packet_header header = {std::uint16_t(sizeof(itch_tcppts::server_packet_header) + sizeof(fields_type) + sizeof(itch_tcppts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_tcppts::sequenced_message_type sequenced_message_type = itch_tcppts::sequenced_message_type::enum_type::order_executed_message;

    fields_type fields;

    // parse method
    static order_executed_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_executed_message*>(buffer);
    }

    // parse method const
    static const order_executed_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_executed_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_executed_message::fields_type, nanoseconds) == 0, "unexpected offset of order_executed_message::fields_type::nanoseconds");
static_assert(offsetof(order_executed_message::fields_type, order_number) == 4, "unexpected offset of order_executed_message::fields_type::order_number");
static_assert(offsetof(order_executed_message::fields_type, executed_quantity) == 12, "unexpected offset of order_executed_message::fields_type::executed_quantity");
static_assert(offsetof(order_executed_message::fields_type, match_number) == 16, "unexpected offset of order_executed_message::fields_type::match_number");
static_assert(sizeof(order_executed_message::fields_type) == 24, "unexpected sizeof order_executed_message::fields_type");
static_assert(sizeof(order_executed_message) == sizeof(itch_tcppts::server_packet_header) + sizeof(itch_tcppts::sequenced_message_type) + 24, "unexpected sizeof order_executed_message");

#pragma pack(pop)
}
