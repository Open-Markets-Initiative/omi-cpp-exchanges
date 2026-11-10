#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/ExecId.hpp"
#include "../types/SenderId.hpp"
#include "../types/Clordid.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/OrderId.hpp"
#include "../types/Price.hpp"
#include "../types/StopPx.hpp"
#include "../types/TransactTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/CrossIdOptional.hpp"
#include "../types/HostCrossId.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityId.hpp"
#include "../types/CumQty.hpp"
#include "../types/OrderQty.hpp"
#include "../types/MinQty.hpp"
#include "../types/DisplayQty.hpp"
#include "../types/ExpireDate.hpp"
#include "../types/OrdTypeOptional.hpp"
#include "../types/Side.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/CrossTypeOptional.hpp"
#include "../types/ExecInst.hpp"
#include "../types/ExecutionMode.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/ManagedOrder.hpp"
#include "../types/ShortSaleType.hpp"
#include "../types/DiscretionPrice.hpp"
#include "../types/ReservationPrice.hpp"
#include "../types/PriorityIndicator.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Execution Report Elimination
struct execution_report_elimination {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::exec_id exec_id;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::order_id order_id;
        sbe_ilink3::price price;
        sbe_ilink3::stop_px stop_px;
        sbe_ilink3::transact_time transact_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::cross_id_optional cross_id_optional;
        sbe_ilink3::host_cross_id host_cross_id;
        sbe_ilink3::location location;
        sbe_ilink3::security_id security_id;
        sbe_ilink3::cum_qty cum_qty;
        sbe_ilink3::order_qty order_qty;
        sbe_ilink3::min_qty min_qty;
        sbe_ilink3::display_qty display_qty;
        sbe_ilink3::expire_date expire_date;
        sbe_ilink3::ord_type_optional ord_type_optional;
        sbe_ilink3::side side;
        sbe_ilink3::time_in_force time_in_force;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::cross_type_optional cross_type_optional;
        sbe_ilink3::exec_inst exec_inst;
        sbe_ilink3::execution_mode execution_mode;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::managed_order managed_order;
        sbe_ilink3::short_sale_type short_sale_type;
        sbe_ilink3::discretion_price discretion_price;
        sbe_ilink3::reservation_price reservation_price;
        sbe_ilink3::priority_indicator priority_indicator;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::execution_report_elimination, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static execution_report_elimination* parse(std::byte* buffer) {
        return reinterpret_cast<execution_report_elimination*>(buffer);
    }

    // parse method const
    static const execution_report_elimination* parse(const std::byte* buffer) {
        return reinterpret_cast<const execution_report_elimination*>(buffer);
    }

};

