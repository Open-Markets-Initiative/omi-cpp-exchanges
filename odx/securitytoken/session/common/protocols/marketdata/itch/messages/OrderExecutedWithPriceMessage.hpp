#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderNumber.hpp"
#include "../types/ExecutedQuantity.hpp"
#include "../types/MatchNumber.hpp"
#include "../types/TradePrice.hpp"
#include "../types/OccurredAsCross.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxsecuritytoken::pts::itch::v1_2 {

namespace itch_pts = ::odx::odxsecuritytoken::pts::itch::v1_2;

#pragma pack(push, 1)

// Order Executed With Price Message
struct order_executed_with_price_message {

    struct fields_type {
        itch_pts::nanoseconds nanoseconds;
        itch_pts::order_number order_number;
        itch_pts::executed_quantity executed_quantity;
        itch_pts::match_number match_number;
        itch_pts::trade_price trade_price;
        itch_pts::occurred_as_cross occurred_as_cross;
    };

    itch_pts::server_packet_header header = {std::uint16_t(sizeof(itch_pts::server_packet_header) + sizeof(fields_type) + sizeof(itch_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_pts::sequenced_message_type sequenced_message_type = itch_pts::sequenced_message_type::enum_type::order_executed_with_price_message;

    fields_type fields;

    // parse method
    static order_executed_with_price_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_executed_with_price_message*>(buffer);
    }

    // parse method const
    static const order_executed_with_price_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_executed_with_price_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_executed_with_price_message::fields_type, nanoseconds) == 0, "unexpected offset of order_executed_with_price_message::fields_type::nanoseconds");
static_assert(offsetof(order_executed_with_price_message::fields_type, order_number) == 4, "unexpected offset of order_executed_with_price_message::fields_type::order_number");
static_assert(offsetof(order_executed_with_price_message::fields_type, executed_quantity) == 12, "unexpected offset of order_executed_with_price_message::fields_type::executed_quantity");
static_assert(offsetof(order_executed_with_price_message::fields_type, match_number) == 16, "unexpected offset of order_executed_with_price_message::fields_type::match_number");
static_assert(offsetof(order_executed_with_price_message::fields_type, trade_price) == 24, "unexpected offset of order_executed_with_price_message::fields_type::trade_price");
static_assert(offsetof(order_executed_with_price_message::fields_type, occurred_as_cross) == 28, "unexpected offset of order_executed_with_price_message::fields_type::occurred_as_cross");
static_assert(sizeof(order_executed_with_price_message::fields_type) == 29, "unexpected sizeof order_executed_with_price_message::fields_type");
static_assert(sizeof(order_executed_with_price_message) == sizeof(itch_pts::server_packet_header) + sizeof(itch_pts::sequenced_message_type) + 29, "unexpected sizeof order_executed_with_price_message");

#pragma pack(pop)
}
