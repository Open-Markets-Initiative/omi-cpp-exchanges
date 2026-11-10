#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/Text.hpp"
#include "../types/ExecId.hpp"
#include "../types/SenderId.hpp"
#include "../types/Clordid.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/OrderId.hpp"
#include "../types/PriceOptional.hpp"
#include "../types/StopPx.hpp"
#include "../types/TransactTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/CrossIdOptional.hpp"
#include "../types/HostCrossId.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityId.hpp"
#include "../types/OrderQty.hpp"
#include "../types/MinQty.hpp"
#include "../types/DisplayQty.hpp"
#include "../types/OrdRejReason.hpp"
#include "../types/ExpireDate.hpp"
#include "../types/DelayDuration.hpp"
#include "../types/OrdTypeOptional.hpp"
#include "../types/Side.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/CrossTypeOptional.hpp"
#include "../types/ExecInst.hpp"
#include "../types/ExecutionMode.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/ManagedOrder.hpp"
#include "../types/ShortSaleType.hpp"
#include "../types/DelayToTime.hpp"
#include "../types/DiscretionPrice.hpp"
#include "../types/ReservationPrice.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Execution Report Reject
struct execution_report_reject {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::exec_id exec_id;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::order_id order_id;
        sbe_ilink3::price_optional price_optional;
        sbe_ilink3::stop_px stop_px;
        sbe_ilink3::transact_time transact_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::cross_id_optional cross_id_optional;
        sbe_ilink3::host_cross_id host_cross_id;
        sbe_ilink3::location location;
        sbe_ilink3::security_id security_id;
        sbe_ilink3::order_qty order_qty;
        sbe_ilink3::min_qty min_qty;
        sbe_ilink3::display_qty display_qty;
        sbe_ilink3::ord_rej_reason ord_rej_reason;
        sbe_ilink3::expire_date expire_date;
        sbe_ilink3::delay_duration delay_duration;
        sbe_ilink3::ord_type_optional ord_type_optional;
        sbe_ilink3::side side;
        sbe_ilink3::time_in_force time_in_force;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::cross_type_optional cross_type_optional;
        sbe_ilink3::exec_inst exec_inst;
        sbe_ilink3::execution_mode execution_mode;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::managed_order managed_order;
        sbe_ilink3::short_sale_type short_sale_type;
        sbe_ilink3::delay_to_time delay_to_time;
        sbe_ilink3::discretion_price discretion_price;
        sbe_ilink3::reservation_price reservation_price;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::execution_report_reject, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static execution_report_reject* parse(std::byte* buffer) {
        return reinterpret_cast<execution_report_reject*>(buffer);
    }

    // parse method const
    static const execution_report_reject* parse(const std::byte* buffer) {
        return reinterpret_cast<const execution_report_reject*>(buffer);
    }

};

