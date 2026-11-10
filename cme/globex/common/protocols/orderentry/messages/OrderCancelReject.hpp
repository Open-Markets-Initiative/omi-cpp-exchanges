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
#include "../types/TransactTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/Location.hpp"
#include "../types/CxlRejReason.hpp"
#include "../types/DelayDuration.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/DelayToTime.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Order Cancel Reject
struct order_cancel_reject {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::exec_id exec_id;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::order_id order_id;
        sbe_ilink3::transact_time transact_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::location location;
        sbe_ilink3::cxl_rej_reason cxl_rej_reason;
        sbe_ilink3::delay_duration delay_duration;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::delay_to_time delay_to_time;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::order_cancel_reject, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static order_cancel_reject* parse(std::byte* buffer) {
        return reinterpret_cast<order_cancel_reject*>(buffer);
    }

    // parse method const
    static const order_cancel_reject* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_cancel_reject*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_cancel_reject::fields_type, seq_num) == 0, "unexpected offset of order_cancel_reject::fields_type::seq_num");
static_assert(offsetof(order_cancel_reject::fields_type, uuid) == 4, "unexpected offset of order_cancel_reject::fields_type::uuid");
static_assert(offsetof(order_cancel_reject::fields_type, text) == 12, "unexpected offset of order_cancel_reject::fields_type::text");
static_assert(offsetof(order_cancel_reject::fields_type, exec_id) == 268, "unexpected offset of order_cancel_reject::fields_type::exec_id");
static_assert(offsetof(order_cancel_reject::fields_type, sender_id) == 308, "unexpected offset of order_cancel_reject::fields_type::sender_id");
static_assert(offsetof(order_cancel_reject::fields_type, clordid) == 328, "unexpected offset of order_cancel_reject::fields_type::clordid");
static_assert(offsetof(order_cancel_reject::fields_type, party_details_list_req_id) == 348, "unexpected offset of order_cancel_reject::fields_type::party_details_list_req_id");
static_assert(offsetof(order_cancel_reject::fields_type, order_id) == 356, "unexpected offset of order_cancel_reject::fields_type::order_id");
static_assert(offsetof(order_cancel_reject::fields_type, transact_time) == 364, "unexpected offset of order_cancel_reject::fields_type::transact_time");
static_assert(offsetof(order_cancel_reject::fields_type, sending_time_epoch) == 372, "unexpected offset of order_cancel_reject::fields_type::sending_time_epoch");
static_assert(offsetof(order_cancel_reject::fields_type, order_request_id) == 380, "unexpected offset of order_cancel_reject::fields_type::order_request_id");
static_assert(offsetof(order_cancel_reject::fields_type, location) == 388, "unexpected offset of order_cancel_reject::fields_type::location");
static_assert(offsetof(order_cancel_reject::fields_type, cxl_rej_reason) == 393, "unexpected offset of order_cancel_reject::fields_type::cxl_rej_reason");
static_assert(offsetof(order_cancel_reject::fields_type, delay_duration) == 395, "unexpected offset of order_cancel_reject::fields_type::delay_duration");
static_assert(offsetof(order_cancel_reject::fields_type, manual_order_indicator) == 397, "unexpected offset of order_cancel_reject::fields_type::manual_order_indicator");
static_assert(offsetof(order_cancel_reject::fields_type, poss_retrans_flag) == 398, "unexpected offset of order_cancel_reject::fields_type::poss_retrans_flag");
static_assert(offsetof(order_cancel_reject::fields_type, split_msg) == 399, "unexpected offset of order_cancel_reject::fields_type::split_msg");
static_assert(offsetof(order_cancel_reject::fields_type, liquidity_flag) == 400, "unexpected offset of order_cancel_reject::fields_type::liquidity_flag");
static_assert(offsetof(order_cancel_reject::fields_type, delay_to_time) == 401, "unexpected offset of order_cancel_reject::fields_type::delay_to_time");
static_assert(sizeof(order_cancel_reject::fields_type) == 409, "unexpected sizeof order_cancel_reject::fields_type");
static_assert(sizeof(order_cancel_reject) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 409, "unexpected sizeof order_cancel_reject");

#pragma pack(pop)
}
