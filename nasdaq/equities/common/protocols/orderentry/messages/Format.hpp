#pragma once

#include <ostream>
#include <string_view>

#include "DebugPacket.hpp"
#include "LoginRequestPacket.hpp"
#include "UnsequencedDataPacket.hpp"
#include "EnterOrderMessage.hpp"
#include "ReplaceOrderMessage.hpp"
#include "CancelOrderMessage.hpp"
#include "ModifyOrderMessage.hpp"
#include "AccountQueryMessage.hpp"
#include "MassCancelRequestMessage.hpp"
#include "DisableOrderEntryRequestMessage.hpp"
#include "EnableOrderEntryRequestMessage.hpp"
#include "LoginAcceptedPacket.hpp"
#include "LoginRejectedPacket.hpp"
#include "SequencedDataPacket.hpp"
#include "SystemEventMessage.hpp"
#include "OrderAcceptedMessage.hpp"
#include "ReplacedMessage.hpp"
#include "CanceledMessage.hpp"
#include "AiqCanceledMessage.hpp"
#include "OrderExecutedMessage.hpp"
#include "BrokenTradeMessage.hpp"
#include "RejectedOrderMessage.hpp"
#include "CancelPendingMessage.hpp"
#include "CancelRejectMessage.hpp"
#include "OrderPriorityUpdateMessage.hpp"
#include "OrderModifiedMessage.hpp"
#include "OrderRestatedMessage.hpp"
#include "AccountQueryResponseMessage.hpp"
#include "MassCancelResponseMessage.hpp"
#include "DisableOrderEntryResponseMessage.hpp"
#include "EnableOrderEntryResponseMessage.hpp"
#include "../json/messages/debug_packet_json.hpp"
#include "../json/messages/unsequenced_data_packet_json.hpp"
#include "../json/messages/enter_order_message_json.hpp"
#include "../json/messages/replace_order_message_json.hpp"
#include "../json/messages/cancel_order_message_json.hpp"
#include "../json/messages/modify_order_message_json.hpp"
#include "../json/messages/account_query_message_json.hpp"
#include "../json/messages/mass_cancel_request_message_json.hpp"
#include "../json/messages/disable_order_entry_request_message_json.hpp"
#include "../json/messages/enable_order_entry_request_message_json.hpp"
#include "../json/messages/sequenced_data_packet_json.hpp"
#include "../json/messages/order_accepted_message_json.hpp"
#include "../json/messages/replaced_message_json.hpp"
#include "../json/messages/canceled_message_json.hpp"
#include "../json/messages/aiq_canceled_message_json.hpp"
#include "../json/messages/order_executed_message_json.hpp"
#include "../json/messages/broken_trade_message_json.hpp"
#include "../json/messages/rejected_order_message_json.hpp"
#include "../json/messages/cancel_pending_message_json.hpp"
#include "../json/messages/cancel_reject_message_json.hpp"
#include "../json/messages/order_priority_update_message_json.hpp"
#include "../json/messages/order_modified_message_json.hpp"
#include "../json/messages/order_restated_message_json.hpp"
#include "../json/messages/account_query_response_message_json.hpp"
#include "../json/messages/mass_cancel_response_message_json.hpp"
#include "../json/messages/disable_order_entry_response_message_json.hpp"
#include "../json/messages/enable_order_entry_response_message_json.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;

