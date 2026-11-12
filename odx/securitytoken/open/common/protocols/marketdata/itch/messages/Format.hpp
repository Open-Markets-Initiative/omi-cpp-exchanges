#pragma once

#include <ostream>
#include <string_view>

#include "DebugPacket.hpp"
#include "LoginRequestPacket.hpp"
#include "UnsequencedDataPacket.hpp"
#include "LoginAcceptedPacket.hpp"
#include "LoginRejectedPacket.hpp"
#include "SequencedDataPacket.hpp"
#include "SecondsMessage.hpp"
#include "SystemEventMessage.hpp"
#include "PriceTickSizeMessage.hpp"
#include "OrderbookDirectoryMessage.hpp"
#include "TradingStateMessage.hpp"
#include "OrderAddedMessage.hpp"
#include "OrderExecutedWithPriceMessage.hpp"
#include "OrderDeletedMessage.hpp"
#include "OrderReplacedMessage.hpp"
#include "OrderBookStateMessage.hpp"
#include "EquilibriumPriceUpdateMessage.hpp"
#include "../json/messages/debug_packet_json.hpp"
#include "../json/messages/unsequenced_data_packet_json.hpp"
#include "../json/messages/sequenced_data_packet_json.hpp"

namespace odx::odxsecuritytoken::pts::itch::v1_2 {

namespace itch_pts = ::odx::odxsecuritytoken::pts::itch::v1_2;

inline std::ostream& operator<<(std::ostream& os, const client_packet_header& value) {
    os << "packet_length=" << value.packet_length.get().value()
       << ",client_packet_type=\"" << itch_pts::client_packet_type::to_string(value.client_packet_type.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const server_packet_header& value) {
    os << "packet_length=" << value.packet_length.get().value()
       << ",server_packet_type=\"" << itch_pts::server_packet_type::to_string(value.server_packet_type.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const debug_packet& msg) {
    os << "tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const login_request_packet& msg) {
    os << "username=\"" << msg.fields.username.get_trimmed().value() << '"'
       << ",password=\"" << msg.fields.password.get_trimmed().value() << '"'
       << ",requested_session=\"" << msg.fields.requested_session.get_trimmed().value() << '"'
       << ",requested_sequence_number=\"" << msg.fields.requested_sequence_number.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const unsequenced_data_packet& msg) {
    os << "unsequenced_message_type=\"" << msg.fields.unsequenced_message_type.get().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const login_accepted_packet& msg) {
    os << "accepted_session=\"" << msg.fields.accepted_session.get_trimmed().value() << '"'
       << ",accepted_sequence_number=\"" << msg.fields.accepted_sequence_number.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const login_rejected_packet& msg) {
    os << "reject_reason_code=\"" << itch_pts::reject_reason_code::to_string(msg.fields.reject_reason_code.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sequenced_data_packet& msg) {
    os << "sequenced_message_type=\"" << itch_pts::sequenced_message_type::to_string(msg.fields.sequenced_message_type.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const seconds_message& msg) {
    os << "seconds=" << msg.fields.seconds.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const system_event_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",group=\"" << msg.fields.group.get_trimmed().value() << '"'
       << ",system_event=\"" << itch_pts::system_event::to_string(msg.fields.system_event.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const price_tick_size_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",price_tick_size_table_id=" << msg.fields.price_tick_size_table_id.get().value()
       << ",price_tick_size=" << msg.fields.price_tick_size.get().value()
       << ",price_start=" << msg.fields.price_start.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const orderbook_directory_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",orderbook_id=\"" << msg.fields.orderbook_id.get_trimmed().value() << '"'
       << ",orderbook_code=\"" << msg.fields.orderbook_code.get_trimmed().value() << '"'
       << ",exchange_symbol=\"" << msg.fields.exchange_symbol.get_trimmed().value() << '"'
       << ",group=\"" << msg.fields.group.get_trimmed().value() << '"'
       << ",round_lot_size=" << msg.fields.round_lot_size.get().value()
       << ",price_tick_size_table_id=" << msg.fields.price_tick_size_table_id.get().value()
       << ",price_decimals=" << msg.fields.price_decimals.get().value()
       << ",upper_price_limit=" << msg.fields.upper_price_limit.get().value()
       << ",lower_price_limit=" << msg.fields.lower_price_limit.get().value()
       << ",restriction_of_market_order=\"" << itch_pts::restriction_of_market_order::to_string(msg.fields.restriction_of_market_order.get().value()) << '"'
       << ",attention_flag=\"" << itch_pts::attention_flag::to_string(msg.fields.attention_flag.get().value()) << '"'
       << ",termination_flag=\"" << itch_pts::termination_flag::to_string(msg.fields.termination_flag.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trading_state_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",orderbook_id=\"" << msg.fields.orderbook_id.get_trimmed().value() << '"'
       << ",group=\"" << msg.fields.group.get_trimmed().value() << '"'
       << ",trading_state=\"" << itch_pts::trading_state::to_string(msg.fields.trading_state.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_added_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",order_number=" << msg.fields.order_number.get().value()
       << ",buy_sell_indicator=\"" << itch_pts::buy_sell_indicator::to_string(msg.fields.buy_sell_indicator.get().value()) << '"'
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",orderbook_id=\"" << msg.fields.orderbook_id.get_trimmed().value() << '"'
       << ",group=\"" << msg.fields.group.get_trimmed().value() << '"'
       << ",price=" << (msg.fields.price.get().has_value() ? std::to_string(msg.fields.price.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_executed_with_price_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",order_number=" << msg.fields.order_number.get().value()
       << ",executed_quantity=" << msg.fields.executed_quantity.get().value()
       << ",match_number=" << msg.fields.match_number.get().value()
       << ",trade_price=" << msg.fields.trade_price.get().value()
       << ",occurred_as_cross=\"" << itch_pts::occurred_as_cross::to_string(msg.fields.occurred_as_cross.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_deleted_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",order_number=" << msg.fields.order_number.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_replaced_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",original_order_number=" << msg.fields.original_order_number.get().value()
       << ",new_order_number=" << msg.fields.new_order_number.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",price=" << (msg.fields.price.get().has_value() ? std::to_string(msg.fields.price.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_book_state_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",orderbook_id=\"" << msg.fields.orderbook_id.get_trimmed().value() << '"'
       << ",state_name=\"" << msg.fields.state_name.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const equilibrium_price_update_message& msg) {
    os << "nanoseconds=" << msg.fields.nanoseconds.get().value()
       << ",orderbook_id=\"" << msg.fields.orderbook_id.get_trimmed().value() << '"'
       << ",equilibrium_price=" << (msg.fields.equilibrium_price.get().has_value() ? std::to_string(msg.fields.equilibrium_price.get().value()) : std::string_view("null"))
       ;
    return os;
}

}
