#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/ExecId.hpp"
#include "../types/SenderId.hpp"
#include "../types/Clordid.hpp"
#include "../structs/Volatility.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/LastPx.hpp"
#include "../types/OrderId.hpp"
#include "../types/UnderlyingPx.hpp"
#include "../types/TransactTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/SecExecId.hpp"
#include "../types/Location.hpp"
#include "../structs/OptionDelta.hpp"
#include "../structs/TimeToExpiration.hpp"
#include "../structs/RiskFreeRate.hpp"
#include "../types/SecurityId.hpp"
#include "../types/LastQty.hpp"
#include "../types/CumQty.hpp"
#include "../types/SideTradeId.hpp"
#include "../types/TradeDate.hpp"
#include "../types/OrdStatusTrd.hpp"
#include "../types/OrdTypeOptional.hpp"
#include "../types/Side.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/SettlDate.hpp"
#include "../structs/CalculatedCcyLastQty.hpp"
#include "../structs/GrossTradeAmt.hpp"
#include "../types/FillPx.hpp"
#include "../types/FillQty.hpp"
#include "../types/FillExecId.hpp"
#include "../types/FillYieldType.hpp"
#include "../structs/GroupSize.hpp"
#include "../types/OrderEventPx.hpp"
#include "../types/OrderEventText.hpp"
#include "../types/OrderEventExecId.hpp"
#include "../types/OrderEventQty.hpp"
#include "../types/OrderEventType.hpp"
#include "../types/OrderEventReason.hpp"
#include "../structs/ContraGrossTradeAmt.hpp"
#include "../structs/ContraCalculatedCcyLastQty.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct execution_report_trade_spread_leg_fills_groups_entry {
    sbe_ilink3::fill_px fill_px;
    sbe_ilink3::fill_qty fill_qty;
    sbe_ilink3::fill_exec_id fill_exec_id;
    sbe_ilink3::fill_yield_type fill_yield_type;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct execution_report_trade_spread_leg_spread_leg_order_events_groups_entry {
    sbe_ilink3::order_event_px order_event_px;
    sbe_ilink3::order_event_text order_event_text;
    sbe_ilink3::order_event_exec_id order_event_exec_id;
    sbe_ilink3::order_event_qty order_event_qty;
    sbe_ilink3::order_event_type order_event_type;
    sbe_ilink3::order_event_reason order_event_reason;
    sbe_ilink3::contra_gross_trade_amt contra_gross_trade_amt;
    sbe_ilink3::contra_calculated_ccy_last_qty contra_calculated_ccy_last_qty;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Execution Report Trade Spread Leg
struct execution_report_trade_spread_leg {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::exec_id exec_id;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::volatility volatility;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::last_px last_px;
        sbe_ilink3::order_id order_id;
        sbe_ilink3::underlying_px underlying_px;
        sbe_ilink3::transact_time transact_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::sec_exec_id sec_exec_id;
        sbe_ilink3::location location;
        sbe_ilink3::option_delta option_delta;
        sbe_ilink3::time_to_expiration time_to_expiration;
        sbe_ilink3::risk_free_rate risk_free_rate;
        sbe_ilink3::security_id security_id;
        sbe_ilink3::last_qty last_qty;
        sbe_ilink3::cum_qty cum_qty;
        sbe_ilink3::side_trade_id side_trade_id;
        sbe_ilink3::trade_date trade_date;
        sbe_ilink3::ord_status_trd ord_status_trd;
        sbe_ilink3::ord_type_optional ord_type_optional;
        sbe_ilink3::side side;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::settl_date settl_date;
        sbe_ilink3::calculated_ccy_last_qty calculated_ccy_last_qty;
        sbe_ilink3::gross_trade_amt gross_trade_amt;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::execution_report_trade_spread_leg, schema_id(8), version(9)};

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(sbe_ilink3::simple_open_framing_header) - sizeof(fields_type) - sizeof(sbe_ilink3::message_header);

    fields_type fields;
    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.message_length.get().value();
        return sz == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)
            ? tail + tail_capacity
            : tail + (sz - sizeof(sbe_ilink3::simple_open_framing_header) - sizeof(fields_type) - sizeof(sbe_ilink3::message_header));
    }

    // sequential access to variable-length regions
    sbe_group_iterator<execution_report_trade_spread_leg_fills_groups_entry, group_size> fills_groups() const {
        return { tail_begin(), tail_end() };
    }

    sbe_group_iterator<execution_report_trade_spread_leg_spread_leg_order_events_groups_entry, group_size> spread_leg_order_events_groups(const std::byte* position) const {
        return { position, tail_end() };
    }


    // parse method
    static execution_report_trade_spread_leg* parse(std::byte* buffer) {
        return reinterpret_cast<execution_report_trade_spread_leg*>(buffer);
    }

    // parse method const
    static const execution_report_trade_spread_leg* parse(const std::byte* buffer) {
        return reinterpret_cast<const execution_report_trade_spread_leg*>(buffer);
    }

};

