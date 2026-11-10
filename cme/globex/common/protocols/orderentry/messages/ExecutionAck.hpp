#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/OrderId.hpp"
#include "../types/ExecAckStatus.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Clordid.hpp"
#include "../types/SecExecId.hpp"
#include "../types/LastPx.hpp"
#include "../types/SecurityId.hpp"
#include "../types/LastQty.hpp"
#include "../types/DkReason.hpp"
#include "../types/Side.hpp"
#include "../types/SenderId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/Location.hpp"
#include "../types/ManualOrderIndicator.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Execution Ack
struct execution_ack {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::order_id order_id;
        sbe_ilink3::exec_ack_status exec_ack_status;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::sec_exec_id sec_exec_id;
        sbe_ilink3::last_px last_px;
        sbe_ilink3::security_id security_id;
        sbe_ilink3::last_qty last_qty;
        sbe_ilink3::dk_reason dk_reason;
        sbe_ilink3::side side;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::location location;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::execution_ack, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static execution_ack* parse(std::byte* buffer) {
        return reinterpret_cast<execution_ack*>(buffer);
    }

    // parse method const
    static const execution_ack* parse(const std::byte* buffer) {
        return reinterpret_cast<const execution_ack*>(buffer);
    }

};

// layout verification
static_assert(offsetof(execution_ack::fields_type, party_details_list_req_id) == 0, "unexpected offset of execution_ack::fields_type::party_details_list_req_id");
static_assert(offsetof(execution_ack::fields_type, order_id) == 8, "unexpected offset of execution_ack::fields_type::order_id");
static_assert(offsetof(execution_ack::fields_type, exec_ack_status) == 16, "unexpected offset of execution_ack::fields_type::exec_ack_status");
static_assert(offsetof(execution_ack::fields_type, seq_num) == 17, "unexpected offset of execution_ack::fields_type::seq_num");
static_assert(offsetof(execution_ack::fields_type, clordid) == 21, "unexpected offset of execution_ack::fields_type::clordid");
static_assert(offsetof(execution_ack::fields_type, sec_exec_id) == 41, "unexpected offset of execution_ack::fields_type::sec_exec_id");
static_assert(offsetof(execution_ack::fields_type, last_px) == 49, "unexpected offset of execution_ack::fields_type::last_px");
static_assert(offsetof(execution_ack::fields_type, security_id) == 57, "unexpected offset of execution_ack::fields_type::security_id");
static_assert(offsetof(execution_ack::fields_type, last_qty) == 61, "unexpected offset of execution_ack::fields_type::last_qty");
static_assert(offsetof(execution_ack::fields_type, dk_reason) == 65, "unexpected offset of execution_ack::fields_type::dk_reason");
static_assert(offsetof(execution_ack::fields_type, side) == 66, "unexpected offset of execution_ack::fields_type::side");
static_assert(offsetof(execution_ack::fields_type, sender_id) == 67, "unexpected offset of execution_ack::fields_type::sender_id");
static_assert(offsetof(execution_ack::fields_type, sending_time_epoch) == 87, "unexpected offset of execution_ack::fields_type::sending_time_epoch");
static_assert(offsetof(execution_ack::fields_type, location) == 95, "unexpected offset of execution_ack::fields_type::location");
static_assert(offsetof(execution_ack::fields_type, manual_order_indicator) == 100, "unexpected offset of execution_ack::fields_type::manual_order_indicator");
static_assert(sizeof(execution_ack::fields_type) == 101, "unexpected sizeof execution_ack::fields_type");
static_assert(sizeof(execution_ack) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 101, "unexpected sizeof execution_ack");

#pragma pack(pop)
}
