#pragma once

#include <ostream>
#include <string_view>

#include "SignOnRequestInMessage.hpp"
#include "SystemInformationInMessage.hpp"
#include "GatewayRouterRequestMessage.hpp"
#include "UpdateLocalDatabaseInMessage.hpp"
#include "DownloadRequestMessage.hpp"
#include "OrderEntryRequestMessage.hpp"
#include "OrderModificationRequestMessage.hpp"
#include "OrderCancellationRequestMessage.hpp"
#include "OrderConfirmationMessage.hpp"
#include "OrderModificationConfirmationMessage.hpp"
#include "OrderEntryMessage.hpp"
#include "PriceModificationMessage.hpp"
#include "TradeInquiryMessage.hpp"
#include "SpreadOrderEntryMessage.hpp"
#include "SecureBoxRegistrationRequestInMessage.hpp"
#include "BoxSignOnRequestInMessage.hpp"
#include "BoxSignOffMessage.hpp"
#include "BranchOrderValueLimitUpdateMessage.hpp"
#include "UserOrderValueLimitUpdateMessage.hpp"
#include "NormalOrderLimitUpdateMessage.hpp"
#include "ResetUserPasswordMessage.hpp"
#include "CollateralUserStatusChangeRequestMessage.hpp"
#include "UserTradeModifyCancelStatusChangeRequestMessage.hpp"
#include "UserAddressUnlockRequestMessage.hpp"
#include "SignOnRequestOutMessage.hpp"
#include "SignOffRequestOutMessage.hpp"
#include "SystemInformationOutMessage.hpp"
#include "GatewayRouterResponseMessage.hpp"
#include "UpdateLocalDatabaseHeaderMessage.hpp"
#include "UpdateLocalDatabaseTrailerMessage.hpp"
#include "UpdateLocalDatabaseDataMessage.hpp"
#include "MessageRecordMessage.hpp"
#include "TradeConfirmationMessage.hpp"
#include "UserOrderLimitUpdateMessage.hpp"
#include "DealerLimitUpdateMessage.hpp"
#include "SpreadOrderLimitUpdateMessage.hpp"
#include "ControlMessageToTraderMessage.hpp"
#include "MarketStatisticsReportMessage.hpp"
#include "EnhancedMarketStatisticsReportMessage.hpp"
#include "MarketIndexReportMessage.hpp"
#include "IndustryIndexReportMessage.hpp"
#include "SectorIndexReportMessage.hpp"
#include "SpreadBhavcopyMessage.hpp"
#include "GlobalIndicesMessage.hpp"
#include "GlobalContractsMessage.hpp"
#include "BoxSignOnRequestOutMessage.hpp"
#include "ContingencyBroadcastMessage.hpp"
#include "CollateralUserStatusChangeResponseMessage.hpp"
#include "UserTradeModifyCancelStatusChangeResponseMessage.hpp"
#include "UserAddressUnlockConfirmMessage.hpp"
#include "UserAddressUnlockApproveMessage.hpp"
#include "GiveupConfirmationMessage.hpp"
#include "OrderCancellationConfirmationMessage.hpp"
#include "../json/messages/gateway_router_response_message_json.hpp"
#include "../json/messages/update_local_database_data_message_json.hpp"
#include "../json/messages/message_record_message_json.hpp"
#include "../json/messages/market_statistics_report_message_json.hpp"
#include "../json/messages/spread_bhavcopy_message_json.hpp"
#include "../json/messages/order_cancellation_confirmation_message_json.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