// layout verification
static_assert(offsetof(execution_report_elimination::fields_type, seq_num) == 0, "unexpected offset of execution_report_elimination::fields_type::seq_num");
static_assert(offsetof(execution_report_elimination::fields_type, uuid) == 4, "unexpected offset of execution_report_elimination::fields_type::uuid");
static_assert(offsetof(execution_report_elimination::fields_type, exec_id) == 12, "unexpected offset of execution_report_elimination::fields_type::exec_id");
static_assert(offsetof(execution_report_elimination::fields_type, sender_id) == 52, "unexpected offset of execution_report_elimination::fields_type::sender_id");
static_assert(offsetof(execution_report_elimination::fields_type, clordid) == 72, "unexpected offset of execution_report_elimination::fields_type::clordid");
static_assert(offsetof(execution_report_elimination::fields_type, party_details_list_req_id) == 92, "unexpected offset of execution_report_elimination::fields_type::party_details_list_req_id");
static_assert(offsetof(execution_report_elimination::fields_type, order_id) == 100, "unexpected offset of execution_report_elimination::fields_type::order_id");
static_assert(offsetof(execution_report_elimination::fields_type, price) == 108, "unexpected offset of execution_report_elimination::fields_type::price");
static_assert(offsetof(execution_report_elimination::fields_type, stop_px) == 116, "unexpected offset of execution_report_elimination::fields_type::stop_px");
static_assert(offsetof(execution_report_elimination::fields_type, transact_time) == 124, "unexpected offset of execution_report_elimination::fields_type::transact_time");
static_assert(offsetof(execution_report_elimination::fields_type, sending_time_epoch) == 132, "unexpected offset of execution_report_elimination::fields_type::sending_time_epoch");
static_assert(offsetof(execution_report_elimination::fields_type, order_request_id) == 140, "unexpected offset of execution_report_elimination::fields_type::order_request_id");
static_assert(offsetof(execution_report_elimination::fields_type, cross_id_optional) == 148, "unexpected offset of execution_report_elimination::fields_type::cross_id_optional");
static_assert(offsetof(execution_report_elimination::fields_type, host_cross_id) == 156, "unexpected offset of execution_report_elimination::fields_type::host_cross_id");
static_assert(offsetof(execution_report_elimination::fields_type, location) == 164, "unexpected offset of execution_report_elimination::fields_type::location");
static_assert(offsetof(execution_report_elimination::fields_type, security_id) == 169, "unexpected offset of execution_report_elimination::fields_type::security_id");
static_assert(offsetof(execution_report_elimination::fields_type, cum_qty) == 173, "unexpected offset of execution_report_elimination::fields_type::cum_qty");
static_assert(offsetof(execution_report_elimination::fields_type, order_qty) == 177, "unexpected offset of execution_report_elimination::fields_type::order_qty");
static_assert(offsetof(execution_report_elimination::fields_type, min_qty) == 181, "unexpected offset of execution_report_elimination::fields_type::min_qty");
static_assert(offsetof(execution_report_elimination::fields_type, display_qty) == 185, "unexpected offset of execution_report_elimination::fields_type::display_qty");
static_assert(offsetof(execution_report_elimination::fields_type, expire_date) == 189, "unexpected offset of execution_report_elimination::fields_type::expire_date");
static_assert(offsetof(execution_report_elimination::fields_type, ord_type_optional) == 191, "unexpected offset of execution_report_elimination::fields_type::ord_type_optional");
static_assert(offsetof(execution_report_elimination::fields_type, side) == 192, "unexpected offset of execution_report_elimination::fields_type::side");
static_assert(offsetof(execution_report_elimination::fields_type, time_in_force) == 193, "unexpected offset of execution_report_elimination::fields_type::time_in_force");
static_assert(offsetof(execution_report_elimination::fields_type, manual_order_indicator) == 194, "unexpected offset of execution_report_elimination::fields_type::manual_order_indicator");
static_assert(offsetof(execution_report_elimination::fields_type, poss_retrans_flag) == 195, "unexpected offset of execution_report_elimination::fields_type::poss_retrans_flag");
static_assert(offsetof(execution_report_elimination::fields_type, cross_type_optional) == 196, "unexpected offset of execution_report_elimination::fields_type::cross_type_optional");
static_assert(offsetof(execution_report_elimination::fields_type, exec_inst) == 197, "unexpected offset of execution_report_elimination::fields_type::exec_inst");
static_assert(offsetof(execution_report_elimination::fields_type, execution_mode) == 198, "unexpected offset of execution_report_elimination::fields_type::execution_mode");
static_assert(offsetof(execution_report_elimination::fields_type, liquidity_flag) == 199, "unexpected offset of execution_report_elimination::fields_type::liquidity_flag");
static_assert(offsetof(execution_report_elimination::fields_type, managed_order) == 200, "unexpected offset of execution_report_elimination::fields_type::managed_order");
static_assert(offsetof(execution_report_elimination::fields_type, short_sale_type) == 201, "unexpected offset of execution_report_elimination::fields_type::short_sale_type");
static_assert(offsetof(execution_report_elimination::fields_type, discretion_price) == 202, "unexpected offset of execution_report_elimination::fields_type::discretion_price");
static_assert(offsetof(execution_report_elimination::fields_type, reservation_price) == 210, "unexpected offset of execution_report_elimination::fields_type::reservation_price");
static_assert(offsetof(execution_report_elimination::fields_type, priority_indicator) == 218, "unexpected offset of execution_report_elimination::fields_type::priority_indicator");
static_assert(sizeof(execution_report_elimination::fields_type) == 219, "unexpected sizeof execution_report_elimination::fields_type");
static_assert(sizeof(execution_report_elimination) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 219, "unexpected sizeof execution_report_elimination");

#pragma pack(pop)
}
