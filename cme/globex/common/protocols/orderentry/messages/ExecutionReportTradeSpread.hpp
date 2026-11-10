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
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/LastPx.hpp"
#include "../types/OrderId.hpp"
#include "../types/Price.hpp"
#include "../types/StopPx.hpp"
#include "../types/TransactTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/SecExecId.hpp"
#include "../types/CrossIdOptional.hpp"
#include "../types/HostCrossId.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityId.hpp"
#include "../types/OrderQty.hpp"
#include "../types/LastQty.hpp"
#include "../types/CumQty.hpp"
#include "../types/MdTradeEntryId.hpp"
#include "../types/SideTradeId.hpp"
#include "../types/Leaves.hpp"
#include "../types/TradeDate.hpp"
#include "../types/ExpireDate.hpp"
#include "../types/OrdStatusTrd.hpp"
#include "../types/OrdTypeOptional.hpp"
#include "../types/Side.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/AggressorIndicator.hpp"
#include "../types/CrossTypeOptional.hpp"
#include "../types/TotalNumSecurities.hpp"
#include "../types/ExecInst.hpp"
#include "../types/ExecutionMode.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/ShortSaleType.hpp"
#include "../types/FillPx.hpp"
#include "../types/FillQty.hpp"
#include "../types/FillExecId.hpp"
#include "../types/FillYieldType.hpp"
#include "../structs/GroupSize.hpp"
#include "../types/LegExecId.hpp"
#include "../types/LegLastPx.hpp"
#include "../types/LegSecurityId.hpp"
#include "../types/LegTradeId.hpp"
#include "../types/LegLastQty.hpp"
#include "../types/LegSide.hpp"
#include "../types/OrderEventPx.hpp"
#include "../types/OrderEventText.hpp"
#include "../types/OrderEventExecId.hpp"
#include "../types/OrderEventQty.hpp"
#include "../types/OrderEventType.hpp"
#include "../types/OrderEventReason.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct execution_report_trade_spread_fills_groups_entry {
    sbe_ilink3::fill_px fill_px;
    sbe_ilink3::fill_qty fill_qty;
    sbe_ilink3::fill_exec_id fill_exec_id;
    sbe_ilink3::fill_yield_type fill_yield_type;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct execution_report_trade_spread_trade_legs_groups_entry {
    sbe_ilink3::leg_exec_id leg_exec_id;
    sbe_ilink3::leg_last_px leg_last_px;
    sbe_ilink3::leg_security_id leg_security_id;
    sbe_ilink3::leg_trade_id leg_trade_id;
    sbe_ilink3::leg_last_qty leg_last_qty;
    sbe_ilink3::leg_side leg_side;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct execution_report_trade_spread_spread_order_events_groups_entry {
    sbe_ilink3::order_event_px order_event_px;
    sbe_ilink3::order_event_text order_event_text;
    sbe_ilink3::order_event_exec_id order_event_exec_id;
    sbe_ilink3::order_event_qty order_event_qty;
    sbe_ilink3::order_event_type order_event_type;
    sbe_ilink3::order_event_reason order_event_reason;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Execution Report Trade Spread
struct execution_report_trade_spread {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::exec_id exec_id;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::last_px last_px;
        sbe_ilink3::order_id order_id;
        sbe_ilink3::price price;
        sbe_ilink3::stop_px stop_px;
        sbe_ilink3::transact_time transact_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::sec_exec_id sec_exec_id;
        sbe_ilink3::cross_id_optional cross_id_optional;
        sbe_ilink3::host_cross_id host_cross_id;
        sbe_ilink3::location location;
        sbe_ilink3::security_id security_id;
        sbe_ilink3::order_qty order_qty;
        sbe_ilink3::last_qty last_qty;
        sbe_ilink3::cum_qty cum_qty;
        sbe_ilink3::md_trade_entry_id md_trade_entry_id;
        sbe_ilink3::side_trade_id side_trade_id;
        sbe_ilink3::leaves leaves;
        sbe_ilink3::trade_date trade_date;
        sbe_ilink3::expire_date expire_date;
        sbe_ilink3::ord_status_trd ord_status_trd;
        sbe_ilink3::ord_type_optional ord_type_optional;
        sbe_ilink3::side side;
        sbe_ilink3::time_in_force time_in_force;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::aggressor_indicator aggressor_indicator;
        sbe_ilink3::cross_type_optional cross_type_optional;
        sbe_ilink3::total_num_securities total_num_securities;
        sbe_ilink3::exec_inst exec_inst;
        sbe_ilink3::execution_mode execution_mode;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::short_sale_type short_sale_type;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::execution_report_trade_spread, schema_id(8), version(9)};

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
    sbe_group_iterator<execution_report_trade_spread_fills_groups_entry, group_size> fills_groups() const {
        return { tail_begin(), tail_end() };
    }

    sbe_group_iterator<execution_report_trade_spread_trade_legs_groups_entry, group_size> trade_legs_groups(const std::byte* position) const {
        return { position, tail_end() };
    }

    sbe_group_iterator<execution_report_trade_spread_spread_order_events_groups_entry, group_size> spread_order_events_groups(const std::byte* position) const {
        return { position, tail_end() };
    }


    // parse method
    static execution_report_trade_spread* parse(std::byte* buffer) {
        return reinterpret_cast<execution_report_trade_spread*>(buffer);
    }

    // parse method const
    static const execution_report_trade_spread* parse(const std::byte* buffer) {
        return reinterpret_cast<const execution_report_trade_spread*>(buffer);
    }

};

// layout verification
static_assert(offsetof(execution_report_trade_spread::fields_type, seq_num) == 0, "unexpected offset of execution_report_trade_spread::fields_type::seq_num");
static_assert(offsetof(execution_report_trade_spread::fields_type, uuid) == 4, "unexpected offset of execution_report_trade_spread::fields_type::uuid");
static_assert(offsetof(execution_report_trade_spread::fields_type, exec_id) == 12, "unexpected offset of execution_report_trade_spread::fields_type::exec_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, sender_id) == 52, "unexpected offset of execution_report_trade_spread::fields_type::sender_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, clordid) == 72, "unexpected offset of execution_report_trade_spread::fields_type::clordid");
static_assert(offsetof(execution_report_trade_spread::fields_type, party_details_list_req_id) == 92, "unexpected offset of execution_report_trade_spread::fields_type::party_details_list_req_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, last_px) == 100, "unexpected offset of execution_report_trade_spread::fields_type::last_px");
static_assert(offsetof(execution_report_trade_spread::fields_type, order_id) == 108, "unexpected offset of execution_report_trade_spread::fields_type::order_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, price) == 116, "unexpected offset of execution_report_trade_spread::fields_type::price");
static_assert(offsetof(execution_report_trade_spread::fields_type, stop_px) == 124, "unexpected offset of execution_report_trade_spread::fields_type::stop_px");
static_assert(offsetof(execution_report_trade_spread::fields_type, transact_time) == 132, "unexpected offset of execution_report_trade_spread::fields_type::transact_time");
static_assert(offsetof(execution_report_trade_spread::fields_type, sending_time_epoch) == 140, "unexpected offset of execution_report_trade_spread::fields_type::sending_time_epoch");
static_assert(offsetof(execution_report_trade_spread::fields_type, order_request_id) == 148, "unexpected offset of execution_report_trade_spread::fields_type::order_request_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, sec_exec_id) == 156, "unexpected offset of execution_report_trade_spread::fields_type::sec_exec_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, cross_id_optional) == 164, "unexpected offset of execution_report_trade_spread::fields_type::cross_id_optional");
static_assert(offsetof(execution_report_trade_spread::fields_type, host_cross_id) == 172, "unexpected offset of execution_report_trade_spread::fields_type::host_cross_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, location) == 180, "unexpected offset of execution_report_trade_spread::fields_type::location");
static_assert(offsetof(execution_report_trade_spread::fields_type, security_id) == 185, "unexpected offset of execution_report_trade_spread::fields_type::security_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, order_qty) == 189, "unexpected offset of execution_report_trade_spread::fields_type::order_qty");
static_assert(offsetof(execution_report_trade_spread::fields_type, last_qty) == 193, "unexpected offset of execution_report_trade_spread::fields_type::last_qty");
static_assert(offsetof(execution_report_trade_spread::fields_type, cum_qty) == 197, "unexpected offset of execution_report_trade_spread::fields_type::cum_qty");
static_assert(offsetof(execution_report_trade_spread::fields_type, md_trade_entry_id) == 201, "unexpected offset of execution_report_trade_spread::fields_type::md_trade_entry_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, side_trade_id) == 205, "unexpected offset of execution_report_trade_spread::fields_type::side_trade_id");
static_assert(offsetof(execution_report_trade_spread::fields_type, leaves) == 209, "unexpected offset of execution_report_trade_spread::fields_type::leaves");
static_assert(offsetof(execution_report_trade_spread::fields_type, trade_date) == 213, "unexpected offset of execution_report_trade_spread::fields_type::trade_date");
static_assert(offsetof(execution_report_trade_spread::fields_type, expire_date) == 215, "unexpected offset of execution_report_trade_spread::fields_type::expire_date");
static_assert(offsetof(execution_report_trade_spread::fields_type, ord_status_trd) == 217, "unexpected offset of execution_report_trade_spread::fields_type::ord_status_trd");
static_assert(offsetof(execution_report_trade_spread::fields_type, ord_type_optional) == 218, "unexpected offset of execution_report_trade_spread::fields_type::ord_type_optional");
static_assert(offsetof(execution_report_trade_spread::fields_type, side) == 219, "unexpected offset of execution_report_trade_spread::fields_type::side");
static_assert(offsetof(execution_report_trade_spread::fields_type, time_in_force) == 220, "unexpected offset of execution_report_trade_spread::fields_type::time_in_force");
static_assert(offsetof(execution_report_trade_spread::fields_type, manual_order_indicator) == 221, "unexpected offset of execution_report_trade_spread::fields_type::manual_order_indicator");
static_assert(offsetof(execution_report_trade_spread::fields_type, poss_retrans_flag) == 222, "unexpected offset of execution_report_trade_spread::fields_type::poss_retrans_flag");
static_assert(offsetof(execution_report_trade_spread::fields_type, aggressor_indicator) == 223, "unexpected offset of execution_report_trade_spread::fields_type::aggressor_indicator");
static_assert(offsetof(execution_report_trade_spread::fields_type, cross_type_optional) == 224, "unexpected offset of execution_report_trade_spread::fields_type::cross_type_optional");
static_assert(offsetof(execution_report_trade_spread::fields_type, total_num_securities) == 225, "unexpected offset of execution_report_trade_spread::fields_type::total_num_securities");
static_assert(offsetof(execution_report_trade_spread::fields_type, exec_inst) == 226, "unexpected offset of execution_report_trade_spread::fields_type::exec_inst");
static_assert(offsetof(execution_report_trade_spread::fields_type, execution_mode) == 227, "unexpected offset of execution_report_trade_spread::fields_type::execution_mode");
static_assert(offsetof(execution_report_trade_spread::fields_type, liquidity_flag) == 228, "unexpected offset of execution_report_trade_spread::fields_type::liquidity_flag");
static_assert(offsetof(execution_report_trade_spread::fields_type, short_sale_type) == 229, "unexpected offset of execution_report_trade_spread::fields_type::short_sale_type");
static_assert(sizeof(execution_report_trade_spread::fields_type) == 230, "unexpected sizeof execution_report_trade_spread::fields_type");

#pragma pack(pop)
}

#include "details/ExecutionReportTradeSpreadWriter.hpp"
#include "details/ExecutionReportTradeSpreadReader.hpp"