// layout verification
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, seq_num) == 0, "unexpected offset of execution_report_trade_spread_leg::fields_type::seq_num");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, uuid) == 4, "unexpected offset of execution_report_trade_spread_leg::fields_type::uuid");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, exec_id) == 12, "unexpected offset of execution_report_trade_spread_leg::fields_type::exec_id");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, sender_id) == 52, "unexpected offset of execution_report_trade_spread_leg::fields_type::sender_id");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, clordid) == 72, "unexpected offset of execution_report_trade_spread_leg::fields_type::clordid");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, volatility) == 92, "unexpected offset of execution_report_trade_spread_leg::fields_type::volatility");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, party_details_list_req_id) == 101, "unexpected offset of execution_report_trade_spread_leg::fields_type::party_details_list_req_id");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, last_px) == 109, "unexpected offset of execution_report_trade_spread_leg::fields_type::last_px");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, order_id) == 117, "unexpected offset of execution_report_trade_spread_leg::fields_type::order_id");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, underlying_px) == 125, "unexpected offset of execution_report_trade_spread_leg::fields_type::underlying_px");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, transact_time) == 133, "unexpected offset of execution_report_trade_spread_leg::fields_type::transact_time");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, sending_time_epoch) == 141, "unexpected offset of execution_report_trade_spread_leg::fields_type::sending_time_epoch");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, sec_exec_id) == 149, "unexpected offset of execution_report_trade_spread_leg::fields_type::sec_exec_id");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, location) == 157, "unexpected offset of execution_report_trade_spread_leg::fields_type::location");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, option_delta) == 162, "unexpected offset of execution_report_trade_spread_leg::fields_type::option_delta");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, time_to_expiration) == 167, "unexpected offset of execution_report_trade_spread_leg::fields_type::time_to_expiration");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, risk_free_rate) == 172, "unexpected offset of execution_report_trade_spread_leg::fields_type::risk_free_rate");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, security_id) == 177, "unexpected offset of execution_report_trade_spread_leg::fields_type::security_id");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, last_qty) == 181, "unexpected offset of execution_report_trade_spread_leg::fields_type::last_qty");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, cum_qty) == 185, "unexpected offset of execution_report_trade_spread_leg::fields_type::cum_qty");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, side_trade_id) == 189, "unexpected offset of execution_report_trade_spread_leg::fields_type::side_trade_id");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, trade_date) == 193, "unexpected offset of execution_report_trade_spread_leg::fields_type::trade_date");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, ord_status_trd) == 195, "unexpected offset of execution_report_trade_spread_leg::fields_type::ord_status_trd");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, ord_type_optional) == 196, "unexpected offset of execution_report_trade_spread_leg::fields_type::ord_type_optional");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, side) == 197, "unexpected offset of execution_report_trade_spread_leg::fields_type::side");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, poss_retrans_flag) == 198, "unexpected offset of execution_report_trade_spread_leg::fields_type::poss_retrans_flag");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, settl_date) == 199, "unexpected offset of execution_report_trade_spread_leg::fields_type::settl_date");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, calculated_ccy_last_qty) == 201, "unexpected offset of execution_report_trade_spread_leg::fields_type::calculated_ccy_last_qty");
static_assert(offsetof(execution_report_trade_spread_leg::fields_type, gross_trade_amt) == 210, "unexpected offset of execution_report_trade_spread_leg::fields_type::gross_trade_amt");
static_assert(sizeof(execution_report_trade_spread_leg::fields_type) == 219, "unexpected sizeof execution_report_trade_spread_leg::fields_type");

#pragma pack(pop)
}

#include "details/ExecutionReportTradeSpreadLegWriter.hpp"
#include "details/ExecutionReportTradeSpreadLegReader.hpp"
