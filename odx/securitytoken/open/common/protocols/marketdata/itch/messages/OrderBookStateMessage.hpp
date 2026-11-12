#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderbookId.hpp"
#include "../types/StateName.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxsecuritytoken::pts::itch::v1_2 {

namespace itch_pts = ::odx::odxsecuritytoken::pts::itch::v1_2;

#pragma pack(push, 1)

// Order Book State Message
struct order_book_state_message {

    struct fields_type {
        itch_pts::nanoseconds nanoseconds;
        itch_pts::orderbook_id orderbook_id;
        itch_pts::state_name state_name;
    };

    itch_pts::server_packet_header header = {std::uint16_t(sizeof(itch_pts::server_packet_header) + sizeof(fields_type) + sizeof(itch_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_pts::sequenced_message_type sequenced_message_type = itch_pts::sequenced_message_type::enum_type::order_book_state_message;

    fields_type fields;

    // parse method
    static order_book_state_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_book_state_message*>(buffer);
    }

    // parse method const
    static const order_book_state_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_book_state_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_book_state_message::fields_type, nanoseconds) == 0, "unexpected offset of order_book_state_message::fields_type::nanoseconds");
static_assert(offsetof(order_book_state_message::fields_type, orderbook_id) == 4, "unexpected offset of order_book_state_message::fields_type::orderbook_id");
static_assert(offsetof(order_book_state_message::fields_type, state_name) == 8, "unexpected offset of order_book_state_message::fields_type::state_name");
static_assert(sizeof(order_book_state_message::fields_type) == 28, "unexpected sizeof order_book_state_message::fields_type");
static_assert(sizeof(order_book_state_message) == sizeof(itch_pts::server_packet_header) + sizeof(itch_pts::sequenced_message_type) + 28, "unexpected sizeof order_book_state_message");

#pragma pack(pop)
}
