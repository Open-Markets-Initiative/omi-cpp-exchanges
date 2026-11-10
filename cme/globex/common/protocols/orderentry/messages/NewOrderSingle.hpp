#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/PriceOptional.hpp"
#include "../types/OrderQty.hpp"
#include "../types/SecurityId.hpp"
#include "../types/Side.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderId.hpp"
#include "../types/Clordid.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/StopPx.hpp"
#include "../types/Location.hpp"
#include "../types/MinQty.hpp"
#include "../types/DisplayQty.hpp"
#include "../types/ExpireDate.hpp"
#include "../types/OrdType.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/ExecInst.hpp"
#include "../types/ExecutionMode.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/ManagedOrder.hpp"
#include "../types/ShortSaleType.hpp"
#include "../types/DiscretionPrice.hpp"
#include "../types/ReservationPrice.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// New Order Single
struct new_order_single {

    struct fields_type {
        sbe_ilink3::price_optional price_optional;
        sbe_ilink3::order_qty order_qty;
        sbe_ilink3::security_id security_id;
        sbe_ilink3::side side;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::stop_px stop_px;
        sbe_ilink3::location location;
        sbe_ilink3::min_qty min_qty;
        sbe_ilink3::display_qty display_qty;
        sbe_ilink3::expire_date expire_date;
        sbe_ilink3::ord_type ord_type;
        sbe_ilink3::time_in_force time_in_force;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::exec_inst exec_inst;
        sbe_ilink3::execution_mode execution_mode;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::managed_order managed_order;
        sbe_ilink3::short_sale_type short_sale_type;
        sbe_ilink3::discretion_price discretion_price;
        sbe_ilink3::reservation_price reservation_price;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::new_order_single, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static new_order_single* parse(std::byte* buffer) {
        return reinterpret_cast<new_order_single*>(buffer);
    }

    // parse method const
    static const new_order_single* parse(const std::byte* buffer) {
        return reinterpret_cast<const new_order_single*>(buffer);
    }

};

// layout verification
static_assert(offsetof(new_order_single::fields_type, price_optional) == 0, "unexpected offset of new_order_single::fields_type::price_optional");
static_assert(offsetof(new_order_single::fields_type, order_qty) == 8, "unexpected offset of new_order_single::fields_type::order_qty");
static_assert(offsetof(new_order_single::fields_type, security_id) == 12, "unexpected offset of new_order_single::fields_type::security_id");
static_assert(offsetof(new_order_single::fields_type, side) == 16, "unexpected offset of new_order_single::fields_type::side");
static_assert(offsetof(new_order_single::fields_type, seq_num) == 17, "unexpected offset of new_order_single::fields_type::seq_num");
static_assert(offsetof(new_order_single::fields_type, sender_id) == 21, "unexpected offset of new_order_single::fields_type::sender_id");
static_assert(offsetof(new_order_single::fields_type, clordid) == 41, "unexpected offset of new_order_single::fields_type::clordid");
static_assert(offsetof(new_order_single::fields_type, party_details_list_req_id) == 61, "unexpected offset of new_order_single::fields_type::party_details_list_req_id");
static_assert(offsetof(new_order_single::fields_type, order_request_id) == 69, "unexpected offset of new_order_single::fields_type::order_request_id");
static_assert(offsetof(new_order_single::fields_type, sending_time_epoch) == 77, "unexpected offset of new_order_single::fields_type::sending_time_epoch");
static_assert(offsetof(new_order_single::fields_type, stop_px) == 85, "unexpected offset of new_order_single::fields_type::stop_px");
static_assert(offsetof(new_order_single::fields_type, location) == 93, "unexpected offset of new_order_single::fields_type::location");
static_assert(offsetof(new_order_single::fields_type, min_qty) == 98, "unexpected offset of new_order_single::fields_type::min_qty");
static_assert(offsetof(new_order_single::fields_type, display_qty) == 102, "unexpected offset of new_order_single::fields_type::display_qty");
static_assert(offsetof(new_order_single::fields_type, expire_date) == 106, "unexpected offset of new_order_single::fields_type::expire_date");
static_assert(offsetof(new_order_single::fields_type, ord_type) == 108, "unexpected offset of new_order_single::fields_type::ord_type");
static_assert(offsetof(new_order_single::fields_type, time_in_force) == 109, "unexpected offset of new_order_single::fields_type::time_in_force");
static_assert(offsetof(new_order_single::fields_type, manual_order_indicator) == 110, "unexpected offset of new_order_single::fields_type::manual_order_indicator");
static_assert(offsetof(new_order_single::fields_type, exec_inst) == 111, "unexpected offset of new_order_single::fields_type::exec_inst");
static_assert(offsetof(new_order_single::fields_type, execution_mode) == 112, "unexpected offset of new_order_single::fields_type::execution_mode");
static_assert(offsetof(new_order_single::fields_type, liquidity_flag) == 113, "unexpected offset of new_order_single::fields_type::liquidity_flag");
static_assert(offsetof(new_order_single::fields_type, managed_order) == 114, "unexpected offset of new_order_single::fields_type::managed_order");
static_assert(offsetof(new_order_single::fields_type, short_sale_type) == 115, "unexpected offset of new_order_single::fields_type::short_sale_type");
static_assert(offsetof(new_order_single::fields_type, discretion_price) == 116, "unexpected offset of new_order_single::fields_type::discretion_price");
static_assert(offsetof(new_order_single::fields_type, reservation_price) == 124, "unexpected offset of new_order_single::fields_type::reservation_price");
static_assert(sizeof(new_order_single::fields_type) == 132, "unexpected sizeof new_order_single::fields_type");
static_assert(sizeof(new_order_single) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 132, "unexpected sizeof new_order_single");

#pragma pack(pop)
}
