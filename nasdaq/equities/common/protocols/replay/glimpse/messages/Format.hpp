#pragma once

#include <ostream>
#include <string_view>

#include "DebugPacket.hpp"
#include "LoginRequestPacket.hpp"
#include "UnsequencedDataPacket.hpp"
#include "LoginAcceptedPacket.hpp"
#include "LoginRejectedPacket.hpp"
#include "SequencedDataPacket.hpp"
#include "SystemEventMessage.hpp"
#include "AddOrderMessage.hpp"
#include "AddOrderMpidAttributionMessage.hpp"
#include "StockDirectoryMessage.hpp"
#include "StockTradingActionMessage.hpp"
#include "RegShoRestrictionMessage.hpp"
#include "RetailInterestMessage.hpp"
#include "EndOfSnapshotMessage.hpp"
#include "OperationalHaltMessage.hpp"
#include "../json/messages/debug_packet_json.hpp"
#include "../json/messages/unsequenced_data_packet_json.hpp"
#include "../json/messages/sequenced_data_packet_json.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

inline std::ostream& operator<<(std::ostream& os, const client_packet_header& value) {
    os << "packet_length=" << value.packet_length.get().value()
       << ",client_packet_type=\"" << glimpse_totalview::client_packet_type::to_string(value.client_packet_type.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const server_packet_header& value) {
    os << "packet_length=" << value.packet_length.get().value()
       << ",server_packet_type=\"" << glimpse_totalview::server_packet_type::to_string(value.server_packet_type.get().value()) << '"'
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
    os << "reject_reason_code=\"" << glimpse_totalview::reject_reason_code::to_string(msg.fields.reject_reason_code.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sequenced_data_packet& msg) {
    os << "sequenced_message_type=\"" << glimpse_totalview::sequenced_message_type::to_string(msg.fields.sequenced_message_type.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const system_event_message& msg) {
    os << "stock_locate=" << msg.fields.stock_locate.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",event_code=\"" << glimpse_totalview::event_code::to_string(msg.fields.event_code.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const add_order_message& msg) {
    os << "stock_locate=" << msg.fields.stock_locate.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",order_reference_number=" << msg.fields.order_reference_number.get().value()
       << ",buy_sell_indicator=\"" << glimpse_totalview::buy_sell_indicator::to_string(msg.fields.buy_sell_indicator.get().value()) << '"'
       << ",shares=" << msg.fields.shares.get().value()
       << ",stock=\"" << msg.fields.stock.get_trimmed().value() << '"'
       << ",price=" << msg.fields.price.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const add_order_mpid_attribution_message& msg) {
    os << "stock_locate=" << msg.fields.stock_locate.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",order_reference_number=" << msg.fields.order_reference_number.get().value()
       << ",buy_sell_indicator=\"" << glimpse_totalview::buy_sell_indicator::to_string(msg.fields.buy_sell_indicator.get().value()) << '"'
       << ",shares=" << msg.fields.shares.get().value()
       << ",stock=\"" << msg.fields.stock.get_trimmed().value() << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",attribution=\"" << msg.fields.attribution.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const stock_directory_message& msg) {
    os << "stock_locate=" << msg.fields.stock_locate.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",stock=\"" << msg.fields.stock.get_trimmed().value() << '"'
       << ",market_category=\"" << glimpse_totalview::market_category::to_string(msg.fields.market_category.get().value()) << '"'
       << ",financial_status_indicator=\"" << glimpse_totalview::financial_status_indicator::to_string(msg.fields.financial_status_indicator.get().value()) << '"'
       << ",round_lot_size=" << msg.fields.round_lot_size.get().value()
       << ",round_lots_only=\"" << msg.fields.round_lots_only.get().value() << '"'
       << ",issue_classification=\"" << msg.fields.issue_classification.get().value() << '"'
       << ",issue_sub_type=\"" << msg.fields.issue_sub_type.get_trimmed().value() << '"'
       << ",authenticity=\"" << glimpse_totalview::authenticity::to_string(msg.fields.authenticity.get().value()) << '"'
       << ",short_sale_threshold_indicator=\"" << msg.fields.short_sale_threshold_indicator.get().value() << '"'
       << ",ipo_flag=\"" << msg.fields.ipo_flag.get().value() << '"'
       << ",luld_reference_price_tier=\"" << msg.fields.luld_reference_price_tier.get().value() << '"'
       << ",etp_flag=\"" << msg.fields.etp_flag.get().value() << '"'
       << ",etp_leverage_factor=" << msg.fields.etp_leverage_factor.get().value()
       << ",inverse_indicator=\"" << msg.fields.inverse_indicator.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const stock_trading_action_message& msg) {
    os << "stock_locate=" << msg.fields.stock_locate.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",stock=\"" << msg.fields.stock.get_trimmed().value() << '"'
       << ",trading_state=\"" << glimpse_totalview::trading_state::to_string(msg.fields.trading_state.get().value()) << '"'
       << ",reason=\"" << msg.fields.reason.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const reg_sho_restriction_message& msg) {
    os << "locate_code=" << msg.fields.locate_code.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",stock=\"" << msg.fields.stock.get_trimmed().value() << '"'
       << ",reg_sho_action=\"" << glimpse_totalview::reg_sho_action::to_string(msg.fields.reg_sho_action.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const retail_interest_message& msg) {
    os << "stock_locate=" << msg.fields.stock_locate.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",stock=\"" << msg.fields.stock.get_trimmed().value() << '"'
       << ",interest_flag=\"" << glimpse_totalview::interest_flag::to_string(msg.fields.interest_flag.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const end_of_snapshot_message& msg) {
    os << "sequence_number=\"" << msg.fields.sequence_number.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const operational_halt_message& msg) {
    os << "stock_locate=" << msg.fields.stock_locate.get().value()
       << ",tracking_number=" << msg.fields.tracking_number.get().value()
       << ",timestamp=" << msg.fields.timestamp.get().value()
       << ",stock=\"" << msg.fields.stock.get_trimmed().value() << '"'
       << ",market_code=\"" << glimpse_totalview::market_code::to_string(msg.fields.market_code.get().value()) << '"'
       << ",operational_halt_action=\"" << glimpse_totalview::operational_halt_action::to_string(msg.fields.operational_halt_action.get().value()) << '"'
       ;
    return os;
}

}
