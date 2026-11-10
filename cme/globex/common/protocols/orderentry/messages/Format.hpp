#pragma once

#include <ostream>
#include <string_view>

#include "Negotiate.hpp"
#include "Establish.hpp"
#include "Sequence.hpp"
#include "Terminate.hpp"
#include "RetransmitRequest.hpp"
#include "NewOrderSingle.hpp"
#include "OrderCancelReplaceRequest.hpp"
#include "OrderCancelRequest.hpp"
#include "MassQuote.hpp"
#include "PartyDetailsDefinitionRequest.hpp"
#include "QuoteCancel.hpp"
#include "OrderMassActionRequest.hpp"
#include "OrderMassStatusRequest.hpp"
#include "OrderStatusRequest.hpp"
#include "PartyDetailsListRequest.hpp"
#include "ExecutionAck.hpp"
#include "RequestForQuote.hpp"
#include "NewOrderCross.hpp"
#include "SecurityDefinitionRequest.hpp"
#include "RequestForCross.hpp"
#include "MassQuoteRequest.hpp"
#include "QuoteCancelBySet.hpp"
#include "QuoteCancelByGroup.hpp"
#include "QuoteCancelByInstrument.hpp"
#include "NegotiationResponse.hpp"
#include "NegotiationReject.hpp"
#include "EstablishmentAck.hpp"
#include "EstablishmentReject.hpp"
#include "Retransmission.hpp"
#include "RetransmitReject.hpp"
#include "NotApplied.hpp"
#include "PartyDetailsDefinitionRequestAck.hpp"
#include "BusinessReject.hpp"
#include "ExecutionReportNew.hpp"
#include "ExecutionReportReject.hpp"
#include "ExecutionReportElimination.hpp"
#include "ExecutionReportTradeOutright.hpp"
#include "ExecutionReportTradeSpread.hpp"
#include "ExecutionReportTradeSpreadLeg.hpp"
#include "ExecutionReportModify.hpp"
#include "ExecutionReportStatus.hpp"
#include "ExecutionReportCancel.hpp"
#include "OrderCancelReject.hpp"
#include "OrderCancelReplaceReject.hpp"
#include "PartyDetailsListReport.hpp"
#include "MassQuoteAck.hpp"
#include "RequestForQuoteAck.hpp"
#include "ExecutionReportTradeAddendumOutright.hpp"
#include "ExecutionReportTradeAddendumSpread.hpp"
#include "ExecutionReportTradeAddendumSpreadLeg.hpp"
#include "SecurityDefinitionResponse.hpp"
#include "OrderMassActionReport.hpp"
#include "QuoteCancelAck.hpp"
#include "MassQuoteResponse.hpp"
#include "../json/messages/negotiate_json.hpp"
#include "../json/messages/establish_json.hpp"
#include "../json/messages/mass_quote_json.hpp"
#include "../json/messages/party_details_definition_request_json.hpp"
#include "../json/messages/quote_cancel_json.hpp"
#include "../json/messages/party_details_list_request_json.hpp"
#include "../json/messages/request_for_quote_json.hpp"
#include "../json/messages/new_order_cross_json.hpp"
#include "../json/messages/security_definition_request_json.hpp"
#include "../json/messages/request_for_cross_json.hpp"
#include "../json/messages/mass_quote_request_json.hpp"
#include "../json/messages/quote_cancel_by_set_json.hpp"
#include "../json/messages/quote_cancel_by_group_json.hpp"
#include "../json/messages/quote_cancel_by_instrument_json.hpp"
#include "../json/messages/negotiation_response_json.hpp"
#include "../json/messages/party_details_definition_request_ack_json.hpp"
#include "../json/messages/execution_report_trade_outright_json.hpp"
#include "../json/messages/execution_report_trade_spread_json.hpp"
#include "../json/messages/execution_report_trade_spread_leg_json.hpp"
#include "../json/messages/party_details_list_report_json.hpp"
#include "../json/messages/mass_quote_ack_json.hpp"
#include "../json/messages/execution_report_trade_addendum_outright_json.hpp"
#include "../json/messages/execution_report_trade_addendum_spread_json.hpp"
#include "../json/messages/execution_report_trade_addendum_spread_leg_json.hpp"
#include "../json/messages/security_definition_response_json.hpp"
#include "../json/messages/order_mass_action_report_json.hpp"
#include "../json/messages/quote_cancel_ack_json.hpp"
#include "../json/messages/mass_quote_response_json.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