inline std::ostream& operator<<(std::ostream& os, const message_header& value) {
    os << "transaction_code=\"" << nnf_orderentry::transaction_code::to_string(value.transaction_code.get().value()) << '"'
       << ",log_time=" << value.log_time.get().value()
       << ",alpha_char=\"" << value.alpha_char.get_trimmed().value() << '"'
       << ",trader_id=" << value.trader_id.get().value()
       << ",error_code=\"" << nnf_orderentry::error_code::to_string(value.error_code.get().value()) << '"'
       << ",header_timestamp=" << value.header_timestamp.get().value()
       << ",time_stamp_1=\"" << value.time_stamp_1.get_trimmed().value() << '"'
       << ",time_stamp_2=\"" << value.time_stamp_2.get_trimmed().value() << '"'
       << ",message_length=" << value.message_length.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sign_on_request_in_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       << ",password=\"" << msg.fields.password.get_trimmed().value() << '"'
       << ",new_password=\"" << msg.fields.new_password.get_trimmed().value() << '"'
       << ",trader_name=\"" << msg.fields.trader_name.get_trimmed().value() << '"'
       << ",last_password_change_date=" << msg.fields.last_password_change_date.get().value()
       << ",broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",branch_id=" << msg.fields.branch_id.get().value()
       << ",version_number=" << msg.fields.version_number.get().value()
       << ",batch_2_start_time=" << msg.fields.batch_2_start_time.get().value()
       << ",host_switch_context=\"" << msg.fields.host_switch_context.get().value() << '"'
       << ",colour=\"" << msg.fields.colour.get_trimmed().value() << '"'
       << ",user_type=" << msg.fields.user_type.get().value()
       << ",sequence_number=" << msg.fields.sequence_number.get().value()
       << ",ws_class_name=\"" << msg.fields.ws_class_name.get_trimmed().value() << '"'
       << ",broker_status=\"" << msg.fields.broker_status.get().value() << '"'
       << ",show_index=\"" << msg.fields.show_index.get().value() << '"'
       << ",st_broker_eligibility_per_mkt=" << static_cast<unsigned>(msg.fields.st_broker_eligibility_per_mkt.get().value())
       << ",member_type=" << msg.fields.member_type.get().value()
       << ",clearing_status=\"" << msg.fields.clearing_status.get().value() << '"'
       << ",broker_name=\"" << msg.fields.broker_name.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const system_information_in_message& msg) {
    os << "last_update_portfolio_t_ime=" << msg.fields.last_update_portfolio_t_ime.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const gateway_router_request_message& msg) {
    os << "box_id=" << msg.fields.box_id.get().value()
       << ",broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",filler=\"" << msg.fields.filler.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const update_local_database_in_message& msg) {
    os << "last_update_security_time=" << msg.fields.last_update_security_time.get().value()
       << ",last_update_participant_time=" << msg.fields.last_update_participant_time.get().value()
       << ",last_update_instrument_time=" << msg.fields.last_update_instrument_time.get().value()
       << ",last_update_index_time=" << msg.fields.last_update_index_time.get().value()
       << ",request_for_open_orders=\"" << msg.fields.request_for_open_orders.get().value() << '"'
       << ",st_market_status.market_status_normal=\"" << nnf_orderentry::market_status_normal::to_string(msg.fields.st_market_status.market_status_normal.get().value()) << '"'
       << ",st_market_status.market_status_oddlot=\"" << nnf_orderentry::market_status_oddlot::to_string(msg.fields.st_market_status.market_status_oddlot.get().value()) << '"'
       << ",st_market_status.market_status_spot=\"" << nnf_orderentry::market_status_spot::to_string(msg.fields.st_market_status.market_status_spot.get().value()) << '"'
       << ",st_market_status.market_status_auction=\"" << nnf_orderentry::market_status_auction::to_string(msg.fields.st_market_status.market_status_auction.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_normal=\"" << nnf_orderentry::ex_market_status_normal::to_string(msg.fields.st_ex_market_status.ex_market_status_normal.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_oddlot=\"" << nnf_orderentry::ex_market_status_oddlot::to_string(msg.fields.st_ex_market_status.ex_market_status_oddlot.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_spot=\"" << nnf_orderentry::ex_market_status_spot::to_string(msg.fields.st_ex_market_status.ex_market_status_spot.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_auction=\"" << nnf_orderentry::ex_market_status_auction::to_string(msg.fields.st_ex_market_status.ex_market_status_auction.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_normal=\"" << nnf_orderentry::pl_market_status_normal::to_string(msg.fields.st_pl_market_status.pl_market_status_normal.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_oddlot=\"" << nnf_orderentry::pl_market_status_oddlot::to_string(msg.fields.st_pl_market_status.pl_market_status_oddlot.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_spot=\"" << nnf_orderentry::pl_market_status_spot::to_string(msg.fields.st_pl_market_status.pl_market_status_spot.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_auction=\"" << nnf_orderentry::pl_market_status_auction::to_string(msg.fields.st_pl_market_status.pl_market_status_auction.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const download_request_message& msg) {
    os << "sequence_number=" << msg.fields.sequence_number.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_entry_request_message& msg) {
    os << "order_entry_body.participant_type=\"" << nnf_orderentry::participant_type::to_string(msg.fields.order_entry_body.participant_type.get().value()) << '"'
       << ",order_entry_body.competitor_period=" << msg.fields.order_entry_body.competitor_period.get().value()
       << ",order_entry_body.solicitor_period=" << msg.fields.order_entry_body.solicitor_period.get().value()
       << ",order_entry_body.modified_cancelled_by=\"" << nnf_orderentry::modified_cancelled_by::to_string(msg.fields.order_entry_body.modified_cancelled_by.get().value()) << '"'
       << ",order_entry_body.reason_code=\"" << nnf_orderentry::reason_code::to_string(msg.fields.order_entry_body.reason_code.get().value()) << '"'
       << ",order_entry_body.token_no=" << msg.fields.order_entry_body.token_no.get().value()
       << ",order_entry_body.contract_desc.instrument_name=\"" << msg.fields.order_entry_body.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.symbol=\"" << msg.fields.order_entry_body.contract_desc.symbol.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.expiry_date=" << msg.fields.order_entry_body.contract_desc.expiry_date.get().value()
       << ",order_entry_body.contract_desc.strike_price=" << msg.fields.order_entry_body.contract_desc.strike_price.get().value()
       << ",order_entry_body.contract_desc.option_type=\"" << msg.fields.order_entry_body.contract_desc.option_type.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.ca_level=" << msg.fields.order_entry_body.contract_desc.ca_level.get().value()
       << ",order_entry_body.counter_party_broker_id=\"" << msg.fields.order_entry_body.counter_party_broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.closeout_flag=\"" << msg.fields.order_entry_body.closeout_flag.get().value() << '"'
       << ",order_entry_body.order_type=" << msg.fields.order_entry_body.order_type.get().value()
       << ",order_entry_body.order_number=" << msg.fields.order_entry_body.order_number.get().value()
       << ",order_entry_body.account_number=\"" << msg.fields.order_entry_body.account_number.get_trimmed().value() << '"'
       << ",order_entry_body.book_type=\"" << nnf_orderentry::book_type::to_string(msg.fields.order_entry_body.book_type.get().value()) << '"'
       << ",order_entry_body.buy_sell_indicator=\"" << nnf_orderentry::buy_sell_indicator::to_string(msg.fields.order_entry_body.buy_sell_indicator.get().value()) << '"'
       << ",order_entry_body.disclosed_volume=" << msg.fields.order_entry_body.disclosed_volume.get().value()
       << ",order_entry_body.disclosed_volume_remaining=" << msg.fields.order_entry_body.disclosed_volume_remaining.get().value()
       << ",order_entry_body.total_volume_remaining=" << msg.fields.order_entry_body.total_volume_remaining.get().value()
       << ",order_entry_body.volume=" << msg.fields.order_entry_body.volume.get().value()
       << ",order_entry_body.volume_filled_today=" << msg.fields.order_entry_body.volume_filled_today.get().value()
       << ",order_entry_body.price=" << msg.fields.order_entry_body.price.get().value()
       << ",order_entry_body.trigger_price=" << msg.fields.order_entry_body.trigger_price.get().value()
       << ",order_entry_body.good_till_date=" << msg.fields.order_entry_body.good_till_date.get().value()
       << ",order_entry_body.entry_date_time=" << msg.fields.order_entry_body.entry_date_time.get().value()
       << ",order_entry_body.minimum_fill_aon_volume=" << msg.fields.order_entry_body.minimum_fill_aon_volume.get().value()
       << ",order_entry_body.last_modified=" << msg.fields.order_entry_body.last_modified.get().value()
       << ",order_entry_body.st_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.st_order_flags.get().value())
       << ",order_entry_body.branch_id=" << msg.fields.order_entry_body.branch_id.get().value()
       << ",order_entry_body.trader_id=" << msg.fields.order_entry_body.trader_id.get().value()
       << ",order_entry_body.broker_id=\"" << msg.fields.order_entry_body.broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.c_ord_filler=\"" << msg.fields.order_entry_body.c_ord_filler.get_trimmed().value() << '"'
       << ",order_entry_body.open_close=\"" << msg.fields.order_entry_body.open_close.get().value() << '"'
       << ",order_entry_body.settlor=\"" << msg.fields.order_entry_body.settlor.get_trimmed().value() << '"'
       << ",order_entry_body.pro_client_indicator=" << msg.fields.order_entry_body.pro_client_indicator.get().value()
       << ",order_entry_body.settlement_period=" << msg.fields.order_entry_body.settlement_period.get().value()
       << ",order_entry_body.additional_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.additional_order_flags.get().value())
       << ",order_entry_body.filler_116=\"" << msg.fields.order_entry_body.filler_116.get_trimmed().value() << '"'
       << ",order_entry_body.filler_17=\"" << msg.fields.order_entry_body.filler_17.get().value() << '"'
       << ",order_entry_body.filler_18=\"" << msg.fields.order_entry_body.filler_18.get().value() << '"'
       << ",order_entry_body.nnf_field=" << msg.fields.order_entry_body.nnf_field.get().value()
       << ",order_entry_body.mkt_replay=" << msg.fields.order_entry_body.mkt_replay.get().value()
       << ",order_entry_body.pan=\"" << msg.fields.order_entry_body.pan.get_trimmed().value() << '"'
       << ",order_entry_body.algo_id=" << msg.fields.order_entry_body.algo_id.get().value()
       << ",order_entry_body.last_activity_reference=" << msg.fields.order_entry_body.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_modification_request_message& msg) {
    os << "order_entry_body.participant_type=\"" << nnf_orderentry::participant_type::to_string(msg.fields.order_entry_body.participant_type.get().value()) << '"'
       << ",order_entry_body.competitor_period=" << msg.fields.order_entry_body.competitor_period.get().value()
       << ",order_entry_body.solicitor_period=" << msg.fields.order_entry_body.solicitor_period.get().value()
       << ",order_entry_body.modified_cancelled_by=\"" << nnf_orderentry::modified_cancelled_by::to_string(msg.fields.order_entry_body.modified_cancelled_by.get().value()) << '"'
       << ",order_entry_body.reason_code=\"" << nnf_orderentry::reason_code::to_string(msg.fields.order_entry_body.reason_code.get().value()) << '"'
       << ",order_entry_body.token_no=" << msg.fields.order_entry_body.token_no.get().value()
       << ",order_entry_body.contract_desc.instrument_name=\"" << msg.fields.order_entry_body.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.symbol=\"" << msg.fields.order_entry_body.contract_desc.symbol.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.expiry_date=" << msg.fields.order_entry_body.contract_desc.expiry_date.get().value()
       << ",order_entry_body.contract_desc.strike_price=" << msg.fields.order_entry_body.contract_desc.strike_price.get().value()
       << ",order_entry_body.contract_desc.option_type=\"" << msg.fields.order_entry_body.contract_desc.option_type.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.ca_level=" << msg.fields.order_entry_body.contract_desc.ca_level.get().value()
       << ",order_entry_body.counter_party_broker_id=\"" << msg.fields.order_entry_body.counter_party_broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.closeout_flag=\"" << msg.fields.order_entry_body.closeout_flag.get().value() << '"'
       << ",order_entry_body.order_type=" << msg.fields.order_entry_body.order_type.get().value()
       << ",order_entry_body.order_number=" << msg.fields.order_entry_body.order_number.get().value()
       << ",order_entry_body.account_number=\"" << msg.fields.order_entry_body.account_number.get_trimmed().value() << '"'
       << ",order_entry_body.book_type=\"" << nnf_orderentry::book_type::to_string(msg.fields.order_entry_body.book_type.get().value()) << '"'
       << ",order_entry_body.buy_sell_indicator=\"" << nnf_orderentry::buy_sell_indicator::to_string(msg.fields.order_entry_body.buy_sell_indicator.get().value()) << '"'
       << ",order_entry_body.disclosed_volume=" << msg.fields.order_entry_body.disclosed_volume.get().value()
       << ",order_entry_body.disclosed_volume_remaining=" << msg.fields.order_entry_body.disclosed_volume_remaining.get().value()
       << ",order_entry_body.total_volume_remaining=" << msg.fields.order_entry_body.total_volume_remaining.get().value()
       << ",order_entry_body.volume=" << msg.fields.order_entry_body.volume.get().value()
       << ",order_entry_body.volume_filled_today=" << msg.fields.order_entry_body.volume_filled_today.get().value()
       << ",order_entry_body.price=" << msg.fields.order_entry_body.price.get().value()
       << ",order_entry_body.trigger_price=" << msg.fields.order_entry_body.trigger_price.get().value()
       << ",order_entry_body.good_till_date=" << msg.fields.order_entry_body.good_till_date.get().value()
       << ",order_entry_body.entry_date_time=" << msg.fields.order_entry_body.entry_date_time.get().value()
       << ",order_entry_body.minimum_fill_aon_volume=" << msg.fields.order_entry_body.minimum_fill_aon_volume.get().value()
       << ",order_entry_body.last_modified=" << msg.fields.order_entry_body.last_modified.get().value()
       << ",order_entry_body.st_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.st_order_flags.get().value())
       << ",order_entry_body.branch_id=" << msg.fields.order_entry_body.branch_id.get().value()
       << ",order_entry_body.trader_id=" << msg.fields.order_entry_body.trader_id.get().value()
       << ",order_entry_body.broker_id=\"" << msg.fields.order_entry_body.broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.c_ord_filler=\"" << msg.fields.order_entry_body.c_ord_filler.get_trimmed().value() << '"'
       << ",order_entry_body.open_close=\"" << msg.fields.order_entry_body.open_close.get().value() << '"'
       << ",order_entry_body.settlor=\"" << msg.fields.order_entry_body.settlor.get_trimmed().value() << '"'
       << ",order_entry_body.pro_client_indicator=" << msg.fields.order_entry_body.pro_client_indicator.get().value()
       << ",order_entry_body.settlement_period=" << msg.fields.order_entry_body.settlement_period.get().value()
       << ",order_entry_body.additional_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.additional_order_flags.get().value())
       << ",order_entry_body.filler_116=\"" << msg.fields.order_entry_body.filler_116.get_trimmed().value() << '"'
       << ",order_entry_body.filler_17=\"" << msg.fields.order_entry_body.filler_17.get().value() << '"'
       << ",order_entry_body.filler_18=\"" << msg.fields.order_entry_body.filler_18.get().value() << '"'
       << ",order_entry_body.nnf_field=" << msg.fields.order_entry_body.nnf_field.get().value()
       << ",order_entry_body.mkt_replay=" << msg.fields.order_entry_body.mkt_replay.get().value()
       << ",order_entry_body.pan=\"" << msg.fields.order_entry_body.pan.get_trimmed().value() << '"'
       << ",order_entry_body.algo_id=" << msg.fields.order_entry_body.algo_id.get().value()
       << ",order_entry_body.last_activity_reference=" << msg.fields.order_entry_body.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_cancellation_request_message& msg) {
    os << "order_entry_body.participant_type=\"" << nnf_orderentry::participant_type::to_string(msg.fields.order_entry_body.participant_type.get().value()) << '"'
       << ",order_entry_body.competitor_period=" << msg.fields.order_entry_body.competitor_period.get().value()
       << ",order_entry_body.solicitor_period=" << msg.fields.order_entry_body.solicitor_period.get().value()
       << ",order_entry_body.modified_cancelled_by=\"" << nnf_orderentry::modified_cancelled_by::to_string(msg.fields.order_entry_body.modified_cancelled_by.get().value()) << '"'
       << ",order_entry_body.reason_code=\"" << nnf_orderentry::reason_code::to_string(msg.fields.order_entry_body.reason_code.get().value()) << '"'
       << ",order_entry_body.token_no=" << msg.fields.order_entry_body.token_no.get().value()
       << ",order_entry_body.contract_desc.instrument_name=\"" << msg.fields.order_entry_body.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.symbol=\"" << msg.fields.order_entry_body.contract_desc.symbol.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.expiry_date=" << msg.fields.order_entry_body.contract_desc.expiry_date.get().value()
       << ",order_entry_body.contract_desc.strike_price=" << msg.fields.order_entry_body.contract_desc.strike_price.get().value()
       << ",order_entry_body.contract_desc.option_type=\"" << msg.fields.order_entry_body.contract_desc.option_type.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.ca_level=" << msg.fields.order_entry_body.contract_desc.ca_level.get().value()
       << ",order_entry_body.counter_party_broker_id=\"" << msg.fields.order_entry_body.counter_party_broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.closeout_flag=\"" << msg.fields.order_entry_body.closeout_flag.get().value() << '"'
       << ",order_entry_body.order_type=" << msg.fields.order_entry_body.order_type.get().value()
       << ",order_entry_body.order_number=" << msg.fields.order_entry_body.order_number.get().value()
       << ",order_entry_body.account_number=\"" << msg.fields.order_entry_body.account_number.get_trimmed().value() << '"'
       << ",order_entry_body.book_type=\"" << nnf_orderentry::book_type::to_string(msg.fields.order_entry_body.book_type.get().value()) << '"'
       << ",order_entry_body.buy_sell_indicator=\"" << nnf_orderentry::buy_sell_indicator::to_string(msg.fields.order_entry_body.buy_sell_indicator.get().value()) << '"'
       << ",order_entry_body.disclosed_volume=" << msg.fields.order_entry_body.disclosed_volume.get().value()
       << ",order_entry_body.disclosed_volume_remaining=" << msg.fields.order_entry_body.disclosed_volume_remaining.get().value()
       << ",order_entry_body.total_volume_remaining=" << msg.fields.order_entry_body.total_volume_remaining.get().value()
       << ",order_entry_body.volume=" << msg.fields.order_entry_body.volume.get().value()
       << ",order_entry_body.volume_filled_today=" << msg.fields.order_entry_body.volume_filled_today.get().value()
       << ",order_entry_body.price=" << msg.fields.order_entry_body.price.get().value()
       << ",order_entry_body.trigger_price=" << msg.fields.order_entry_body.trigger_price.get().value()
       << ",order_entry_body.good_till_date=" << msg.fields.order_entry_body.good_till_date.get().value()
       << ",order_entry_body.entry_date_time=" << msg.fields.order_entry_body.entry_date_time.get().value()
       << ",order_entry_body.minimum_fill_aon_volume=" << msg.fields.order_entry_body.minimum_fill_aon_volume.get().value()
       << ",order_entry_body.last_modified=" << msg.fields.order_entry_body.last_modified.get().value()
       << ",order_entry_body.st_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.st_order_flags.get().value())
       << ",order_entry_body.branch_id=" << msg.fields.order_entry_body.branch_id.get().value()
       << ",order_entry_body.trader_id=" << msg.fields.order_entry_body.trader_id.get().value()
       << ",order_entry_body.broker_id=\"" << msg.fields.order_entry_body.broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.c_ord_filler=\"" << msg.fields.order_entry_body.c_ord_filler.get_trimmed().value() << '"'
       << ",order_entry_body.open_close=\"" << msg.fields.order_entry_body.open_close.get().value() << '"'
       << ",order_entry_body.settlor=\"" << msg.fields.order_entry_body.settlor.get_trimmed().value() << '"'
       << ",order_entry_body.pro_client_indicator=" << msg.fields.order_entry_body.pro_client_indicator.get().value()
       << ",order_entry_body.settlement_period=" << msg.fields.order_entry_body.settlement_period.get().value()
       << ",order_entry_body.additional_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.additional_order_flags.get().value())
       << ",order_entry_body.filler_116=\"" << msg.fields.order_entry_body.filler_116.get_trimmed().value() << '"'
       << ",order_entry_body.filler_17=\"" << msg.fields.order_entry_body.filler_17.get().value() << '"'
       << ",order_entry_body.filler_18=\"" << msg.fields.order_entry_body.filler_18.get().value() << '"'
       << ",order_entry_body.nnf_field=" << msg.fields.order_entry_body.nnf_field.get().value()
       << ",order_entry_body.mkt_replay=" << msg.fields.order_entry_body.mkt_replay.get().value()
       << ",order_entry_body.pan=\"" << msg.fields.order_entry_body.pan.get_trimmed().value() << '"'
       << ",order_entry_body.algo_id=" << msg.fields.order_entry_body.algo_id.get().value()
       << ",order_entry_body.last_activity_reference=" << msg.fields.order_entry_body.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_confirmation_message& msg) {
    os << "order_entry_body.participant_type=\"" << nnf_orderentry::participant_type::to_string(msg.fields.order_entry_body.participant_type.get().value()) << '"'
       << ",order_entry_body.competitor_period=" << msg.fields.order_entry_body.competitor_period.get().value()
       << ",order_entry_body.solicitor_period=" << msg.fields.order_entry_body.solicitor_period.get().value()
       << ",order_entry_body.modified_cancelled_by=\"" << nnf_orderentry::modified_cancelled_by::to_string(msg.fields.order_entry_body.modified_cancelled_by.get().value()) << '"'
       << ",order_entry_body.reason_code=\"" << nnf_orderentry::reason_code::to_string(msg.fields.order_entry_body.reason_code.get().value()) << '"'
       << ",order_entry_body.token_no=" << msg.fields.order_entry_body.token_no.get().value()
       << ",order_entry_body.contract_desc.instrument_name=\"" << msg.fields.order_entry_body.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.symbol=\"" << msg.fields.order_entry_body.contract_desc.symbol.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.expiry_date=" << msg.fields.order_entry_body.contract_desc.expiry_date.get().value()
       << ",order_entry_body.contract_desc.strike_price=" << msg.fields.order_entry_body.contract_desc.strike_price.get().value()
       << ",order_entry_body.contract_desc.option_type=\"" << msg.fields.order_entry_body.contract_desc.option_type.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.ca_level=" << msg.fields.order_entry_body.contract_desc.ca_level.get().value()
       << ",order_entry_body.counter_party_broker_id=\"" << msg.fields.order_entry_body.counter_party_broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.closeout_flag=\"" << msg.fields.order_entry_body.closeout_flag.get().value() << '"'
       << ",order_entry_body.order_type=" << msg.fields.order_entry_body.order_type.get().value()
       << ",order_entry_body.order_number=" << msg.fields.order_entry_body.order_number.get().value()
       << ",order_entry_body.account_number=\"" << msg.fields.order_entry_body.account_number.get_trimmed().value() << '"'
       << ",order_entry_body.book_type=\"" << nnf_orderentry::book_type::to_string(msg.fields.order_entry_body.book_type.get().value()) << '"'
       << ",order_entry_body.buy_sell_indicator=\"" << nnf_orderentry::buy_sell_indicator::to_string(msg.fields.order_entry_body.buy_sell_indicator.get().value()) << '"'
       << ",order_entry_body.disclosed_volume=" << msg.fields.order_entry_body.disclosed_volume.get().value()
       << ",order_entry_body.disclosed_volume_remaining=" << msg.fields.order_entry_body.disclosed_volume_remaining.get().value()
       << ",order_entry_body.total_volume_remaining=" << msg.fields.order_entry_body.total_volume_remaining.get().value()
       << ",order_entry_body.volume=" << msg.fields.order_entry_body.volume.get().value()
       << ",order_entry_body.volume_filled_today=" << msg.fields.order_entry_body.volume_filled_today.get().value()
       << ",order_entry_body.price=" << msg.fields.order_entry_body.price.get().value()
       << ",order_entry_body.trigger_price=" << msg.fields.order_entry_body.trigger_price.get().value()
       << ",order_entry_body.good_till_date=" << msg.fields.order_entry_body.good_till_date.get().value()
       << ",order_entry_body.entry_date_time=" << msg.fields.order_entry_body.entry_date_time.get().value()
       << ",order_entry_body.minimum_fill_aon_volume=" << msg.fields.order_entry_body.minimum_fill_aon_volume.get().value()
       << ",order_entry_body.last_modified=" << msg.fields.order_entry_body.last_modified.get().value()
       << ",order_entry_body.st_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.st_order_flags.get().value())
       << ",order_entry_body.branch_id=" << msg.fields.order_entry_body.branch_id.get().value()
       << ",order_entry_body.trader_id=" << msg.fields.order_entry_body.trader_id.get().value()
       << ",order_entry_body.broker_id=\"" << msg.fields.order_entry_body.broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.c_ord_filler=\"" << msg.fields.order_entry_body.c_ord_filler.get_trimmed().value() << '"'
       << ",order_entry_body.open_close=\"" << msg.fields.order_entry_body.open_close.get().value() << '"'
       << ",order_entry_body.settlor=\"" << msg.fields.order_entry_body.settlor.get_trimmed().value() << '"'
       << ",order_entry_body.pro_client_indicator=" << msg.fields.order_entry_body.pro_client_indicator.get().value()
       << ",order_entry_body.settlement_period=" << msg.fields.order_entry_body.settlement_period.get().value()
       << ",order_entry_body.additional_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.additional_order_flags.get().value())
       << ",order_entry_body.filler_116=\"" << msg.fields.order_entry_body.filler_116.get_trimmed().value() << '"'
       << ",order_entry_body.filler_17=\"" << msg.fields.order_entry_body.filler_17.get().value() << '"'
       << ",order_entry_body.filler_18=\"" << msg.fields.order_entry_body.filler_18.get().value() << '"'
       << ",order_entry_body.nnf_field=" << msg.fields.order_entry_body.nnf_field.get().value()
       << ",order_entry_body.mkt_replay=" << msg.fields.order_entry_body.mkt_replay.get().value()
       << ",order_entry_body.pan=\"" << msg.fields.order_entry_body.pan.get_trimmed().value() << '"'
       << ",order_entry_body.algo_id=" << msg.fields.order_entry_body.algo_id.get().value()
       << ",order_entry_body.last_activity_reference=" << msg.fields.order_entry_body.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_modification_confirmation_message& msg) {
    os << "order_entry_body.participant_type=\"" << nnf_orderentry::participant_type::to_string(msg.fields.order_entry_body.participant_type.get().value()) << '"'
       << ",order_entry_body.competitor_period=" << msg.fields.order_entry_body.competitor_period.get().value()
       << ",order_entry_body.solicitor_period=" << msg.fields.order_entry_body.solicitor_period.get().value()
       << ",order_entry_body.modified_cancelled_by=\"" << nnf_orderentry::modified_cancelled_by::to_string(msg.fields.order_entry_body.modified_cancelled_by.get().value()) << '"'
       << ",order_entry_body.reason_code=\"" << nnf_orderentry::reason_code::to_string(msg.fields.order_entry_body.reason_code.get().value()) << '"'
       << ",order_entry_body.token_no=" << msg.fields.order_entry_body.token_no.get().value()
       << ",order_entry_body.contract_desc.instrument_name=\"" << msg.fields.order_entry_body.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.symbol=\"" << msg.fields.order_entry_body.contract_desc.symbol.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.expiry_date=" << msg.fields.order_entry_body.contract_desc.expiry_date.get().value()
       << ",order_entry_body.contract_desc.strike_price=" << msg.fields.order_entry_body.contract_desc.strike_price.get().value()
       << ",order_entry_body.contract_desc.option_type=\"" << msg.fields.order_entry_body.contract_desc.option_type.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.ca_level=" << msg.fields.order_entry_body.contract_desc.ca_level.get().value()
       << ",order_entry_body.counter_party_broker_id=\"" << msg.fields.order_entry_body.counter_party_broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.closeout_flag=\"" << msg.fields.order_entry_body.closeout_flag.get().value() << '"'
       << ",order_entry_body.order_type=" << msg.fields.order_entry_body.order_type.get().value()
       << ",order_entry_body.order_number=" << msg.fields.order_entry_body.order_number.get().value()
       << ",order_entry_body.account_number=\"" << msg.fields.order_entry_body.account_number.get_trimmed().value() << '"'
       << ",order_entry_body.book_type=\"" << nnf_orderentry::book_type::to_string(msg.fields.order_entry_body.book_type.get().value()) << '"'
       << ",order_entry_body.buy_sell_indicator=\"" << nnf_orderentry::buy_sell_indicator::to_string(msg.fields.order_entry_body.buy_sell_indicator.get().value()) << '"'
       << ",order_entry_body.disclosed_volume=" << msg.fields.order_entry_body.disclosed_volume.get().value()
       << ",order_entry_body.disclosed_volume_remaining=" << msg.fields.order_entry_body.disclosed_volume_remaining.get().value()
       << ",order_entry_body.total_volume_remaining=" << msg.fields.order_entry_body.total_volume_remaining.get().value()
       << ",order_entry_body.volume=" << msg.fields.order_entry_body.volume.get().value()
       << ",order_entry_body.volume_filled_today=" << msg.fields.order_entry_body.volume_filled_today.get().value()
       << ",order_entry_body.price=" << msg.fields.order_entry_body.price.get().value()
       << ",order_entry_body.trigger_price=" << msg.fields.order_entry_body.trigger_price.get().value()
       << ",order_entry_body.good_till_date=" << msg.fields.order_entry_body.good_till_date.get().value()
       << ",order_entry_body.entry_date_time=" << msg.fields.order_entry_body.entry_date_time.get().value()
       << ",order_entry_body.minimum_fill_aon_volume=" << msg.fields.order_entry_body.minimum_fill_aon_volume.get().value()
       << ",order_entry_body.last_modified=" << msg.fields.order_entry_body.last_modified.get().value()
       << ",order_entry_body.st_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.st_order_flags.get().value())
       << ",order_entry_body.branch_id=" << msg.fields.order_entry_body.branch_id.get().value()
       << ",order_entry_body.trader_id=" << msg.fields.order_entry_body.trader_id.get().value()
       << ",order_entry_body.broker_id=\"" << msg.fields.order_entry_body.broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.c_ord_filler=\"" << msg.fields.order_entry_body.c_ord_filler.get_trimmed().value() << '"'
       << ",order_entry_body.open_close=\"" << msg.fields.order_entry_body.open_close.get().value() << '"'
       << ",order_entry_body.settlor=\"" << msg.fields.order_entry_body.settlor.get_trimmed().value() << '"'
       << ",order_entry_body.pro_client_indicator=" << msg.fields.order_entry_body.pro_client_indicator.get().value()
       << ",order_entry_body.settlement_period=" << msg.fields.order_entry_body.settlement_period.get().value()
       << ",order_entry_body.additional_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.additional_order_flags.get().value())
       << ",order_entry_body.filler_116=\"" << msg.fields.order_entry_body.filler_116.get_trimmed().value() << '"'
       << ",order_entry_body.filler_17=\"" << msg.fields.order_entry_body.filler_17.get().value() << '"'
       << ",order_entry_body.filler_18=\"" << msg.fields.order_entry_body.filler_18.get().value() << '"'
       << ",order_entry_body.nnf_field=" << msg.fields.order_entry_body.nnf_field.get().value()
       << ",order_entry_body.mkt_replay=" << msg.fields.order_entry_body.mkt_replay.get().value()
       << ",order_entry_body.pan=\"" << msg.fields.order_entry_body.pan.get_trimmed().value() << '"'
       << ",order_entry_body.algo_id=" << msg.fields.order_entry_body.algo_id.get().value()
       << ",order_entry_body.last_activity_reference=" << msg.fields.order_entry_body.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_entry_message& msg) {
    os << "order_entry_body.participant_type=\"" << nnf_orderentry::participant_type::to_string(msg.fields.order_entry_body.participant_type.get().value()) << '"'
       << ",order_entry_body.competitor_period=" << msg.fields.order_entry_body.competitor_period.get().value()
       << ",order_entry_body.solicitor_period=" << msg.fields.order_entry_body.solicitor_period.get().value()
       << ",order_entry_body.modified_cancelled_by=\"" << nnf_orderentry::modified_cancelled_by::to_string(msg.fields.order_entry_body.modified_cancelled_by.get().value()) << '"'
       << ",order_entry_body.reason_code=\"" << nnf_orderentry::reason_code::to_string(msg.fields.order_entry_body.reason_code.get().value()) << '"'
       << ",order_entry_body.token_no=" << msg.fields.order_entry_body.token_no.get().value()
       << ",order_entry_body.contract_desc.instrument_name=\"" << msg.fields.order_entry_body.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.symbol=\"" << msg.fields.order_entry_body.contract_desc.symbol.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.expiry_date=" << msg.fields.order_entry_body.contract_desc.expiry_date.get().value()
       << ",order_entry_body.contract_desc.strike_price=" << msg.fields.order_entry_body.contract_desc.strike_price.get().value()
       << ",order_entry_body.contract_desc.option_type=\"" << msg.fields.order_entry_body.contract_desc.option_type.get_trimmed().value() << '"'
       << ",order_entry_body.contract_desc.ca_level=" << msg.fields.order_entry_body.contract_desc.ca_level.get().value()
       << ",order_entry_body.counter_party_broker_id=\"" << msg.fields.order_entry_body.counter_party_broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.closeout_flag=\"" << msg.fields.order_entry_body.closeout_flag.get().value() << '"'
       << ",order_entry_body.order_type=" << msg.fields.order_entry_body.order_type.get().value()
       << ",order_entry_body.order_number=" << msg.fields.order_entry_body.order_number.get().value()
       << ",order_entry_body.account_number=\"" << msg.fields.order_entry_body.account_number.get_trimmed().value() << '"'
       << ",order_entry_body.book_type=\"" << nnf_orderentry::book_type::to_string(msg.fields.order_entry_body.book_type.get().value()) << '"'
       << ",order_entry_body.buy_sell_indicator=\"" << nnf_orderentry::buy_sell_indicator::to_string(msg.fields.order_entry_body.buy_sell_indicator.get().value()) << '"'
       << ",order_entry_body.disclosed_volume=" << msg.fields.order_entry_body.disclosed_volume.get().value()
       << ",order_entry_body.disclosed_volume_remaining=" << msg.fields.order_entry_body.disclosed_volume_remaining.get().value()
       << ",order_entry_body.total_volume_remaining=" << msg.fields.order_entry_body.total_volume_remaining.get().value()
       << ",order_entry_body.volume=" << msg.fields.order_entry_body.volume.get().value()
       << ",order_entry_body.volume_filled_today=" << msg.fields.order_entry_body.volume_filled_today.get().value()
       << ",order_entry_body.price=" << msg.fields.order_entry_body.price.get().value()
       << ",order_entry_body.trigger_price=" << msg.fields.order_entry_body.trigger_price.get().value()
       << ",order_entry_body.good_till_date=" << msg.fields.order_entry_body.good_till_date.get().value()
       << ",order_entry_body.entry_date_time=" << msg.fields.order_entry_body.entry_date_time.get().value()
       << ",order_entry_body.minimum_fill_aon_volume=" << msg.fields.order_entry_body.minimum_fill_aon_volume.get().value()
       << ",order_entry_body.last_modified=" << msg.fields.order_entry_body.last_modified.get().value()
       << ",order_entry_body.st_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.st_order_flags.get().value())
       << ",order_entry_body.branch_id=" << msg.fields.order_entry_body.branch_id.get().value()
       << ",order_entry_body.trader_id=" << msg.fields.order_entry_body.trader_id.get().value()
       << ",order_entry_body.broker_id=\"" << msg.fields.order_entry_body.broker_id.get_trimmed().value() << '"'
       << ",order_entry_body.c_ord_filler=\"" << msg.fields.order_entry_body.c_ord_filler.get_trimmed().value() << '"'
       << ",order_entry_body.open_close=\"" << msg.fields.order_entry_body.open_close.get().value() << '"'
       << ",order_entry_body.settlor=\"" << msg.fields.order_entry_body.settlor.get_trimmed().value() << '"'
       << ",order_entry_body.pro_client_indicator=" << msg.fields.order_entry_body.pro_client_indicator.get().value()
       << ",order_entry_body.settlement_period=" << msg.fields.order_entry_body.settlement_period.get().value()
       << ",order_entry_body.additional_order_flags=" << static_cast<unsigned>(msg.fields.order_entry_body.additional_order_flags.get().value())
       << ",order_entry_body.filler_116=\"" << msg.fields.order_entry_body.filler_116.get_trimmed().value() << '"'
       << ",order_entry_body.filler_17=\"" << msg.fields.order_entry_body.filler_17.get().value() << '"'
       << ",order_entry_body.filler_18=\"" << msg.fields.order_entry_body.filler_18.get().value() << '"'
       << ",order_entry_body.nnf_field=" << msg.fields.order_entry_body.nnf_field.get().value()
       << ",order_entry_body.mkt_replay=" << msg.fields.order_entry_body.mkt_replay.get().value()
       << ",order_entry_body.pan=\"" << msg.fields.order_entry_body.pan.get_trimmed().value() << '"'
       << ",order_entry_body.algo_id=" << msg.fields.order_entry_body.algo_id.get().value()
       << ",order_entry_body.last_activity_reference=" << msg.fields.order_entry_body.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const price_modification_message& msg) {
    os << "token_no=" << msg.fields.token_no.get().value()
       << ",trader_id=" << msg.fields.trader_id.get().value()
       << ",order_number=" << msg.fields.order_number.get().value()
       << ",buy_sell=\"" << nnf_orderentry::buy_sell::to_string(msg.fields.buy_sell.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",volume=" << msg.fields.volume.get().value()
       << ",last_modified=" << msg.fields.last_modified.get().value()
       << ",reference=\"" << msg.fields.reference.get_trimmed().value() << '"'
       << ",last_activity_reference=" << msg.fields.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trade_inquiry_message& msg) {
    os << "token_no=" << msg.fields.token_no.get().value()
       << ",contract_desc.instrument_name=\"" << msg.fields.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",contract_desc.symbol=\"" << msg.fields.contract_desc.symbol.get_trimmed().value() << '"'
       << ",contract_desc.expiry_date=" << msg.fields.contract_desc.expiry_date.get().value()
       << ",contract_desc.strike_price=" << msg.fields.contract_desc.strike_price.get().value()
       << ",contract_desc.option_type=\"" << msg.fields.contract_desc.option_type.get_trimmed().value() << '"'
       << ",contract_desc.ca_level=" << msg.fields.contract_desc.ca_level.get().value()
       << ",fill_number=" << msg.fields.fill_number.get().value()
       << ",fill_quantity=" << msg.fields.fill_quantity.get().value()
       << ",fill_price=" << msg.fields.fill_price.get().value()
       << ",mkt_type=\"" << nnf_orderentry::mkt_type::to_string(msg.fields.mkt_type.get().value()) << '"'
       << ",buy_open_close=\"" << msg.fields.buy_open_close.get().value() << '"'
       << ",buy_broker_id=\"" << msg.fields.buy_broker_id.get_trimmed().value() << '"'
       << ",sell_broker_id=\"" << msg.fields.sell_broker_id.get_trimmed().value() << '"'
       << ",trader_id=" << msg.fields.trader_id.get().value()
       << ",requested_by=\"" << msg.fields.requested_by.get().value() << '"'
       << ",sell_open_close=\"" << msg.fields.sell_open_close.get().value() << '"'
       << ",buy_account_number=\"" << msg.fields.buy_account_number.get_trimmed().value() << '"'
       << ",sell_account_number=\"" << msg.fields.sell_account_number.get_trimmed().value() << '"'
       << ",buy_pan=\"" << msg.fields.buy_pan.get_trimmed().value() << '"'
       << ",sell_pan=\"" << msg.fields.sell_pan.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_order_entry_message& msg) {
    os << "spread_order_body.participant_type_1=\"" << msg.fields.spread_order_body.participant_type_1.get().value() << '"'
       << ",spread_order_body.filler_1=\"" << msg.fields.spread_order_body.filler_1.get().value() << '"'
       << ",spread_order_body.competitor_period_1=" << msg.fields.spread_order_body.competitor_period_1.get().value()
       << ",spread_order_body.solicitor_period_1=" << msg.fields.spread_order_body.solicitor_period_1.get().value()
       << ",spread_order_body.mod_cxl_by_1=\"" << msg.fields.spread_order_body.mod_cxl_by_1.get().value() << '"'
       << ",spread_order_body.filler_9=\"" << msg.fields.spread_order_body.filler_9.get().value() << '"'
       << ",spread_order_body.reason_code_1=" << msg.fields.spread_order_body.reason_code_1.get().value()
       << ",spread_order_body.start_alpha_1=\"" << msg.fields.spread_order_body.start_alpha_1.get_trimmed().value() << '"'
       << ",spread_order_body.end_alpha_1=\"" << msg.fields.spread_order_body.end_alpha_1.get_trimmed().value() << '"'
       << ",spread_order_body.token_1=" << msg.fields.spread_order_body.token_1.get().value()
       << ",spread_order_body.contract_desc.instrument_name=\"" << msg.fields.spread_order_body.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",spread_order_body.contract_desc.symbol=\"" << msg.fields.spread_order_body.contract_desc.symbol.get_trimmed().value() << '"'
       << ",spread_order_body.contract_desc.expiry_date=" << msg.fields.spread_order_body.contract_desc.expiry_date.get().value()
       << ",spread_order_body.contract_desc.strike_price=" << msg.fields.spread_order_body.contract_desc.strike_price.get().value()
       << ",spread_order_body.contract_desc.option_type=\"" << msg.fields.spread_order_body.contract_desc.option_type.get_trimmed().value() << '"'
       << ",spread_order_body.contract_desc.ca_level=" << msg.fields.spread_order_body.contract_desc.ca_level.get().value()
       << ",spread_order_body.op_broker_id_1=\"" << msg.fields.spread_order_body.op_broker_id_1.get_trimmed().value() << '"'
       << ",spread_order_body.fillerx_1=\"" << msg.fields.spread_order_body.fillerx_1.get().value() << '"'
       << ",spread_order_body.filler_options_1=\"" << msg.fields.spread_order_body.filler_options_1.get_trimmed().value() << '"'
       << ",spread_order_body.fillery_1=\"" << msg.fields.spread_order_body.fillery_1.get().value() << '"'
       << ",spread_order_body.order_type_1=" << msg.fields.spread_order_body.order_type_1.get().value()
       << ",spread_order_body.order_number_1=" << msg.fields.spread_order_body.order_number_1.get().value()
       << ",spread_order_body.account_number_1=\"" << msg.fields.spread_order_body.account_number_1.get_trimmed().value() << '"'
       << ",spread_order_body.book_type_1=\"" << nnf_orderentry::book_type_1::to_string(msg.fields.spread_order_body.book_type_1.get().value()) << '"'
       << ",spread_order_body.buy_sell_1=" << msg.fields.spread_order_body.buy_sell_1.get().value()
       << ",spread_order_body.disclosed_vol_1=" << msg.fields.spread_order_body.disclosed_vol_1.get().value()
       << ",spread_order_body.disclosed_vol_remaining_1=" << msg.fields.spread_order_body.disclosed_vol_remaining_1.get().value()
       << ",spread_order_body.total_vol_remaining_1=" << msg.fields.spread_order_body.total_vol_remaining_1.get().value()
       << ",spread_order_body.volume_1=" << msg.fields.spread_order_body.volume_1.get().value()
       << ",spread_order_body.volume_filled_today_1=" << msg.fields.spread_order_body.volume_filled_today_1.get().value()
       << ",spread_order_body.price_1=" << msg.fields.spread_order_body.price_1.get().value()
       << ",spread_order_body.trigger_price_1=" << msg.fields.spread_order_body.trigger_price_1.get().value()
       << ",spread_order_body.good_till_date_1=" << msg.fields.spread_order_body.good_till_date_1.get().value()
       << ",spread_order_body.entry_date_time_1=" << msg.fields.spread_order_body.entry_date_time_1.get().value()
       << ",spread_order_body.min_fill_aon_1=" << msg.fields.spread_order_body.min_fill_aon_1.get().value()
       << ",spread_order_body.last_modified_1=" << msg.fields.spread_order_body.last_modified_1.get().value()
       << ",spread_order_body.st_order_flags=" << static_cast<unsigned>(msg.fields.spread_order_body.st_order_flags.get().value())
       << ",spread_order_body.branch_id_1=" << msg.fields.spread_order_body.branch_id_1.get().value()
       << ",spread_order_body.trader_id_1=" << msg.fields.spread_order_body.trader_id_1.get().value()
       << ",spread_order_body.broker_id_1=\"" << msg.fields.spread_order_body.broker_id_1.get_trimmed().value() << '"'
       << ",spread_order_body.c_ord_filler=\"" << msg.fields.spread_order_body.c_ord_filler.get_trimmed().value() << '"'
       << ",spread_order_body.open_close_1=\"" << msg.fields.spread_order_body.open_close_1.get().value() << '"'
       << ",spread_order_body.settlor_1=\"" << msg.fields.spread_order_body.settlor_1.get_trimmed().value() << '"'
       << ",spread_order_body.pro_client_1=" << msg.fields.spread_order_body.pro_client_1.get().value()
       << ",spread_order_body.settlement_period_1=" << msg.fields.spread_order_body.settlement_period_1.get().value()
       << ",spread_order_body.additional_order_flags=" << static_cast<unsigned>(msg.fields.spread_order_body.additional_order_flags.get().value())
       << ",spread_order_body.filler_116=\"" << msg.fields.spread_order_body.filler_116.get_trimmed().value() << '"'
       << ",spread_order_body.filler_17=\"" << msg.fields.spread_order_body.filler_17.get().value() << '"'
       << ",spread_order_body.filler_18=\"" << msg.fields.spread_order_body.filler_18.get().value() << '"'
       << ",spread_order_body.nnf_field=" << msg.fields.spread_order_body.nnf_field.get().value()
       << ",spread_order_body.mkt_replay=" << msg.fields.spread_order_body.mkt_replay.get().value()
       << ",spread_order_body.pan=\"" << msg.fields.spread_order_body.pan.get_trimmed().value() << '"'
       << ",spread_order_body.algo_id=" << msg.fields.spread_order_body.algo_id.get().value()
       << ",spread_order_body.last_activity_reference=" << msg.fields.spread_order_body.last_activity_reference.get().value()
       << ",spread_order_body.price_diff=" << msg.fields.spread_order_body.price_diff.get().value()
       << ",spread_order_body.ms_spd_leg_info.token_2=" << msg.fields.spread_order_body.ms_spd_leg_info.token_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.contract_desc.instrument_name=\"" << msg.fields.spread_order_body.ms_spd_leg_info.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",spread_order_body.ms_spd_leg_info.contract_desc.symbol=\"" << msg.fields.spread_order_body.ms_spd_leg_info.contract_desc.symbol.get_trimmed().value() << '"'
       << ",spread_order_body.ms_spd_leg_info.contract_desc.expiry_date=" << msg.fields.spread_order_body.ms_spd_leg_info.contract_desc.expiry_date.get().value()
       << ",spread_order_body.ms_spd_leg_info.contract_desc.strike_price=" << msg.fields.spread_order_body.ms_spd_leg_info.contract_desc.strike_price.get().value()
       << ",spread_order_body.ms_spd_leg_info.contract_desc.option_type=\"" << msg.fields.spread_order_body.ms_spd_leg_info.contract_desc.option_type.get_trimmed().value() << '"'
       << ",spread_order_body.ms_spd_leg_info.contract_desc.ca_level=" << msg.fields.spread_order_body.ms_spd_leg_info.contract_desc.ca_level.get().value()
       << ",spread_order_body.ms_spd_leg_info.op_broker_id_2=\"" << msg.fields.spread_order_body.ms_spd_leg_info.op_broker_id_2.get_trimmed().value() << '"'
       << ",spread_order_body.ms_spd_leg_info.fillerx_2=\"" << msg.fields.spread_order_body.ms_spd_leg_info.fillerx_2.get().value() << '"'
       << ",spread_order_body.ms_spd_leg_info.order_type_2=" << msg.fields.spread_order_body.ms_spd_leg_info.order_type_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.buy_sell_2=" << msg.fields.spread_order_body.ms_spd_leg_info.buy_sell_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.disclosed_vol_2=" << msg.fields.spread_order_body.ms_spd_leg_info.disclosed_vol_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.disclosed_vol_remaining_2=" << msg.fields.spread_order_body.ms_spd_leg_info.disclosed_vol_remaining_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.total_vol_remaining_2=" << msg.fields.spread_order_body.ms_spd_leg_info.total_vol_remaining_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.volume_2=" << msg.fields.spread_order_body.ms_spd_leg_info.volume_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.volume_filled_today_2=" << msg.fields.spread_order_body.ms_spd_leg_info.volume_filled_today_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.price_2=" << msg.fields.spread_order_body.ms_spd_leg_info.price_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.trigger_price_2=" << msg.fields.spread_order_body.ms_spd_leg_info.trigger_price_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.min_fill_aon_2=" << msg.fields.spread_order_body.ms_spd_leg_info.min_fill_aon_2.get().value()
       << ",spread_order_body.ms_spd_leg_info.st_order_flags=" << static_cast<unsigned>(msg.fields.spread_order_body.ms_spd_leg_info.st_order_flags.get().value())
       << ",spread_order_body.ms_spd_leg_info.open_close_2=\"" << msg.fields.spread_order_body.ms_spd_leg_info.open_close_2.get().value() << '"'
       << ",spread_order_body.ms_spd_leg_info.additional_order_flags=" << static_cast<unsigned>(msg.fields.spread_order_body.ms_spd_leg_info.additional_order_flags.get().value())
       << ",spread_order_body.ms_spd_leg_info.filler_y=\"" << msg.fields.spread_order_body.ms_spd_leg_info.filler_y.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const secure_box_registration_request_in_message& msg) {
    os << "box_id=" << msg.fields.box_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const box_sign_on_request_in_message& msg) {
    os << "box_id=" << msg.fields.box_id.get().value()
       << ",broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",session_key=\"" << msg.fields.session_key.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const box_sign_off_message& msg) {
    os << "box_id=" << msg.fields.box_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const branch_order_value_limit_update_message& msg) {
    os << "broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",branch_id=" << msg.fields.branch_id.get().value()
       << ",branch_limits.branch_buy_value_limit=" << msg.fields.branch_limits.branch_buy_value_limit.get().value()
       << ",branch_limits.branch_sell_value_limit=" << msg.fields.branch_limits.branch_sell_value_limit.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const user_order_value_limit_update_message& msg) {
    os << "broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",branch_id=" << msg.fields.branch_id.get().value()
       << ",user_id=" << msg.fields.user_id.get().value()
       << ",user_limits.user_order_buy_value_limit=" << msg.fields.user_limits.user_order_buy_value_limit.get().value()
       << ",user_limits.user_order_sell_value_limit=" << msg.fields.user_limits.user_order_sell_value_limit.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const normal_order_limit_update_message& msg) {
    os << "broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",user_id=" << msg.fields.user_id.get().value()
       << ",order_qty_limit=" << msg.fields.order_qty_limit.get().value()
       << ",order_val_limit=" << msg.fields.order_val_limit.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const reset_user_password_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const collateral_user_status_change_request_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       << ",col_user_bit=\"" << msg.fields.col_user_bit.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const user_trade_modify_cancel_status_change_request_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       << ",trd_mod_cxl_bit=\"" << msg.fields.trd_mod_cxl_bit.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const user_address_unlock_request_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sign_on_request_out_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       << ",password=\"" << msg.fields.password.get_trimmed().value() << '"'
       << ",new_password=\"" << msg.fields.new_password.get_trimmed().value() << '"'
       << ",trader_name=\"" << msg.fields.trader_name.get_trimmed().value() << '"'
       << ",last_password_change_date=" << msg.fields.last_password_change_date.get().value()
       << ",broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",branch_id=" << msg.fields.branch_id.get().value()
       << ",version_number=" << msg.fields.version_number.get().value()
       << ",end_time=" << msg.fields.end_time.get().value()
       << ",colour=\"" << msg.fields.colour.get_trimmed().value() << '"'
       << ",user_type=" << msg.fields.user_type.get().value()
       << ",sequence_number=" << msg.fields.sequence_number.get().value()
       << ",broker_status=\"" << msg.fields.broker_status.get().value() << '"'
       << ",show_index=\"" << msg.fields.show_index.get().value() << '"'
       << ",st_broker_eligibility_per_mkt=" << static_cast<unsigned>(msg.fields.st_broker_eligibility_per_mkt.get().value())
       << ",member_type=" << msg.fields.member_type.get().value()
       << ",clearing_status=\"" << msg.fields.clearing_status.get().value() << '"'
       << ",broker_name=\"" << msg.fields.broker_name.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sign_off_request_out_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const system_information_out_message& msg) {
    os << "st_market_status.market_status_normal=\"" << nnf_orderentry::market_status_normal::to_string(msg.fields.st_market_status.market_status_normal.get().value()) << '"'
       << ",st_market_status.market_status_oddlot=\"" << nnf_orderentry::market_status_oddlot::to_string(msg.fields.st_market_status.market_status_oddlot.get().value()) << '"'
       << ",st_market_status.market_status_spot=\"" << nnf_orderentry::market_status_spot::to_string(msg.fields.st_market_status.market_status_spot.get().value()) << '"'
       << ",st_market_status.market_status_auction=\"" << nnf_orderentry::market_status_auction::to_string(msg.fields.st_market_status.market_status_auction.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_normal=\"" << nnf_orderentry::ex_market_status_normal::to_string(msg.fields.st_ex_market_status.ex_market_status_normal.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_oddlot=\"" << nnf_orderentry::ex_market_status_oddlot::to_string(msg.fields.st_ex_market_status.ex_market_status_oddlot.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_spot=\"" << nnf_orderentry::ex_market_status_spot::to_string(msg.fields.st_ex_market_status.ex_market_status_spot.get().value()) << '"'
       << ",st_ex_market_status.ex_market_status_auction=\"" << nnf_orderentry::ex_market_status_auction::to_string(msg.fields.st_ex_market_status.ex_market_status_auction.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_normal=\"" << nnf_orderentry::pl_market_status_normal::to_string(msg.fields.st_pl_market_status.pl_market_status_normal.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_oddlot=\"" << nnf_orderentry::pl_market_status_oddlot::to_string(msg.fields.st_pl_market_status.pl_market_status_oddlot.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_spot=\"" << nnf_orderentry::pl_market_status_spot::to_string(msg.fields.st_pl_market_status.pl_market_status_spot.get().value()) << '"'
       << ",st_pl_market_status.pl_market_status_auction=\"" << nnf_orderentry::pl_market_status_auction::to_string(msg.fields.st_pl_market_status.pl_market_status_auction.get().value()) << '"'
       << ",update_portfolio=\"" << msg.fields.update_portfolio.get().value() << '"'
       << ",market_index=" << msg.fields.market_index.get().value()
       << ",default_settlement_period_normal=" << msg.fields.default_settlement_period_normal.get().value()
       << ",default_settlement_period_spot=" << msg.fields.default_settlement_period_spot.get().value()
       << ",default_settlement_period_auction=" << msg.fields.default_settlement_period_auction.get().value()
       << ",competitor_period=" << msg.fields.competitor_period.get().value()
       << ",solicitor_period=" << msg.fields.solicitor_period.get().value()
       << ",warning_percent=" << msg.fields.warning_percent.get().value()
       << ",volume_freeze_percent=" << msg.fields.volume_freeze_percent.get().value()
       << ",snap_quote_time=" << msg.fields.snap_quote_time.get().value()
       << ",board_lot_quantity=" << msg.fields.board_lot_quantity.get().value()
       << ",tick_size=" << msg.fields.tick_size.get().value()
       << ",maximum_gtc_days=" << msg.fields.maximum_gtc_days.get().value()
       << ",st_stock_eligible_indicators=" << static_cast<unsigned>(msg.fields.st_stock_eligible_indicators.get().value())
       << ",disclosed_quantity_percent_allowed=" << msg.fields.disclosed_quantity_percent_allowed.get().value()
       << ",risk_free_interest_rate=" << msg.fields.risk_free_interest_rate.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const gateway_router_response_message& msg) {
    os << "box_id=" << msg.fields.box_id.get().value()
       << ",broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",filler=\"" << msg.fields.filler.get().value() << '"'
       << ",ip_address=\"" << msg.fields.ip_address.get_trimmed().value() << '"'
       << ",port=" << msg.fields.port.get().value()
       << ",session_key=\"" << msg.fields.session_key.get_trimmed().value() << '"'
       << ",cryptographic_key=\"" << msg.fields.cryptographic_key.get_trimmed().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const update_local_database_header_message&) {
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const update_local_database_trailer_message&) {
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const update_local_database_data_message& msg) {
    os << "inner_header.inner_trader_id=" << msg.fields.inner_header.inner_trader_id.get().value()
       << ",inner_header.inner_log_time=" << msg.fields.inner_header.inner_log_time.get().value()
       << ",inner_header.inner_alpha_char=\"" << msg.fields.inner_header.inner_alpha_char.get_trimmed().value() << '"'
       << ",inner_header.inner_transaction_code=" << msg.fields.inner_header.inner_transaction_code.get().value()
       << ",inner_header.inner_error_code=" << msg.fields.inner_header.inner_error_code.get().value()
       << ",inner_header.inner_timestamp=" << msg.fields.inner_header.inner_timestamp.get().value()
       << ",inner_header.inner_time_stamp_1=\"" << msg.fields.inner_header.inner_time_stamp_1.get_trimmed().value() << '"'
       << ",inner_header.inner_time_stamp_2=\"" << msg.fields.inner_header.inner_time_stamp_2.get_trimmed().value() << '"'
       << ",inner_header.inner_message_length=" << msg.fields.inner_header.inner_message_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const message_record_message& msg) {
    os << "inner_header.inner_trader_id=" << msg.fields.inner_header.inner_trader_id.get().value()
       << ",inner_header.inner_log_time=" << msg.fields.inner_header.inner_log_time.get().value()
       << ",inner_header.inner_alpha_char=\"" << msg.fields.inner_header.inner_alpha_char.get_trimmed().value() << '"'
       << ",inner_header.inner_transaction_code=" << msg.fields.inner_header.inner_transaction_code.get().value()
       << ",inner_header.inner_error_code=" << msg.fields.inner_header.inner_error_code.get().value()
       << ",inner_header.inner_timestamp=" << msg.fields.inner_header.inner_timestamp.get().value()
       << ",inner_header.inner_time_stamp_1=\"" << msg.fields.inner_header.inner_time_stamp_1.get_trimmed().value() << '"'
       << ",inner_header.inner_time_stamp_2=\"" << msg.fields.inner_header.inner_time_stamp_2.get_trimmed().value() << '"'
       << ",inner_header.inner_message_length=" << msg.fields.inner_header.inner_message_length.get().value()
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trade_confirmation_message& msg) {
    os << "response_order_number=" << msg.fields.response_order_number.get().value()
       << ",broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",trader_number=" << msg.fields.trader_number.get().value()
       << ",account_number=\"" << msg.fields.account_number.get_trimmed().value() << '"'
       << ",buy_sell_indicator=\"" << nnf_orderentry::buy_sell_indicator::to_string(msg.fields.buy_sell_indicator.get().value()) << '"'
       << ",original_volume=" << msg.fields.original_volume.get().value()
       << ",disclosed_volume=" << msg.fields.disclosed_volume.get().value()
       << ",remaining_volume=" << msg.fields.remaining_volume.get().value()
       << ",disclosed_volume_remaining=" << msg.fields.disclosed_volume_remaining.get().value()
       << ",price=" << msg.fields.price.get().value()
       << ",st_order_flags=" << static_cast<unsigned>(msg.fields.st_order_flags.get().value())
       << ",good_till_date=" << msg.fields.good_till_date.get().value()
       << ",fill_number=" << msg.fields.fill_number.get().value()
       << ",fill_quantity=" << msg.fields.fill_quantity.get().value()
       << ",fill_price=" << msg.fields.fill_price.get().value()
       << ",volume_filled_today=" << msg.fields.volume_filled_today.get().value()
       << ",activity_type=\"" << msg.fields.activity_type.get_trimmed().value() << '"'
       << ",activity_time=" << msg.fields.activity_time.get().value()
       << ",counter_trader_order_number=" << msg.fields.counter_trader_order_number.get().value()
       << ",counter_broker_id=\"" << msg.fields.counter_broker_id.get_trimmed().value() << '"'
       << ",token=" << msg.fields.token.get().value()
       << ",contract_desc.instrument_name=\"" << msg.fields.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",contract_desc.symbol=\"" << msg.fields.contract_desc.symbol.get_trimmed().value() << '"'
       << ",contract_desc.expiry_date=" << msg.fields.contract_desc.expiry_date.get().value()
       << ",contract_desc.strike_price=" << msg.fields.contract_desc.strike_price.get().value()
       << ",contract_desc.option_type=\"" << msg.fields.contract_desc.option_type.get_trimmed().value() << '"'
       << ",contract_desc.ca_level=" << msg.fields.contract_desc.ca_level.get().value()
       << ",open_close=\"" << msg.fields.open_close.get().value() << '"'
       << ",old_open_close=\"" << msg.fields.old_open_close.get().value() << '"'
       << ",trade_book_type=\"" << msg.fields.trade_book_type.get().value() << '"'
       << ",old_account_number=\"" << msg.fields.old_account_number.get_trimmed().value() << '"'
       << ",participant=\"" << msg.fields.participant.get_trimmed().value() << '"'
       << ",old_participant=\"" << msg.fields.old_participant.get_trimmed().value() << '"'
       << ",additional_order_flags=" << static_cast<unsigned>(msg.fields.additional_order_flags.get().value())
       << ",pan=\"" << msg.fields.pan.get_trimmed().value() << '"'
       << ",old_pan=\"" << msg.fields.old_pan.get_trimmed().value() << '"'
       << ",algo_id=" << msg.fields.algo_id.get().value()
       << ",last_activity_reference=" << msg.fields.last_activity_reference.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const user_order_limit_update_message& msg) {
    os << "broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",branch_id=" << msg.fields.branch_id.get().value()
       << ",user_name=\"" << msg.fields.user_name.get_trimmed().value() << '"'
       << ",user_id=" << msg.fields.user_id.get().value()
       << ",user_type=" << msg.fields.user_type.get().value()
       << ",instrument_user.branch_buy_value_limit=" << msg.fields.instrument_user.branch_buy_value_limit.get().value()
       << ",instrument_user.branch_sell_value_limit=" << msg.fields.instrument_user.branch_sell_value_limit.get().value()
       << ",instrument_user.branch_used_buy_value_limit=" << msg.fields.instrument_user.branch_used_buy_value_limit.get().value()
       << ",instrument_user.branch_used_sell_value_limit=" << msg.fields.instrument_user.branch_used_sell_value_limit.get().value()
       << ",instrument_user.user_order_buy_value_limit=" << msg.fields.instrument_user.user_order_buy_value_limit.get().value()
       << ",instrument_user.user_order_sell_value_limit=" << msg.fields.instrument_user.user_order_sell_value_limit.get().value()
       << ",instrument_user.user_order_used_buy_value_limit=" << msg.fields.instrument_user.user_order_used_buy_value_limit.get().value()
       << ",instrument_user.user_order_used_sell_value_limit=" << msg.fields.instrument_user.user_order_used_sell_value_limit.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const dealer_limit_update_message& msg) {
    os << "broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",user_id=" << msg.fields.user_id.get().value()
       << ",ord_qty_buff=" << msg.fields.ord_qty_buff.get().value()
       << ",ord_val_buff=" << msg.fields.ord_val_buff.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_order_limit_update_message& msg) {
    os << "broker_id=\"" << msg.fields.broker_id.get_trimmed().value() << '"'
       << ",user_id=" << msg.fields.user_id.get().value()
       << ",spd_ord_qty_buff=" << msg.fields.spd_ord_qty_buff.get().value()
       << ",spd_ord_val_buff=" << msg.fields.spd_ord_val_buff.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const control_message_to_trader_message& msg) {
    os << "trader_id=" << msg.fields.trader_id.get().value()
       << ",action_code=\"" << msg.fields.action_code.get_trimmed().value() << '"'
       << ",broad_cast_message_length=" << msg.fields.broad_cast_message_length.get().value()
       << ",broad_cast_message=\"" << msg.fields.broad_cast_message.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const market_statistics_report_message& msg) {
    os << "message_type=\"" << msg.fields.message_type.get().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const enhanced_market_statistics_report_message& msg) {
    os << "message_type=\"" << msg.fields.message_type.get().value() << '"'
       << ",number_of_records=" << msg.fields.number_of_records.get().value()
       << ",enhncd_mkt_stats_data.contract_desc.instrument_name=\"" << msg.fields.enhncd_mkt_stats_data.contract_desc.instrument_name.get_trimmed().value() << '"'
       << ",enhncd_mkt_stats_data.contract_desc.symbol=\"" << msg.fields.enhncd_mkt_stats_data.contract_desc.symbol.get_trimmed().value() << '"'
       << ",enhncd_mkt_stats_data.contract_desc.expiry_date=" << msg.fields.enhncd_mkt_stats_data.contract_desc.expiry_date.get().value()
       << ",enhncd_mkt_stats_data.contract_desc.strike_price=" << msg.fields.enhncd_mkt_stats_data.contract_desc.strike_price.get().value()
       << ",enhncd_mkt_stats_data.contract_desc.option_type=\"" << msg.fields.enhncd_mkt_stats_data.contract_desc.option_type.get_trimmed().value() << '"'
       << ",enhncd_mkt_stats_data.contract_desc.ca_level=" << msg.fields.enhncd_mkt_stats_data.contract_desc.ca_level.get().value()
       << ",enhncd_mkt_stats_data.market_type=" << msg.fields.enhncd_mkt_stats_data.market_type.get().value()
       << ",enhncd_mkt_stats_data.open_price=" << msg.fields.enhncd_mkt_stats_data.open_price.get().value()
       << ",enhncd_mkt_stats_data.high_price=" << msg.fields.enhncd_mkt_stats_data.high_price.get().value()
       << ",enhncd_mkt_stats_data.low_price=" << msg.fields.enhncd_mkt_stats_data.low_price.get().value()
       << ",enhncd_mkt_stats_data.closing_price=" << msg.fields.enhncd_mkt_stats_data.closing_price.get().value()
       << ",enhncd_mkt_stats_data.total_quantity_traded_short=" << msg.fields.enhncd_mkt_stats_data.total_quantity_traded_short.get().value()
       << ",enhncd_mkt_stats_data.total_value_traded=" << msg.fields.enhncd_mkt_stats_data.total_value_traded.get().value()
       << ",enhncd_mkt_stats_data.previous_close_price=" << msg.fields.enhncd_mkt_stats_data.previous_close_price.get().value()
       << ",enhncd_mkt_stats_data.open_interest_long=" << msg.fields.enhncd_mkt_stats_data.open_interest_long.get().value()
       << ",enhncd_mkt_stats_data.chg_open_interest=" << msg.fields.enhncd_mkt_stats_data.chg_open_interest.get().value()
       << ",enhncd_mkt_stats_data.indicator=\"" << msg.fields.enhncd_mkt_stats_data.indicator.get_trimmed().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const market_index_report_message& msg) {
    os << "message_type=\"" << msg.fields.message_type.get().value() << '"'
       << ",index_name=\"" << msg.fields.index_name.get_trimmed().value() << '"'
       << ",mkt_index.market_opening_index=" << msg.fields.mkt_index.market_opening_index.get().value()
       << ",mkt_index.market_high_index=" << msg.fields.mkt_index.market_high_index.get().value()
       << ",mkt_index.market_low_index=" << msg.fields.mkt_index.market_low_index.get().value()
       << ",mkt_index.market_closing_index=" << msg.fields.mkt_index.market_closing_index.get().value()
       << ",mkt_index.market_start_index=" << msg.fields.mkt_index.market_start_index.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const industry_index_report_message& msg) {
    os << "message_type=\"" << msg.fields.message_type.get().value() << '"'
       << ",number_of_industry_records=" << msg.fields.number_of_industry_records.get().value()
       << ",industry_index.industry_name=\"" << msg.fields.industry_index.industry_name.get_trimmed().value() << '"'
       << ",industry_index.opening=" << msg.fields.industry_index.opening.get().value()
       << ",industry_index.high=" << msg.fields.industry_index.high.get().value()
       << ",industry_index.low=" << msg.fields.industry_index.low.get().value()
       << ",industry_index.closing=" << msg.fields.industry_index.closing.get().value()
       << ",industry_index.start=" << msg.fields.industry_index.start.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sector_index_report_message& msg) {
    os << "message_type=\"" << msg.fields.message_type.get().value() << '"'
       << ",industry_name=\"" << msg.fields.industry_name.get_trimmed().value() << '"'
       << ",number_of_industry_records=" << msg.fields.number_of_industry_records.get().value()
       << ",index_data.sector_name=\"" << msg.fields.index_data.sector_name.get_trimmed().value() << '"'
       << ",index_data.index_value=" << msg.fields.index_data.index_value.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_bhavcopy_message& msg) {
    os << "message_type=\"" << msg.fields.message_type.get().value() << '"'
       << ",tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const global_indices_message& msg) {
    os << "index_details.token=" << msg.fields.index_details.token.get().value()
       << ",index_details.name=\"" << msg.fields.index_details.name.get_trimmed().value() << '"'
       << ",index_details.open=" << msg.fields.index_details.open.get().value()
       << ",index_details.high=" << msg.fields.index_details.high.get().value()
       << ",index_details.low=" << msg.fields.index_details.low.get().value()
       << ",index_details.last=" << msg.fields.index_details.last.get().value()
       << ",index_details.close=" << msg.fields.index_details.close.get().value()
       << ",index_details.prev_close=" << msg.fields.index_details.prev_close.get().value()
       << ",index_details.life_high=" << msg.fields.index_details.life_high.get().value()
       << ",index_details.life_low=" << msg.fields.index_details.life_low.get().value()
       << ",index_details.filler_4=" << msg.fields.index_details.filler_4.get().value()
       << ",index_details.filler_2=" << msg.fields.index_details.filler_2.get().value()
       << ",index_details.filler_3=" << msg.fields.index_details.filler_3.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const global_contracts_message& msg) {
    os << "contracts_details.token=" << msg.fields.contracts_details.token.get().value()
       << ",contracts_details.nse_symbol=\"" << msg.fields.contracts_details.nse_symbol.get_trimmed().value() << '"'
       << ",contracts_details.instrument_name=\"" << msg.fields.contracts_details.instrument_name.get_trimmed().value() << '"'
       << ",contracts_details.exp_day=" << msg.fields.contracts_details.exp_day.get().value()
       << ",contracts_details.exp_month=" << msg.fields.contracts_details.exp_month.get().value()
       << ",contracts_details.exp_year=" << msg.fields.contracts_details.exp_year.get().value()
       << ",contracts_details.option_type=\"" << msg.fields.contracts_details.option_type.get_trimmed().value() << '"'
       << ",contracts_details.strike_price=" << msg.fields.contracts_details.strike_price.get().value()
       << ",contracts_details.bid_price=" << msg.fields.contracts_details.bid_price.get().value()
       << ",contracts_details.ask_price=" << msg.fields.contracts_details.ask_price.get().value()
       << ",contracts_details.bid_size=" << msg.fields.contracts_details.bid_size.get().value()
       << ",contracts_details.ask_size=" << msg.fields.contracts_details.ask_size.get().value()
       << ",contracts_details.open=" << msg.fields.contracts_details.open.get().value()
       << ",contracts_details.high=" << msg.fields.contracts_details.high.get().value()
       << ",contracts_details.low=" << msg.fields.contracts_details.low.get().value()
       << ",contracts_details.last=" << msg.fields.contracts_details.last.get().value()
       << ",contracts_details.close=" << msg.fields.contracts_details.close.get().value()
       << ",contracts_details.prev_close=" << msg.fields.contracts_details.prev_close.get().value()
       << ",contracts_details.limit_high=" << msg.fields.contracts_details.limit_high.get().value()
       << ",contracts_details.limit_low=" << msg.fields.contracts_details.limit_low.get().value()
       << ",contracts_details.total_trades=" << msg.fields.contracts_details.total_trades.get().value()
       << ",contracts_details.open_interest_double=" << msg.fields.contracts_details.open_interest_double.get().value()
       << ",contracts_details.filler_4=" << msg.fields.contracts_details.filler_4.get().value()
       << ",contracts_details.filler_2=" << msg.fields.contracts_details.filler_2.get().value()
       << ",contracts_details.filler_3=" << msg.fields.contracts_details.filler_3.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const box_sign_on_request_out_message& msg) {
    os << "box_id=" << msg.fields.box_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const contingency_broadcast_message& msg) {
    os << "stream_number=" << msg.fields.stream_number.get().value()
       << ",status=\"" << nnf_orderentry::status::to_string(msg.fields.status.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const collateral_user_status_change_response_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       << ",col_user_bit=\"" << msg.fields.col_user_bit.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const user_trade_modify_cancel_status_change_response_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       << ",trd_mod_cxl_bit=\"" << msg.fields.trd_mod_cxl_bit.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const user_address_unlock_confirm_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const user_address_unlock_approve_message& msg) {
    os << "user_id=" << msg.fields.user_id.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const giveup_confirmation_message& msg) {
    os << "reason_code=\"" << nnf_orderentry::reason_code::to_string(msg.fields.reason_code.get().value()) << '"'
       << ",giveup.order_number=" << msg.fields.giveup.order_number.get().value()
       << ",giveup.fill_number=" << msg.fields.giveup.fill_number.get().value()
       << ",giveup.instrument_name=\"" << msg.fields.giveup.instrument_name.get_trimmed().value() << '"'
       << ",giveup.symbol=\"" << msg.fields.giveup.symbol.get_trimmed().value() << '"'
       << ",giveup.expiry_date=" << msg.fields.giveup.expiry_date.get().value()
       << ",giveup.strike_price=" << msg.fields.giveup.strike_price.get().value()
       << ",giveup.option_type=\"" << msg.fields.giveup.option_type.get_trimmed().value() << '"'
       << ",giveup.ca_level=" << msg.fields.giveup.ca_level.get().value()
       << ",giveup.fill_volume=" << msg.fields.giveup.fill_volume.get().value()
       << ",giveup.fill_price=" << msg.fields.giveup.fill_price.get().value()
       << ",giveup.broker_id=\"" << msg.fields.giveup.broker_id.get_trimmed().value() << '"'
       << ",giveup.filler=\"" << msg.fields.giveup.filler.get().value() << '"'
       << ",giveup.buy_sell=\"" << nnf_orderentry::buy_sell::to_string(msg.fields.giveup.buy_sell.get().value()) << '"'
       << ",giveup.book_type=\"" << nnf_orderentry::book_type::to_string(msg.fields.giveup.book_type.get().value()) << '"'
       << ",giveup.last_modified_date_time=" << msg.fields.giveup.last_modified_date_time.get().value()
       << ",giveup.initiated_by_control=\"" << msg.fields.giveup.initiated_by_control.get().value() << '"'
       << ",giveup.open_close=\"" << msg.fields.giveup.open_close.get().value() << '"'
       << ",giveup.participant=\"" << msg.fields.giveup.participant.get_trimmed().value() << '"'
       << ",giveup.giveup_flag=\"" << msg.fields.giveup.giveup_flag.get().value() << '"'
       << ",giveup.deleted=\"" << msg.fields.giveup.deleted.get().value() << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_cancellation_confirmation_message& msg) {
    os << "tail=" << json::tail_to_json_string(msg)
       ;
    return os;
}

}
