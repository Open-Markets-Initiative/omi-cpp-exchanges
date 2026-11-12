#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Nanoseconds.hpp"
#include "../types/OrderbookId.hpp"
#include "../types/OrderbookCode.hpp"
#include "../types/ExchangeSymbol.hpp"
#include "../types/Group.hpp"
#include "../types/RoundLotSize.hpp"
#include "../types/PriceTickSizeTableId.hpp"
#include "../types/PriceDecimals.hpp"
#include "../types/UpperPriceLimit.hpp"
#include "../types/LowerPriceLimit.hpp"
#include "../types/RestrictionOfMarketOrder.hpp"
#include "../types/AttentionFlag.hpp"
#include "../types/TerminationFlag.hpp"
#include "../types/SequencedMessageType.hpp"

namespace odx::odxsecuritytoken::pts::itch::v1_2 {

namespace itch_pts = ::odx::odxsecuritytoken::pts::itch::v1_2;

#pragma pack(push, 1)

// Orderbook Directory Message
struct orderbook_directory_message {

    struct fields_type {
        itch_pts::nanoseconds nanoseconds;
        itch_pts::orderbook_id orderbook_id;
        itch_pts::orderbook_code orderbook_code;
        itch_pts::exchange_symbol exchange_symbol;
        itch_pts::group group;
        itch_pts::round_lot_size round_lot_size;
        itch_pts::price_tick_size_table_id price_tick_size_table_id;
        itch_pts::price_decimals price_decimals;
        itch_pts::upper_price_limit upper_price_limit;
        itch_pts::lower_price_limit lower_price_limit;
        itch_pts::restriction_of_market_order restriction_of_market_order;
        itch_pts::attention_flag attention_flag;
        itch_pts::termination_flag termination_flag;
    };

    itch_pts::server_packet_header header = {std::uint16_t(sizeof(itch_pts::server_packet_header) + sizeof(fields_type) + sizeof(itch_pts::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    itch_pts::sequenced_message_type sequenced_message_type = itch_pts::sequenced_message_type::enum_type::orderbook_directory_message;

    fields_type fields;

    // parse method
    static orderbook_directory_message* parse(std::byte* buffer) {
        return reinterpret_cast<orderbook_directory_message*>(buffer);
    }

    // parse method const
    static const orderbook_directory_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const orderbook_directory_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(orderbook_directory_message::fields_type, nanoseconds) == 0, "unexpected offset of orderbook_directory_message::fields_type::nanoseconds");
static_assert(offsetof(orderbook_directory_message::fields_type, orderbook_id) == 4, "unexpected offset of orderbook_directory_message::fields_type::orderbook_id");
static_assert(offsetof(orderbook_directory_message::fields_type, orderbook_code) == 8, "unexpected offset of orderbook_directory_message::fields_type::orderbook_code");
static_assert(offsetof(orderbook_directory_message::fields_type, exchange_symbol) == 20, "unexpected offset of orderbook_directory_message::fields_type::exchange_symbol");
static_assert(offsetof(orderbook_directory_message::fields_type, group) == 32, "unexpected offset of orderbook_directory_message::fields_type::group");
static_assert(offsetof(orderbook_directory_message::fields_type, round_lot_size) == 36, "unexpected offset of orderbook_directory_message::fields_type::round_lot_size");
static_assert(offsetof(orderbook_directory_message::fields_type, price_tick_size_table_id) == 40, "unexpected offset of orderbook_directory_message::fields_type::price_tick_size_table_id");
static_assert(offsetof(orderbook_directory_message::fields_type, price_decimals) == 44, "unexpected offset of orderbook_directory_message::fields_type::price_decimals");
static_assert(offsetof(orderbook_directory_message::fields_type, upper_price_limit) == 48, "unexpected offset of orderbook_directory_message::fields_type::upper_price_limit");
static_assert(offsetof(orderbook_directory_message::fields_type, lower_price_limit) == 52, "unexpected offset of orderbook_directory_message::fields_type::lower_price_limit");
static_assert(offsetof(orderbook_directory_message::fields_type, restriction_of_market_order) == 56, "unexpected offset of orderbook_directory_message::fields_type::restriction_of_market_order");
static_assert(offsetof(orderbook_directory_message::fields_type, attention_flag) == 57, "unexpected offset of orderbook_directory_message::fields_type::attention_flag");
static_assert(offsetof(orderbook_directory_message::fields_type, termination_flag) == 58, "unexpected offset of orderbook_directory_message::fields_type::termination_flag");
static_assert(sizeof(orderbook_directory_message::fields_type) == 59, "unexpected sizeof orderbook_directory_message::fields_type");
static_assert(sizeof(orderbook_directory_message) == sizeof(itch_pts::server_packet_header) + sizeof(itch_pts::sequenced_message_type) + 59, "unexpected sizeof orderbook_directory_message");

#pragma pack(pop)
}