inline std::ostream& operator<<(std::ostream& os, const message_header& value) {
    os << "block_length=" << value.block_length.get().value()
       << ",template_id=\"" << sbe_ilink3::template_id::to_string(value.template_id.get().value()) << '"'
       << ",schema_id=" << value.schema_id.get().value()
       << ",version=" << value.version.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const party_details_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trd_reg_publications_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_sets_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const requesting_party_ids_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const party_ids_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const related_sym_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sides_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const request_legs_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const broken_dates_request_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_quote_request_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_by_set_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_by_group_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_by_instrument_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const fills_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const outright_order_events_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trade_legs_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_order_events_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_leg_order_events_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_ack_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const outright_trade_events_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trade_addendum_legs_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_trade_events_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_leg_trade_events_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const response_legs_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const broken_dates_response_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const affected_orders_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_ack_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_ack_sets_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_quote_response_entries_groups& value) {
    os << "group_size.block_length=" << value.group_size.block_length.get().value()
       << ",group_size.num_in_group=" << static_cast<unsigned>(value.group_size.num_in_group.get().value())
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const leg_option_delta& value) {
    os << "mantissa_32=" << (value.mantissa_32.get().has_value() ? std::to_string(value.mantissa_32.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const calculated_ccy_last_qty& value) {
    os << "mantissa=" << (value.mantissa.get().has_value() ? std::to_string(value.mantissa.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const gross_trade_amt& value) {
    os << "mantissa=" << (value.mantissa.get().has_value() ? std::to_string(value.mantissa.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const contra_gross_trade_amt& value) {
    os << "mantissa=" << (value.mantissa.get().has_value() ? std::to_string(value.mantissa.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const contra_calculated_ccy_last_qty& value) {
    os << "mantissa=" << (value.mantissa.get().has_value() ? std::to_string(value.mantissa.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const volatility& value) {
    os << "mantissa=" << (value.mantissa.get().has_value() ? std::to_string(value.mantissa.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const option_delta& value) {
    os << "mantissa_32=" << (value.mantissa_32.get().has_value() ? std::to_string(value.mantissa_32.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const time_to_expiration& value) {
    os << "mantissa_32=" << (value.mantissa_32.get().has_value() ? std::to_string(value.mantissa_32.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const risk_free_rate& value) {
    os << "mantissa_32=" << (value.mantissa_32.get().has_value() ? std::to_string(value.mantissa_32.get().value()) : std::string_view("null"))
       << ",exponent=" << (value.exponent.get().has_value() ? std::to_string(value.exponent.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const negotiate& msg) {
    os << "hmac_signature=" << msg.fields.hmac_signature.get().value()
       << ",access_key_id=\"" << msg.fields.access_key_id.get_trimmed().value() << '"'
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",session=\"" << msg.fields.session.get_trimmed().value() << '"'
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const establish& msg) {
    os << "hmac_signature=" << msg.fields.hmac_signature.get().value()
       << ",access_key_id=\"" << msg.fields.access_key_id.get_trimmed().value() << '"'
       << ",trading_system_name=\"" << msg.fields.trading_system_name.get_trimmed().value() << '"'
       << ",trading_system_version=\"" << msg.fields.trading_system_version.get_trimmed().value() << '"'
       << ",trading_system_vendor=\"" << msg.fields.trading_system_vendor.get_trimmed().value() << '"'
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",next_seq_no=" << msg.fields.next_seq_no.get().value()
       << ",session=\"" << msg.fields.session.get_trimmed().value() << '"'
       << ",firm=\"" << msg.fields.firm.get_trimmed().value() << '"'
       << ",keep_alive_interval=" << msg.fields.keep_alive_interval.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sequence& msg) {
    os << "uuid=" << msg.fields.uuid.get().value()
       << ",next_seq_no=" << msg.fields.next_seq_no.get().value()
       << ",fault_tolerance_indicator=\"" << sbe_ilink3::fault_tolerance_indicator::to_string(msg.fields.fault_tolerance_indicator.get().value()) << '"'
       << ",keep_alive_interval_lapsed=\"" << sbe_ilink3::keep_alive_interval_lapsed::to_string(msg.fields.keep_alive_interval_lapsed.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const terminate& msg) {
    os << "reason=\"" << msg.fields.reason.get_trimmed().value() << '"'
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",error_codes=" << msg.fields.error_codes.get().value()
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const retransmit_request& msg) {
    os << "uuid=" << msg.fields.uuid.get().value()
       << ",last_uuid=" << (msg.fields.last_uuid.get().has_value() ? std::to_string(msg.fields.last_uuid.get().value()) : std::string_view("null"))
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",from_seq_no=" << msg.fields.from_seq_no.get().value()
       << ",msg_count_16=" << msg.fields.msg_count_16.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const new_order_single& msg) {
    os << "price_optional=" << (msg.fields.price_optional.get().has_value() ? std::to_string(msg.fields.price_optional.get().value()) : std::string_view("null"))
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",ord_type=\"" << sbe_ilink3::ord_type::to_string(msg.fields.ord_type.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",reservation_price=" << (msg.fields.reservation_price.get().has_value() ? std::to_string(msg.fields.reservation_price.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_cancel_replace_request& msg) {
    os << "price_optional=" << (msg.fields.price_optional.get().has_value() ? std::to_string(msg.fields.price_optional.get().value()) : std::string_view("null"))
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id_optional=" << (msg.fields.order_id_optional.get().has_value() ? std::to_string(msg.fields.order_id_optional.get().value()) : std::string_view("null"))
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",ord_type=\"" << sbe_ilink3::ord_type::to_string(msg.fields.ord_type.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",ofm_override=\"" << sbe_ilink3::ofm_override::to_string(msg.fields.ofm_override.get().value()) << '"'
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_cancel_request& msg) {
    os << "order_id_optional=" << (msg.fields.order_id_optional.get().has_value() ? std::to_string(msg.fields.order_id_optional.get().value()) : std::string_view("null"))
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",orig_order_user=\"" << msg.fields.orig_order_user.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_quote& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",quote_req_id_optional=" << (msg.fields.quote_req_id_optional.get().has_value() ? std::to_string(msg.fields.quote_req_id_optional.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",tot_no_quote_entries=" << static_cast<unsigned>(msg.fields.tot_no_quote_entries.get().value())
       << ",mm_protection_reset=\"" << sbe_ilink3::mm_protection_reset::to_string(msg.fields.mm_protection_reset.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",unused_30=\"" << msg.fields.unused_30.get_trimmed().value() << '"'
       << ",future_30=\"" << msg.fields.future_30.get_trimmed().value() << '"'
       << ",quote_entry_open=\"" << sbe_ilink3::quote_entry_open::to_string(msg.fields.quote_entry_open.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const party_details_definition_request& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",list_update_action=\"" << sbe_ilink3::list_update_action::to_string(msg.fields.list_update_action.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",memo=\"" << msg.fields.memo.get_trimmed().value() << '"'
       << ",avg_px_group_id=\"" << msg.fields.avg_px_group_id.get_trimmed().value() << '"'
       << ",self_match_prevention_id=" << (msg.fields.self_match_prevention_id.get().has_value() ? std::to_string(msg.fields.self_match_prevention_id.get().value()) : std::string_view("null"))
       << ",cmta_giveup_cd=\"" << sbe_ilink3::cmta_giveup_cd::to_string(msg.fields.cmta_giveup_cd.get().value()) << '"'
       << ",cust_order_capacity=\"" << sbe_ilink3::cust_order_capacity::to_string(msg.fields.cust_order_capacity.get().value()) << '"'
       << ",clearing_account_type=\"" << sbe_ilink3::clearing_account_type::to_string(msg.fields.clearing_account_type.get().value()) << '"'
       << ",self_match_prevention_instruction=\"" << sbe_ilink3::self_match_prevention_instruction::to_string(msg.fields.self_match_prevention_instruction.get().value()) << '"'
       << ",avg_px_indicator=\"" << sbe_ilink3::avg_px_indicator::to_string(msg.fields.avg_px_indicator.get().value()) << '"'
       << ",clearing_trade_price_type=\"" << sbe_ilink3::clearing_trade_price_type::to_string(msg.fields.clearing_trade_price_type.get().value()) << '"'
       << ",cust_order_handling_inst=\"" << sbe_ilink3::cust_order_handling_inst::to_string(msg.fields.cust_order_handling_inst.get().value()) << '"'
       << ",executor=" << (msg.fields.executor.get().has_value() ? std::to_string(msg.fields.executor.get().value()) : std::string_view("null"))
       << ",idm_short_code=" << (msg.fields.idm_short_code.get().has_value() ? std::to_string(msg.fields.idm_short_code.get().value()) : std::string_view("null"))
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",quote_cancel_type=\"" << sbe_ilink3::quote_cancel_type::to_string(msg.fields.quote_cancel_type.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",orig_order_user=\"" << msg.fields.orig_order_user.get_trimmed().value() << '"'
       << ",quote_entry_open=\"" << sbe_ilink3::quote_entry_open::to_string(msg.fields.quote_entry_open.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_mass_action_request& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",security_group=\"" << msg.fields.security_group.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id_optional=" << (msg.fields.security_id_optional.get().has_value() ? std::to_string(msg.fields.security_id_optional.get().value()) : std::string_view("null"))
       << ",mass_action_scope=\"" << sbe_ilink3::mass_action_scope::to_string(msg.fields.mass_action_scope.get().value()) << '"'
       << ",market_segment_id=" << (msg.fields.market_segment_id.get().has_value() ? std::to_string(msg.fields.market_segment_id.get().value()) : std::string_view("null"))
       << ",mass_cancel_request_type=\"" << sbe_ilink3::mass_cancel_request_type::to_string(msg.fields.mass_cancel_request_type.get().value()) << '"'
       << ",side_optional=\"" << sbe_ilink3::side_optional::to_string(msg.fields.side_optional.get().value()) << '"'
       << ",mass_action_ord_typ=\"" << sbe_ilink3::mass_action_ord_typ::to_string(msg.fields.mass_action_ord_typ.get().value()) << '"'
       << ",mass_cancel_tif=\"" << sbe_ilink3::mass_cancel_tif::to_string(msg.fields.mass_cancel_tif.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",orig_order_user=\"" << msg.fields.orig_order_user.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_mass_status_request& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",mass_status_req_id=" << msg.fields.mass_status_req_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",security_group=\"" << msg.fields.security_group.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id_optional=" << (msg.fields.security_id_optional.get().has_value() ? std::to_string(msg.fields.security_id_optional.get().value()) : std::string_view("null"))
       << ",mass_status_req_type=\"" << sbe_ilink3::mass_status_req_type::to_string(msg.fields.mass_status_req_type.get().value()) << '"'
       << ",ord_status_req_type=\"" << sbe_ilink3::ord_status_req_type::to_string(msg.fields.ord_status_req_type.get().value()) << '"'
       << ",mass_status_tif=\"" << sbe_ilink3::mass_status_tif::to_string(msg.fields.mass_status_tif.get().value()) << '"'
       << ",market_segment_id=" << (msg.fields.market_segment_id.get().has_value() ? std::to_string(msg.fields.market_segment_id.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_status_request& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",ord_status_req_id=" << msg.fields.ord_status_req_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const party_details_list_request& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_ack& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",exec_ack_status=\"" << sbe_ilink3::exec_ack_status::to_string(msg.fields.exec_ack_status.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",sec_exec_id=" << msg.fields.sec_exec_id.get().value()
       << ",last_px=" << msg.fields.last_px.get().value()
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",last_qty=" << msg.fields.last_qty.get().value()
       << ",dk_reason=\"" << sbe_ilink3::dk_reason::to_string(msg.fields.dk_reason.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const request_for_quote& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",quote_req_id=" << msg.fields.quote_req_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_type=\"" << sbe_ilink3::quote_type::to_string(msg.fields.quote_type.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const new_order_cross& msg) {
    os << "cross_id=" << msg.fields.cross_id.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",trans_bkd_time=" << msg.fields.trans_bkd_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const security_definition_request& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",security_req_id=" << msg.fields.security_req_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id_optional=\"" << msg.fields.sender_id_optional.get_trimmed().value() << '"'
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",security_sub_type=\"" << msg.fields.security_sub_type.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",start_date=" << (msg.fields.start_date.get().has_value() ? std::to_string(msg.fields.start_date.get().value()) : std::string_view("null"))
       << ",end_date=" << (msg.fields.end_date.get().has_value() ? std::to_string(msg.fields.end_date.get().value()) : std::string_view("null"))
       << ",max_no_of_substitutions=" << (msg.fields.max_no_of_substitutions.get().has_value() ? std::to_string(msg.fields.max_no_of_substitutions.get().value()) : std::string_view("null"))
       << ",source_repo_id=" << (msg.fields.source_repo_id.get().has_value() ? std::to_string(msg.fields.source_repo_id.get().value()) : std::string_view("null"))
       << ",broken_date_term_type=" << (msg.fields.broken_date_term_type.get().has_value() ? std::to_string(msg.fields.broken_date_term_type.get().value()) : std::string_view("null"))
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const request_for_cross& msg) {
    os << "cross_id=" << msg.fields.cross_id.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",cross_type=\"" << sbe_ilink3::cross_type::to_string(msg.fields.cross_type.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_quote_request& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",quote_req_id_optional=" << (msg.fields.quote_req_id_optional.get().has_value() ? std::to_string(msg.fields.quote_req_id_optional.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",tot_no_quote_entries=" << static_cast<unsigned>(msg.fields.tot_no_quote_entries.get().value())
       << ",mm_protection_reset=\"" << sbe_ilink3::mm_protection_reset::to_string(msg.fields.mm_protection_reset.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",quote_entry_open=\"" << sbe_ilink3::quote_entry_open::to_string(msg.fields.quote_entry_open.get().value()) << '"'
       << ",quote_entry_control=\"" << sbe_ilink3::quote_entry_control::to_string(msg.fields.quote_entry_control.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_by_set& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",cxl_linked_sessions=\"" << sbe_ilink3::cxl_linked_sessions::to_string(msg.fields.cxl_linked_sessions.get().value()) << '"'
       << ",quote_entry_control=\"" << sbe_ilink3::quote_entry_control::to_string(msg.fields.quote_entry_control.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_by_group& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",cxl_linked_sessions=\"" << sbe_ilink3::cxl_linked_sessions::to_string(msg.fields.cxl_linked_sessions.get().value()) << '"'
       << ",quote_entry_control=\"" << sbe_ilink3::quote_entry_control::to_string(msg.fields.quote_entry_control.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_by_instrument& msg) {
    os << "party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",seq_num=" << msg.fields.seq_num.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",cxl_linked_sessions=\"" << sbe_ilink3::cxl_linked_sessions::to_string(msg.fields.cxl_linked_sessions.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const negotiation_response& msg) {
    os << "uuid=" << msg.fields.uuid.get().value()
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",secret_key_secure_id_expiration=" << (msg.fields.secret_key_secure_id_expiration.get().has_value() ? std::to_string(msg.fields.secret_key_secure_id_expiration.get().value()) : std::string_view("null"))
       << ",fault_tolerance_indicator=\"" << sbe_ilink3::fault_tolerance_indicator::to_string(msg.fields.fault_tolerance_indicator.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",previous_seq_no=" << msg.fields.previous_seq_no.get().value()
       << ",previous_uuid=" << msg.fields.previous_uuid.get().value()
       << ",environment_indicator=" << (msg.fields.environment_indicator.get().has_value() ? std::to_string(msg.fields.environment_indicator.get().value()) : std::string_view("null"))
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const negotiation_reject& msg) {
    os << "reason=\"" << msg.fields.reason.get_trimmed().value() << '"'
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",error_codes=" << msg.fields.error_codes.get().value()
       << ",fault_tolerance_indicator=\"" << sbe_ilink3::fault_tolerance_indicator::to_string(msg.fields.fault_tolerance_indicator.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",environment_indicator=" << (msg.fields.environment_indicator.get().has_value() ? std::to_string(msg.fields.environment_indicator.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const establishment_ack& msg) {
    os << "uuid=" << msg.fields.uuid.get().value()
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",next_seq_no=" << msg.fields.next_seq_no.get().value()
       << ",previous_seq_no=" << msg.fields.previous_seq_no.get().value()
       << ",previous_uuid=" << msg.fields.previous_uuid.get().value()
       << ",keep_alive_interval=" << msg.fields.keep_alive_interval.get().value()
       << ",secret_key_secure_id_expiration=" << (msg.fields.secret_key_secure_id_expiration.get().has_value() ? std::to_string(msg.fields.secret_key_secure_id_expiration.get().value()) : std::string_view("null"))
       << ",fault_tolerance_indicator=\"" << sbe_ilink3::fault_tolerance_indicator::to_string(msg.fields.fault_tolerance_indicator.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",environment_indicator=" << (msg.fields.environment_indicator.get().has_value() ? std::to_string(msg.fields.environment_indicator.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const establishment_reject& msg) {
    os << "reason=\"" << msg.fields.reason.get_trimmed().value() << '"'
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",next_seq_no=" << msg.fields.next_seq_no.get().value()
       << ",error_codes=" << msg.fields.error_codes.get().value()
       << ",fault_tolerance_indicator=\"" << sbe_ilink3::fault_tolerance_indicator::to_string(msg.fields.fault_tolerance_indicator.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",environment_indicator=" << (msg.fields.environment_indicator.get().has_value() ? std::to_string(msg.fields.environment_indicator.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const retransmission& msg) {
    os << "uuid=" << msg.fields.uuid.get().value()
       << ",last_uuid=" << (msg.fields.last_uuid.get().has_value() ? std::to_string(msg.fields.last_uuid.get().value()) : std::string_view("null"))
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",from_seq_no=" << msg.fields.from_seq_no.get().value()
       << ",msg_count_16=" << msg.fields.msg_count_16.get().value()
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const retransmit_reject& msg) {
    os << "reason=\"" << msg.fields.reason.get_trimmed().value() << '"'
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",last_uuid=" << (msg.fields.last_uuid.get().has_value() ? std::to_string(msg.fields.last_uuid.get().value()) : std::string_view("null"))
       << ",request_timestamp=" << msg.fields.request_timestamp.get().value()
       << ",error_codes=" << msg.fields.error_codes.get().value()
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const not_applied& msg) {
    os << "uuid=" << msg.fields.uuid.get().value()
       << ",from_seq_no=" << msg.fields.from_seq_no.get().value()
       << ",msg_count=" << msg.fields.msg_count.get().value()
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const party_details_definition_request_ack& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",memo=\"" << msg.fields.memo.get_trimmed().value() << '"'
       << ",avg_px_group_id=\"" << msg.fields.avg_px_group_id.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",self_match_prevention_id=" << (msg.fields.self_match_prevention_id.get().has_value() ? std::to_string(msg.fields.self_match_prevention_id.get().value()) : std::string_view("null"))
       << ",party_detail_request_status=" << static_cast<unsigned>(msg.fields.party_detail_request_status.get().value())
       << ",cust_order_capacity=\"" << sbe_ilink3::cust_order_capacity::to_string(msg.fields.cust_order_capacity.get().value()) << '"'
       << ",clearing_account_type=\"" << sbe_ilink3::clearing_account_type::to_string(msg.fields.clearing_account_type.get().value()) << '"'
       << ",self_match_prevention_instruction=\"" << sbe_ilink3::self_match_prevention_instruction::to_string(msg.fields.self_match_prevention_instruction.get().value()) << '"'
       << ",avg_px_indicator=\"" << sbe_ilink3::avg_px_indicator::to_string(msg.fields.avg_px_indicator.get().value()) << '"'
       << ",clearing_trade_price_type=\"" << sbe_ilink3::clearing_trade_price_type::to_string(msg.fields.clearing_trade_price_type.get().value()) << '"'
       << ",cmta_giveup_cd=\"" << sbe_ilink3::cmta_giveup_cd::to_string(msg.fields.cmta_giveup_cd.get().value()) << '"'
       << ",cust_order_handling_inst=\"" << sbe_ilink3::cust_order_handling_inst::to_string(msg.fields.cust_order_handling_inst.get().value()) << '"'
       << ",list_update_action=\"" << sbe_ilink3::list_update_action::to_string(msg.fields.list_update_action.get().value()) << '"'
       << ",party_detail_definition_status=" << static_cast<unsigned>(msg.fields.party_detail_definition_status.get().value())
       << ",executor=" << (msg.fields.executor.get().has_value() ? std::to_string(msg.fields.executor.get().value()) : std::string_view("null"))
       << ",idm_short_code=" << (msg.fields.idm_short_code.get().has_value() ? std::to_string(msg.fields.idm_short_code.get().value()) : std::string_view("null"))
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const business_reject& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",sender_id_optional=\"" << msg.fields.sender_id_optional.get_trimmed().value() << '"'
       << ",party_details_list_req_id_optional=" << (msg.fields.party_details_list_req_id_optional.get().has_value() ? std::to_string(msg.fields.party_details_list_req_id_optional.get().value()) : std::string_view("null"))
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",business_reject_ref_id=" << (msg.fields.business_reject_ref_id.get().has_value() ? std::to_string(msg.fields.business_reject_ref_id.get().value()) : std::string_view("null"))
       << ",location_optional=\"" << msg.fields.location_optional.get_trimmed().value() << '"'
       << ",ref_seq_num=" << (msg.fields.ref_seq_num.get().has_value() ? std::to_string(msg.fields.ref_seq_num.get().value()) : std::string_view("null"))
       << ",ref_tag_id=" << (msg.fields.ref_tag_id.get().has_value() ? std::to_string(msg.fields.ref_tag_id.get().value()) : std::string_view("null"))
       << ",business_reject_reason=" << msg.fields.business_reject_reason.get().value()
       << ",ref_msg_type=\"" << msg.fields.ref_msg_type.get_trimmed().value() << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",manual_order_indicator_optional=\"" << sbe_ilink3::manual_order_indicator_optional::to_string(msg.fields.manual_order_indicator_optional.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_new& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",reservation_price=" << (msg.fields.reservation_price.get().has_value() ? std::to_string(msg.fields.reservation_price.get().value()) : std::string_view("null"))
       << ",priority_indicator=" << (msg.fields.priority_indicator.get().has_value() ? std::to_string(msg.fields.priority_indicator.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_reject& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price_optional=" << (msg.fields.price_optional.get().has_value() ? std::to_string(msg.fields.price_optional.get().value()) : std::string_view("null"))
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",ord_rej_reason=" << msg.fields.ord_rej_reason.get().value()
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",reservation_price=" << (msg.fields.reservation_price.get().has_value() ? std::to_string(msg.fields.reservation_price.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_elimination& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",cum_qty=" << msg.fields.cum_qty.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",reservation_price=" << (msg.fields.reservation_price.get().has_value() ? std::to_string(msg.fields.reservation_price.get().value()) : std::string_view("null"))
       << ",priority_indicator=" << (msg.fields.priority_indicator.get().has_value() ? std::to_string(msg.fields.priority_indicator.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_trade_outright& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",last_px=" << msg.fields.last_px.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",sec_exec_id=" << msg.fields.sec_exec_id.get().value()
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",last_qty=" << msg.fields.last_qty.get().value()
       << ",cum_qty=" << msg.fields.cum_qty.get().value()
       << ",md_trade_entry_id=" << msg.fields.md_trade_entry_id.get().value()
       << ",side_trade_id=" << msg.fields.side_trade_id.get().value()
       << ",trade_link_id=" << (msg.fields.trade_link_id.get().has_value() ? std::to_string(msg.fields.trade_link_id.get().value()) : std::string_view("null"))
       << ",leaves=" << msg.fields.leaves.get().value()
       << ",trade_date=" << (msg.fields.trade_date.get().has_value() ? std::to_string(msg.fields.trade_date.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",ord_status_trd=\"" << sbe_ilink3::ord_status_trd::to_string(msg.fields.ord_status_trd.get().value()) << '"'
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",aggressor_indicator=\"" << sbe_ilink3::aggressor_indicator::to_string(msg.fields.aggressor_indicator.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",ownership=" << static_cast<unsigned>(msg.fields.ownership.get().value())
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",trd_type=" << (msg.fields.trd_type.get().has_value() ? std::to_string(msg.fields.trd_type.get().value()) : std::string_view("null"))
       << ",exec_restatement_reason=\"" << sbe_ilink3::exec_restatement_reason::to_string(msg.fields.exec_restatement_reason.get().value()) << '"'
       << ",settl_date=" << (msg.fields.settl_date.get().has_value() ? std::to_string(msg.fields.settl_date.get().value()) : std::string_view("null"))
       << ",maturity_date=" << (msg.fields.maturity_date.get().has_value() ? std::to_string(msg.fields.maturity_date.get().value()) : std::string_view("null"))
       << ",calculated_ccy_last_qty=" << (msg.fields.calculated_ccy_last_qty.get().has_value() ? std::to_string(msg.fields.calculated_ccy_last_qty.get().value()) : std::string_view("null"))
       << ",gross_trade_amt=" << (msg.fields.gross_trade_amt.get().has_value() ? std::to_string(msg.fields.gross_trade_amt.get().value()) : std::string_view("null"))
       << ",benchmark_price=" << (msg.fields.benchmark_price.get().has_value() ? std::to_string(msg.fields.benchmark_price.get().value()) : std::string_view("null"))
       << ",reservation_price=" << (msg.fields.reservation_price.get().has_value() ? std::to_string(msg.fields.reservation_price.get().value()) : std::string_view("null"))
       << ",priority_indicator=" << (msg.fields.priority_indicator.get().has_value() ? std::to_string(msg.fields.priority_indicator.get().value()) : std::string_view("null"))
       << ",display_limit_price=" << (msg.fields.display_limit_price.get().has_value() ? std::to_string(msg.fields.display_limit_price.get().value()) : std::string_view("null"))
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_trade_spread& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",last_px=" << msg.fields.last_px.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",sec_exec_id=" << msg.fields.sec_exec_id.get().value()
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",last_qty=" << msg.fields.last_qty.get().value()
       << ",cum_qty=" << msg.fields.cum_qty.get().value()
       << ",md_trade_entry_id=" << msg.fields.md_trade_entry_id.get().value()
       << ",side_trade_id=" << msg.fields.side_trade_id.get().value()
       << ",leaves=" << msg.fields.leaves.get().value()
       << ",trade_date=" << (msg.fields.trade_date.get().has_value() ? std::to_string(msg.fields.trade_date.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",ord_status_trd=\"" << sbe_ilink3::ord_status_trd::to_string(msg.fields.ord_status_trd.get().value()) << '"'
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",aggressor_indicator=\"" << sbe_ilink3::aggressor_indicator::to_string(msg.fields.aggressor_indicator.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",total_num_securities=" << static_cast<unsigned>(msg.fields.total_num_securities.get().value())
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_trade_spread_leg& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",volatility=" << (msg.fields.volatility.get().has_value() ? std::to_string(msg.fields.volatility.get().value()) : std::string_view("null"))
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",last_px=" << msg.fields.last_px.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",underlying_px=" << (msg.fields.underlying_px.get().has_value() ? std::to_string(msg.fields.underlying_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",sec_exec_id=" << msg.fields.sec_exec_id.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",option_delta=" << (msg.fields.option_delta.get().has_value() ? std::to_string(msg.fields.option_delta.get().value()) : std::string_view("null"))
       << ",time_to_expiration=" << (msg.fields.time_to_expiration.get().has_value() ? std::to_string(msg.fields.time_to_expiration.get().value()) : std::string_view("null"))
       << ",risk_free_rate=" << (msg.fields.risk_free_rate.get().has_value() ? std::to_string(msg.fields.risk_free_rate.get().value()) : std::string_view("null"))
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",last_qty=" << msg.fields.last_qty.get().value()
       << ",cum_qty=" << msg.fields.cum_qty.get().value()
       << ",side_trade_id=" << msg.fields.side_trade_id.get().value()
       << ",trade_date=" << (msg.fields.trade_date.get().has_value() ? std::to_string(msg.fields.trade_date.get().value()) : std::string_view("null"))
       << ",ord_status_trd=\"" << sbe_ilink3::ord_status_trd::to_string(msg.fields.ord_status_trd.get().value()) << '"'
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",settl_date=" << (msg.fields.settl_date.get().has_value() ? std::to_string(msg.fields.settl_date.get().value()) : std::string_view("null"))
       << ",calculated_ccy_last_qty=" << (msg.fields.calculated_ccy_last_qty.get().has_value() ? std::to_string(msg.fields.calculated_ccy_last_qty.get().value()) : std::string_view("null"))
       << ",gross_trade_amt=" << (msg.fields.gross_trade_amt.get().has_value() ? std::to_string(msg.fields.gross_trade_amt.get().value()) : std::string_view("null"))
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_modify& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",cum_qty=" << msg.fields.cum_qty.get().value()
       << ",leaves=" << msg.fields.leaves.get().value()
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",priority_indicator=" << (msg.fields.priority_indicator.get().has_value() ? std::to_string(msg.fields.priority_indicator.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_status& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price_optional=" << (msg.fields.price_optional.get().has_value() ? std::to_string(msg.fields.price_optional.get().value()) : std::string_view("null"))
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",ord_status_req_id_optional=" << (msg.fields.ord_status_req_id_optional.get().has_value() ? std::to_string(msg.fields.ord_status_req_id_optional.get().value()) : std::string_view("null"))
       << ",mass_status_req_id_optional=" << (msg.fields.mass_status_req_id_optional.get().has_value() ? std::to_string(msg.fields.mass_status_req_id_optional.get().value()) : std::string_view("null"))
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",cum_qty=" << msg.fields.cum_qty.get().value()
       << ",leaves=" << msg.fields.leaves.get().value()
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",ord_status=\"" << sbe_ilink3::ord_status::to_string(msg.fields.ord_status.get().value()) << '"'
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",last_rpt_requested=\"" << sbe_ilink3::last_rpt_requested::to_string(msg.fields.last_rpt_requested.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",reservation_price=" << (msg.fields.reservation_price.get().has_value() ? std::to_string(msg.fields.reservation_price.get().value()) : std::string_view("null"))
       << ",priority_indicator=" << (msg.fields.priority_indicator.get().has_value() ? std::to_string(msg.fields.priority_indicator.get().value()) : std::string_view("null"))
       << ",orig_order_user=\"" << msg.fields.orig_order_user.get_trimmed().value() << '"'
       << ",cancel_text=\"" << msg.fields.cancel_text.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_cancel& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",stop_px=" << (msg.fields.stop_px.get().has_value() ? std::to_string(msg.fields.stop_px.get().value()) : std::string_view("null"))
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",cross_id_optional=" << (msg.fields.cross_id_optional.get().has_value() ? std::to_string(msg.fields.cross_id_optional.get().value()) : std::string_view("null"))
       << ",host_cross_id=" << (msg.fields.host_cross_id.get().has_value() ? std::to_string(msg.fields.host_cross_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",order_qty=" << msg.fields.order_qty.get().value()
       << ",cum_qty=" << msg.fields.cum_qty.get().value()
       << ",min_qty=" << (msg.fields.min_qty.get().has_value() ? std::to_string(msg.fields.min_qty.get().value()) : std::string_view("null"))
       << ",display_qty=" << (msg.fields.display_qty.get().has_value() ? std::to_string(msg.fields.display_qty.get().value()) : std::string_view("null"))
       << ",expire_date=" << (msg.fields.expire_date.get().has_value() ? std::to_string(msg.fields.expire_date.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",time_in_force=\"" << sbe_ilink3::time_in_force::to_string(msg.fields.time_in_force.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",exec_restatement_reason=\"" << sbe_ilink3::exec_restatement_reason::to_string(msg.fields.exec_restatement_reason.get().value()) << '"'
       << ",cross_type_optional=" << (msg.fields.cross_type_optional.get().has_value() ? std::to_string(msg.fields.cross_type_optional.get().value()) : std::string_view("null"))
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",reservation_price=" << (msg.fields.reservation_price.get().has_value() ? std::to_string(msg.fields.reservation_price.get().value()) : std::string_view("null"))
       << ",priority_indicator=" << (msg.fields.priority_indicator.get().has_value() ? std::to_string(msg.fields.priority_indicator.get().value()) : std::string_view("null"))
       << ",orig_order_user=\"" << msg.fields.orig_order_user.get_trimmed().value() << '"'
       << ",cancel_text=\"" << msg.fields.cancel_text.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_cancel_reject& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",cxl_rej_reason=" << msg.fields.cxl_rej_reason.get().value()
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_cancel_replace_reject& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",cxl_rej_reason=" << msg.fields.cxl_rej_reason.get().value()
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const party_details_list_report& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",avg_px_group_id=\"" << msg.fields.avg_px_group_id.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",party_details_list_report_id=" << msg.fields.party_details_list_report_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",self_match_prevention_id=" << (msg.fields.self_match_prevention_id.get().has_value() ? std::to_string(msg.fields.self_match_prevention_id.get().value()) : std::string_view("null"))
       << ",tot_num_parties=" << msg.fields.tot_num_parties.get().value()
       << ",request_result=\"" << sbe_ilink3::request_result::to_string(msg.fields.request_result.get().value()) << '"'
       << ",last_fragment=\"" << sbe_ilink3::last_fragment::to_string(msg.fields.last_fragment.get().value()) << '"'
       << ",cust_order_capacity=\"" << sbe_ilink3::cust_order_capacity::to_string(msg.fields.cust_order_capacity.get().value()) << '"'
       << ",clearing_account_type=\"" << sbe_ilink3::clearing_account_type::to_string(msg.fields.clearing_account_type.get().value()) << '"'
       << ",self_match_prevention_instruction=\"" << sbe_ilink3::self_match_prevention_instruction::to_string(msg.fields.self_match_prevention_instruction.get().value()) << '"'
       << ",avg_px_indicator=\"" << sbe_ilink3::avg_px_indicator::to_string(msg.fields.avg_px_indicator.get().value()) << '"'
       << ",clearing_trade_price_type=\"" << sbe_ilink3::clearing_trade_price_type::to_string(msg.fields.clearing_trade_price_type.get().value()) << '"'
       << ",cmta_giveup_cd=\"" << sbe_ilink3::cmta_giveup_cd::to_string(msg.fields.cmta_giveup_cd.get().value()) << '"'
       << ",cust_order_handling_inst=\"" << sbe_ilink3::cust_order_handling_inst::to_string(msg.fields.cust_order_handling_inst.get().value()) << '"'
       << ",executor=" << (msg.fields.executor.get().has_value() ? std::to_string(msg.fields.executor.get().value()) : std::string_view("null"))
       << ",idm_short_code=" << (msg.fields.idm_short_code.get().has_value() ? std::to_string(msg.fields.idm_short_code.get().value()) : std::string_view("null"))
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_quote_ack& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",request_time=" << msg.fields.request_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",quote_req_id_optional=" << (msg.fields.quote_req_id_optional.get().has_value() ? std::to_string(msg.fields.quote_req_id_optional.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",quote_reject_reason=" << (msg.fields.quote_reject_reason.get().has_value() ? std::to_string(msg.fields.quote_reject_reason.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",quote_ack_status=\"" << sbe_ilink3::quote_ack_status::to_string(msg.fields.quote_ack_status.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",no_processed_entries=" << static_cast<unsigned>(msg.fields.no_processed_entries.get().value())
       << ",mm_protection_reset=\"" << sbe_ilink3::mm_protection_reset::to_string(msg.fields.mm_protection_reset.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",tot_no_quote_entries_optional=" << (msg.fields.tot_no_quote_entries_optional.get().has_value() ? std::to_string(msg.fields.tot_no_quote_entries_optional.get().value()) : std::string_view("null"))
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",quote_entry_open=\"" << sbe_ilink3::quote_entry_open::to_string(msg.fields.quote_entry_open.get().value()) << '"'
       << ",quote_entry_control=\"" << sbe_ilink3::quote_entry_control::to_string(msg.fields.quote_entry_control.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const request_for_quote_ack& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",exchange_quote_req_id=\"" << msg.fields.exchange_quote_req_id.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",request_time=" << msg.fields.request_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",quote_req_id=" << msg.fields.quote_req_id.get().value()
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_reject_reason=" << (msg.fields.quote_reject_reason.get().has_value() ? std::to_string(msg.fields.quote_reject_reason.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",quote_ack_status=\"" << sbe_ilink3::quote_ack_status::to_string(msg.fields.quote_ack_status.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_trade_addendum_outright& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",last_px=" << msg.fields.last_px.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",sec_exec_id=" << msg.fields.sec_exec_id.get().value()
       << ",orig_secondary_execution_id=" << (msg.fields.orig_secondary_execution_id.get().has_value() ? std::to_string(msg.fields.orig_secondary_execution_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",last_qty=" << msg.fields.last_qty.get().value()
       << ",side_trade_id=" << msg.fields.side_trade_id.get().value()
       << ",orig_side_trade_id=" << (msg.fields.orig_side_trade_id.get().has_value() ? std::to_string(msg.fields.orig_side_trade_id.get().value()) : std::string_view("null"))
       << ",trade_date=" << (msg.fields.trade_date.get().has_value() ? std::to_string(msg.fields.trade_date.get().value()) : std::string_view("null"))
       << ",ord_status_trd_cxl=\"" << sbe_ilink3::ord_status_trd_cxl::to_string(msg.fields.ord_status_trd_cxl.get().value()) << '"'
       << ",exec_type=\"" << sbe_ilink3::exec_type::to_string(msg.fields.exec_type.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",discretion_price=" << (msg.fields.discretion_price.get().has_value() ? std::to_string(msg.fields.discretion_price.get().value()) : std::string_view("null"))
       << ",trd_type=" << (msg.fields.trd_type.get().has_value() ? std::to_string(msg.fields.trd_type.get().value()) : std::string_view("null"))
       << ",exec_restatement_reason=\"" << sbe_ilink3::exec_restatement_reason::to_string(msg.fields.exec_restatement_reason.get().value()) << '"'
       << ",settl_date=" << (msg.fields.settl_date.get().has_value() ? std::to_string(msg.fields.settl_date.get().value()) : std::string_view("null"))
       << ",maturity_date=" << (msg.fields.maturity_date.get().has_value() ? std::to_string(msg.fields.maturity_date.get().value()) : std::string_view("null"))
       << ",calculated_ccy_last_qty=" << (msg.fields.calculated_ccy_last_qty.get().has_value() ? std::to_string(msg.fields.calculated_ccy_last_qty.get().value()) : std::string_view("null"))
       << ",gross_trade_amt=" << (msg.fields.gross_trade_amt.get().has_value() ? std::to_string(msg.fields.gross_trade_amt.get().value()) : std::string_view("null"))
       << ",benchmark_price=" << (msg.fields.benchmark_price.get().has_value() ? std::to_string(msg.fields.benchmark_price.get().value()) : std::string_view("null"))
       << ",md_trade_entry_id=" << msg.fields.md_trade_entry_id.get().value()
       << ",exec_ref_id=\"" << msg.fields.exec_ref_id.get_trimmed().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_trade_addendum_spread& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",last_px=" << msg.fields.last_px.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",sec_exec_id=" << msg.fields.sec_exec_id.get().value()
       << ",orig_secondary_execution_id=" << (msg.fields.orig_secondary_execution_id.get().has_value() ? std::to_string(msg.fields.orig_secondary_execution_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",md_trade_entry_id=" << msg.fields.md_trade_entry_id.get().value()
       << ",last_qty=" << msg.fields.last_qty.get().value()
       << ",side_trade_id=" << msg.fields.side_trade_id.get().value()
       << ",orig_side_trade_id=" << (msg.fields.orig_side_trade_id.get().has_value() ? std::to_string(msg.fields.orig_side_trade_id.get().value()) : std::string_view("null"))
       << ",trade_date=" << (msg.fields.trade_date.get().has_value() ? std::to_string(msg.fields.trade_date.get().value()) : std::string_view("null"))
       << ",ord_status_trd_cxl=\"" << sbe_ilink3::ord_status_trd_cxl::to_string(msg.fields.ord_status_trd_cxl.get().value()) << '"'
       << ",exec_type=\"" << sbe_ilink3::exec_type::to_string(msg.fields.exec_type.get().value()) << '"'
       << ",ord_type_optional=\"" << sbe_ilink3::ord_type_optional::to_string(msg.fields.ord_type_optional.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",total_num_securities=" << static_cast<unsigned>(msg.fields.total_num_securities.get().value())
       << ",exec_inst=" << static_cast<unsigned>(msg.fields.exec_inst.get().value())
       << ",execution_mode=\"" << sbe_ilink3::execution_mode::to_string(msg.fields.execution_mode.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",managed_order=\"" << sbe_ilink3::managed_order::to_string(msg.fields.managed_order.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",exec_ref_id=\"" << msg.fields.exec_ref_id.get_trimmed().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const execution_report_trade_addendum_spread_leg& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",exec_id=\"" << msg.fields.exec_id.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",clordid=\"" << msg.fields.clordid.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",last_px=" << msg.fields.last_px.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",sec_exec_id=" << msg.fields.sec_exec_id.get().value()
       << ",orig_secondary_execution_id=" << (msg.fields.orig_secondary_execution_id.get().has_value() ? std::to_string(msg.fields.orig_secondary_execution_id.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id=" << msg.fields.security_id.get().value()
       << ",last_qty=" << msg.fields.last_qty.get().value()
       << ",side_trade_id=" << msg.fields.side_trade_id.get().value()
       << ",orig_side_trade_id=" << (msg.fields.orig_side_trade_id.get().has_value() ? std::to_string(msg.fields.orig_side_trade_id.get().value()) : std::string_view("null"))
       << ",trade_date=" << (msg.fields.trade_date.get().has_value() ? std::to_string(msg.fields.trade_date.get().value()) : std::string_view("null"))
       << ",ord_status_trd_cxl=\"" << sbe_ilink3::ord_status_trd_cxl::to_string(msg.fields.ord_status_trd_cxl.get().value()) << '"'
       << ",exec_type=\"" << sbe_ilink3::exec_type::to_string(msg.fields.exec_type.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",side=\"" << sbe_ilink3::side::to_string(msg.fields.side.get().value()) << '"'
       << ",settl_date=" << (msg.fields.settl_date.get().has_value() ? std::to_string(msg.fields.settl_date.get().value()) : std::string_view("null"))
       << ",calculated_ccy_last_qty=" << (msg.fields.calculated_ccy_last_qty.get().has_value() ? std::to_string(msg.fields.calculated_ccy_last_qty.get().value()) : std::string_view("null"))
       << ",gross_trade_amt=" << (msg.fields.gross_trade_amt.get().has_value() ? std::to_string(msg.fields.gross_trade_amt.get().value()) : std::string_view("null"))
       << ",exec_ref_id=\"" << msg.fields.exec_ref_id.get_trimmed().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const security_definition_response& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",financial_instrument_full_name=\"" << msg.fields.financial_instrument_full_name.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",symbol=\"" << msg.fields.symbol.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",security_req_id=" << msg.fields.security_req_id.get().value()
       << ",security_response_id=" << msg.fields.security_response_id.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",security_group=\"" << msg.fields.security_group.get_trimmed().value() << '"'
       << ",security_type=\"" << msg.fields.security_type.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id_optional=" << (msg.fields.security_id_optional.get().has_value() ? std::to_string(msg.fields.security_id_optional.get().value()) : std::string_view("null"))
       << ",currency=\"" << msg.fields.currency.get_trimmed().value() << '"'
       << ",maturity_month_year.year=" << (msg.fields.maturity_month_year.year.get().has_value() ? std::to_string(msg.fields.maturity_month_year.year.get().value()) : std::string_view("null"))
       << ",maturity_month_year.month=" << (msg.fields.maturity_month_year.month.get().has_value() ? std::to_string(msg.fields.maturity_month_year.month.get().value()) : std::string_view("null"))
       << ",maturity_month_year.day=" << (msg.fields.maturity_month_year.day.get().has_value() ? std::to_string(msg.fields.maturity_month_year.day.get().value()) : std::string_view("null"))
       << ",maturity_month_year.week=" << (msg.fields.maturity_month_year.week.get().has_value() ? std::to_string(msg.fields.maturity_month_year.week.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",start_date=" << (msg.fields.start_date.get().has_value() ? std::to_string(msg.fields.start_date.get().value()) : std::string_view("null"))
       << ",end_date=" << (msg.fields.end_date.get().has_value() ? std::to_string(msg.fields.end_date.get().value()) : std::string_view("null"))
       << ",max_no_of_substitutions=" << (msg.fields.max_no_of_substitutions.get().has_value() ? std::to_string(msg.fields.max_no_of_substitutions.get().value()) : std::string_view("null"))
       << ",source_repo_id=" << (msg.fields.source_repo_id.get().has_value() ? std::to_string(msg.fields.source_repo_id.get().value()) : std::string_view("null"))
       << ",termination_type=\"" << msg.fields.termination_type.get_trimmed().value() << '"'
       << ",security_response_type=\"" << sbe_ilink3::security_response_type::to_string(msg.fields.security_response_type.get().value()) << '"'
       << ",expiration_cycle=\"" << sbe_ilink3::expiration_cycle::to_string(msg.fields.expiration_cycle.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",auto_quote_request=\"" << sbe_ilink3::auto_quote_request::to_string(msg.fields.auto_quote_request.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",broken_date_term_type=" << (msg.fields.broken_date_term_type.get().has_value() ? std::to_string(msg.fields.broken_date_term_type.get().value()) : std::string_view("null"))
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_mass_action_report& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",order_request_id=" << msg.fields.order_request_id.get().value()
       << ",mass_action_report_id=" << msg.fields.mass_action_report_id.get().value()
       << ",security_group=\"" << msg.fields.security_group.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",security_id_optional=" << (msg.fields.security_id_optional.get().has_value() ? std::to_string(msg.fields.security_id_optional.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",mass_action_response=\"" << sbe_ilink3::mass_action_response::to_string(msg.fields.mass_action_response.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",mass_action_scope=\"" << sbe_ilink3::mass_action_scope::to_string(msg.fields.mass_action_scope.get().value()) << '"'
       << ",total_affected_orders=" << msg.fields.total_affected_orders.get().value()
       << ",last_fragment=\"" << sbe_ilink3::last_fragment::to_string(msg.fields.last_fragment.get().value()) << '"'
       << ",mass_action_reject_reason=" << (msg.fields.mass_action_reject_reason.get().has_value() ? std::to_string(msg.fields.mass_action_reject_reason.get().value()) : std::string_view("null"))
       << ",market_segment_id=" << (msg.fields.market_segment_id.get().has_value() ? std::to_string(msg.fields.market_segment_id.get().value()) : std::string_view("null"))
       << ",mass_cancel_request_type=\"" << sbe_ilink3::mass_cancel_request_type::to_string(msg.fields.mass_cancel_request_type.get().value()) << '"'
       << ",side_optional=\"" << sbe_ilink3::side_optional::to_string(msg.fields.side_optional.get().value()) << '"'
       << ",mass_action_ord_typ=\"" << sbe_ilink3::mass_action_ord_typ::to_string(msg.fields.mass_action_ord_typ.get().value()) << '"'
       << ",mass_cancel_tif=\"" << sbe_ilink3::mass_cancel_tif::to_string(msg.fields.mass_cancel_tif.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",orig_order_user=\"" << msg.fields.orig_order_user.get_trimmed().value() << '"'
       << ",cancel_text=\"" << msg.fields.cancel_text.get_trimmed().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quote_cancel_ack& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",request_time=" << msg.fields.request_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",cancelled_symbol=\"" << msg.fields.cancelled_symbol.get_trimmed().value() << '"'
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",quote_reject_reason=" << (msg.fields.quote_reject_reason.get().has_value() ? std::to_string(msg.fields.quote_reject_reason.get().value()) : std::string_view("null"))
       << ",delay_duration=" << (msg.fields.delay_duration.get().has_value() ? std::to_string(msg.fields.delay_duration.get().value()) : std::string_view("null"))
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",quote_cxl_status=\"" << sbe_ilink3::quote_cxl_status::to_string(msg.fields.quote_cxl_status.get().value()) << '"'
       << ",no_processed_entries_32=" << msg.fields.no_processed_entries_32.get().value()
       << ",mm_protection_reset=\"" << sbe_ilink3::mm_protection_reset::to_string(msg.fields.mm_protection_reset.get().value()) << '"'
       << ",unsolicited_cancel_type=\"" << (msg.fields.unsolicited_cancel_type.get().has_value() ? std::string_view(1, msg.fields.unsolicited_cancel_type.get().value()) : std::string_view("null")) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",tot_no_quote_entries_optional=" << (msg.fields.tot_no_quote_entries_optional.get().has_value() ? std::to_string(msg.fields.tot_no_quote_entries_optional.get().value()) : std::string_view("null"))
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",orig_order_user=\"" << msg.fields.orig_order_user.get_trimmed().value() << '"'
       << ",cancel_text=\"" << msg.fields.cancel_text.get_trimmed().value() << '"'
       << ",quote_entry_open=\"" << sbe_ilink3::quote_entry_open::to_string(msg.fields.quote_entry_open.get().value()) << '"'
       << ",cxl_linked_sessions=\"" << sbe_ilink3::cxl_linked_sessions::to_string(msg.fields.cxl_linked_sessions.get().value()) << '"'
       << ",quote_entry_control=\"" << sbe_ilink3::quote_entry_control::to_string(msg.fields.quote_entry_control.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mass_quote_response& msg) {
    os << "seq_num=" << msg.fields.seq_num.get().value()
       << ",uuid=" << msg.fields.uuid.get().value()
       << ",text=\"" << msg.fields.text.get_trimmed().value() << '"'
       << ",sender_id=\"" << msg.fields.sender_id.get_trimmed().value() << '"'
       << ",party_details_list_req_id=" << msg.fields.party_details_list_req_id.get().value()
       << ",transact_time=" << msg.fields.transact_time.get().value()
       << ",sending_time_epoch=" << msg.fields.sending_time_epoch.get().value()
       << ",quote_req_id_optional=" << (msg.fields.quote_req_id_optional.get().has_value() ? std::to_string(msg.fields.quote_req_id_optional.get().value()) : std::string_view("null"))
       << ",location=\"" << msg.fields.location.get_trimmed().value() << '"'
       << ",quote_id=" << msg.fields.quote_id.get().value()
       << ",quote_reject_reason=" << (msg.fields.quote_reject_reason.get().has_value() ? std::to_string(msg.fields.quote_reject_reason.get().value()) : std::string_view("null"))
       << ",quote_ack_status=\"" << sbe_ilink3::quote_ack_status::to_string(msg.fields.quote_ack_status.get().value()) << '"'
       << ",manual_order_indicator=\"" << sbe_ilink3::manual_order_indicator::to_string(msg.fields.manual_order_indicator.get().value()) << '"'
       << ",no_processed_entries=" << static_cast<unsigned>(msg.fields.no_processed_entries.get().value())
       << ",mm_protection_reset=\"" << sbe_ilink3::mm_protection_reset::to_string(msg.fields.mm_protection_reset.get().value()) << '"'
       << ",split_msg=\"" << sbe_ilink3::split_msg::to_string(msg.fields.split_msg.get().value()) << '"'
       << ",liquidity_flag=\"" << sbe_ilink3::liquidity_flag::to_string(msg.fields.liquidity_flag.get().value()) << '"'
       << ",short_sale_type=\"" << sbe_ilink3::short_sale_type::to_string(msg.fields.short_sale_type.get().value()) << '"'
       << ",tot_no_quote_entries_optional=" << (msg.fields.tot_no_quote_entries_optional.get().has_value() ? std::to_string(msg.fields.tot_no_quote_entries_optional.get().value()) : std::string_view("null"))
       << ",poss_retrans_flag=\"" << sbe_ilink3::poss_retrans_flag::to_string(msg.fields.poss_retrans_flag.get().value()) << '"'
       << ",delay_to_time=" << (msg.fields.delay_to_time.get().has_value() ? std::to_string(msg.fields.delay_to_time.get().value()) : std::string_view("null"))
       << ",quote_entry_open=\"" << sbe_ilink3::quote_entry_open::to_string(msg.fields.quote_entry_open.get().value()) << '"'
       << ",quote_entry_control=\"" << sbe_ilink3::quote_entry_control::to_string(msg.fields.quote_entry_control.get().value()) << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

}