inline std::ostream& operator<<(std::ostream& os, const client_packet_header& value) {
    os << "packet_length=" << value.packet_length.get().value()
       << ",client_packet_type=\"" << ouch_orders::client_packet_type::to_string(value.client_packet_type.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const server_packet_header& value) {
    os << "packet_length=" << value.packet_length.get().value()
       << ",server_packet_type=\"" << ouch_orders::server_packet_type::to_string(value.server_packet_type.get().value()) << '"'
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
    os << "unsequenced_message_type=\"" << ouch_orders::unsequenced_message_type::to_string(msg.fields.unsequenced_message_type.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const enter_order_message& msg) {
    os << "user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",side=\"" << ouch_orders::side::to_string(msg.fields.side.get().value()) << '"'
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",symbol=\"" << msg.fields.symbol.get_trimmed().value() << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",time_in_force=\"" << ouch_orders::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",display=\"" << ouch_orders::display::to_string(msg.fields.display.get().value()) << '"'
       << ",capacity=\"" << ouch_orders::capacity::to_string(msg.fields.capacity.get().value()) << '"'
       << ",inter_market_sweep_eligibility=\"" << ouch_orders::inter_market_sweep_eligibility::to_string(msg.fields.inter_market_sweep_eligibility.get().value()) << '"'
       << ",cross_type=\"" << ouch_orders::cross_type::to_string(msg.fields.cross_type.get().value()) << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const replace_order_message& msg) {
    os << "orig_user_ref_num=" << msg.fields.orig_user_ref_num.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",time_in_force=\"" << ouch_orders::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",display=\"" << ouch_orders::display::to_string(msg.fields.display.get().value()) << '"'
       << ",inter_market_sweep_eligibility=\"" << ouch_orders::inter_market_sweep_eligibility::to_string(msg.fields.inter_market_sweep_eligibility.get().value()) << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const cancel_order_message& msg) {
    os << "user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const modify_order_message& msg) {
    os << "user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",side=\"" << ouch_orders::side::to_string(msg.fields.side.get().value()) << '"'
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const account_query_message& msg) {
    os << "appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_cancel_request_message& msg) {
    os << "user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",symbol=\"" << msg.fields.symbol.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const disable_order_entry_request_message& msg) {
    os << "user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const enable_order_entry_request_message& msg) {
    os << "user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
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
    os << "reject_reason_code=\"" << ouch_orders::reject_reason_code::to_string(msg.fields.reject_reason_code.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sequenced_data_packet& msg) {
    os << "sequenced_message_type=\"" << ouch_orders::sequenced_message_type::to_string(msg.fields.sequenced_message_type.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const system_event_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",event_code=\"" << ouch_orders::event_code::to_string(msg.fields.event_code.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_accepted_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",side=\"" << ouch_orders::side::to_string(msg.fields.side.get().value()) << '"'
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",symbol=\"" << msg.fields.symbol.get_trimmed().value() << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",time_in_force=\"" << ouch_orders::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",display=\"" << ouch_orders::display::to_string(msg.fields.display.get().value()) << '"'
       << ",order_reference_number=" << msg.fields.order_reference_number.get().value()
       << ",capacity=\"" << ouch_orders::capacity::to_string(msg.fields.capacity.get().value()) << '"'
       << ",inter_market_sweep_eligibility=\"" << ouch_orders::inter_market_sweep_eligibility::to_string(msg.fields.inter_market_sweep_eligibility.get().value()) << '"'
       << ",cross_type=\"" << ouch_orders::cross_type::to_string(msg.fields.cross_type.get().value()) << '"'
       << ",order_state=\"" << ouch_orders::order_state::to_string(msg.fields.order_state.get().value()) << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const replaced_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",orig_user_ref_num=" << msg.fields.orig_user_ref_num.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",side=\"" << ouch_orders::side::to_string(msg.fields.side.get().value()) << '"'
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",symbol=\"" << msg.fields.symbol.get_trimmed().value() << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",time_in_force=\"" << ouch_orders::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",display=\"" << ouch_orders::display::to_string(msg.fields.display.get().value()) << '"'
       << ",order_reference_number=" << msg.fields.order_reference_number.get().value()
       << ",capacity=\"" << ouch_orders::capacity::to_string(msg.fields.capacity.get().value()) << '"'
       << ",inter_market_sweep_eligibility=\"" << ouch_orders::inter_market_sweep_eligibility::to_string(msg.fields.inter_market_sweep_eligibility.get().value()) << '"'
       << ",cross_type=\"" << ouch_orders::cross_type::to_string(msg.fields.cross_type.get().value()) << '"'
       << ",order_state=\"" << ouch_orders::order_state::to_string(msg.fields.order_state.get().value()) << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const canceled_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",cancel_order_reason=\"" << ouch_orders::cancel_order_reason::to_string(msg.fields.cancel_order_reason.get().value()) << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const aiq_canceled_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",decrement_shares=" << msg.fields.decrement_shares.get().value()
       << ",order_cancel_reason=\"" << msg.fields.order_cancel_reason.get().value() << '"'
       << ",quantity_prevented_from_trading=" << msg.fields.quantity_prevented_from_trading.get().value()
       << ",execution_price=" << msg.fields.execution_price.get().value()
       << ",liquidity_flag=\"" << ouch_orders::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",aiq_strategy=\"" << ouch_orders::aiq_strategy::to_string(msg.fields.aiq_strategy.get().value()) << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_executed_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",liquidity_flag=\"" << ouch_orders::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",match_number=" << msg.fields.match_number.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const broken_trade_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",match_number=" << msg.fields.match_number.get().value()
       << ",broken_trade_reason=\"" << ouch_orders::broken_trade_reason::to_string(msg.fields.broken_trade_reason.get().value()) << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const rejected_order_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",rejected_order_reason=\"" << ouch_orders::rejected_order_reason::to_string(msg.fields.rejected_order_reason.get().value()) << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const cancel_pending_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const cancel_reject_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_priority_update_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",display=\"" << ouch_orders::display::to_string(msg.fields.display.get().value()) << '"'
       << ",order_reference_number=" << msg.fields.order_reference_number.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_modified_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",side=\"" << ouch_orders::side::to_string(msg.fields.side.get().value()) << '"'
       << ",quantity=" << msg.fields.quantity.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_restated_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",order_restated_reason=\"" << ouch_orders::order_restated_reason::to_string(msg.fields.order_restated_reason.get().value()) << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const account_query_response_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",next_user_ref_num=" << msg.fields.next_user_ref_num.get().value()
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_cancel_response_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",symbol=\"" << msg.fields.symbol.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const disable_order_entry_response_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const enable_order_entry_response_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",user_ref_num=" << msg.fields.user_ref_num.get().value()
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",appendage_length=" << msg.fields.appendage_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

}