// layout verification
static_assert(offsetof(execution_report_reject::fields_type, seq_num) == 0, "unexpected offset of execution_report_reject::fields_type::seq_num");
static_assert(offsetof(execution_report_reject::fields_type, uuid) == 4, "unexpected offset of execution_report_reject::fields_type::uuid");
static_assert(offsetof(execution_report_reject::fields_type, text) == 12, "unexpected offset of execution_report_reject::fields_type::text");
static_assert(offsetof(execution_report_reject::fields_type, exec_id) == 268, "unexpected offset of execution_report_reject::fields_type::exec_id");
static_assert(offsetof(execution_report_reject::fields_type, sender_id) == 308, "unexpected offset of execution_report_reject::fields_type::sender_id");
static_assert(offsetof(execution_report_reject::fields_type, clordid) == 328, "unexpected offset of execution_report_reject::fields_type::clordid");
static_assert(offsetof(execution_report_reject::fields_type, party_details_list_req_id) == 348, "unexpected offset of execution_report_reject::fields_type::party_details_list_req_id");
static_assert(offsetof(execution_report_reject::fields_type, order_id) == 356, "unexpected offset of execution_report_reject::fields_type::order_id");
static_assert(offsetof(execution_report_reject::fields_type, price_optional) == 364, "unexpected offset of execution_report_reject::fields_type::price_optional");
static_assert(offsetof(execution_report_reject::fields_type, stop_px) == 372, "unexpected offset of execution_report_reject::fields_type::stop_px");
static_assert(offsetof(execution_report_reject::fields_type, transact_time) == 380, "unexpected offset of execution_report_reject::fields_type::transact_time");
static_assert(offsetof(execution_report_reject::fields_type, sending_time_epoch) == 388, "unexpected offset of execution_report_reject::fields_type::sending_time_epoch");
static_assert(offsetof(execution_report_reject::fields_type, order_request_id) == 396, "unexpected offset of execution_report_reject::fields_type::order_request_id");
static_assert(offsetof(execution_report_reject::fields_type, cross_id_optional) == 404, "unexpected offset of execution_report_reject::fields_type::cross_id_optional");
static_assert(offsetof(execution_report_reject::fields_type, host_cross_id) == 412, "unexpected offset of execution_report_reject::fields_type::host_cross_id");
static_assert(offsetof(execution_report_reject::fields_type, location) == 420, "unexpected offset of execution_report_reject::fields_type::location");
static_assert(offsetof(execution_report_reject::fields_type, security_id) == 425, "unexpected offset of execution_report_reject::fields_type::security_id");
static_assert(offsetof(execution_report_reject::fields_type, order_qty) == 429, "unexpected offset of execution_report_reject::fields_type::order_qty");
static_assert(offsetof(execution_report_reject::fields_type, min_qty) == 433, "unexpected offset of execution_report_reject::fields_type::min_qty");
static_assert(offsetof(execution_report_reject::fields_type, display_qty) == 437, "unexpected offset of execution_report_reject::fields_type::display_qty");
static_assert(offsetof(execution_report_reject::fields_type, ord_rej_reason) == 441, "unexpected offset of execution_report_reject::fields_type::ord_rej_reason");
static_assert(offsetof(execution_report_reject::fields_type, expire_date) == 443, "unexpected offset of execution_report_reject::fields_type::expire_date");
static_assert(offsetof(execution_report_reject::fields_type, delay_duration) == 445, "unexpected offset of execution_report_reject::fields_type::delay_duration");
static_assert(offsetof(execution_report_reject::fields_type, ord_type_optional) == 447, "unexpected offset of execution_report_reject::fields_type::ord_type_optional");
static_assert(offsetof(execution_report_reject::fields_type, side) == 448, "unexpected offset of execution_report_reject::fields_type::side");
static_assert(offsetof(execution_report_reject::fields_type, time_in_force) == 449, "unexpected offset of execution_report_reject::fields_type::time_in_force");
static_assert(offsetof(execution_report_reject::fields_type, manual_order_indicator) == 450, "unexpected offset of execution_report_reject::fields_type::manual_order_indicator");
static_assert(offsetof(execution_report_reject::fields_type, poss_retrans_flag) == 451, "unexpected offset of execution_report_reject::fields_type::poss_retrans_flag");
static_assert(offsetof(execution_report_reject::fields_type, split_msg) == 452, "unexpected offset of execution_report_reject::fields_type::split_msg");
static_assert(offsetof(execution_report_reject::fields_type, cross_type_optional) == 453, "unexpected offset of execution_report_reject::fields_type::cross_type_optional");
static_assert(offsetof(execution_report_reject::fields_type, exec_inst) == 454, "unexpected offset of execution_report_reject::fields_type::exec_inst");
static_assert(offsetof(execution_report_reject::fields_type, execution_mode) == 455, "unexpected offset of execution_report_reject::fields_type::execution_mode");
static_assert(offsetof(execution_report_reject::fields_type, liquidity_flag) == 456, "unexpected offset of execution_report_reject::fields_type::liquidity_flag");
static_assert(offsetof(execution_report_reject::fields_type, managed_order) == 457, "unexpected offset of execution_report_reject::fields_type::managed_order");
static_assert(offsetof(execution_report_reject::fields_type, short_sale_type) == 458, "unexpected offset of execution_report_reject::fields_type::short_sale_type");
static_assert(offsetof(execution_report_reject::fields_type, delay_to_time) == 459, "unexpected offset of execution_report_reject::fields_type::delay_to_time");
static_assert(offsetof(execution_report_reject::fields_type, discretion_price) == 467, "unexpected offset of execution_report_reject::fields_type::discretion_price");
static_assert(offsetof(execution_report_reject::fields_type, reservation_price) == 475, "unexpected offset of execution_report_reject::fields_type::reservation_price");
static_assert(sizeof(execution_report_reject::fields_type) == 483, "unexpected sizeof execution_report_reject::fields_type");
static_assert(sizeof(execution_report_reject) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 483, "unexpected sizeof execution_report_reject");

#pragma pack(pop)
}
