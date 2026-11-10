#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/Text.hpp"
#include "../types/SenderId.hpp"
#include "../types/ExchangeQuoteReqId.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/RequestTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/QuoteReqId.hpp"
#include "../types/Location.hpp"
#include "../types/QuoteRejectReason.hpp"
#include "../types/DelayDuration.hpp"
#include "../types/QuoteAckStatus.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/DelayToTime.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Request For Quote Ack
struct request_for_quote_ack {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::exchange_quote_req_id exchange_quote_req_id;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::request_time request_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::quote_req_id quote_req_id;
        sbe_ilink3::location location;
        sbe_ilink3::quote_reject_reason quote_reject_reason;
        sbe_ilink3::delay_duration delay_duration;
        sbe_ilink3::quote_ack_status quote_ack_status;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::delay_to_time delay_to_time;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::request_for_quote_ack, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static request_for_quote_ack* parse(std::byte* buffer) {
        return reinterpret_cast<request_for_quote_ack*>(buffer);
    }

    // parse method const
    static const request_for_quote_ack* parse(const std::byte* buffer) {
        return reinterpret_cast<const request_for_quote_ack*>(buffer);
    }

};

// layout verification
static_assert(offsetof(request_for_quote_ack::fields_type, seq_num) == 0, "unexpected offset of request_for_quote_ack::fields_type::seq_num");
static_assert(offsetof(request_for_quote_ack::fields_type, uuid) == 4, "unexpected offset of request_for_quote_ack::fields_type::uuid");
static_assert(offsetof(request_for_quote_ack::fields_type, text) == 12, "unexpected offset of request_for_quote_ack::fields_type::text");
static_assert(offsetof(request_for_quote_ack::fields_type, sender_id) == 268, "unexpected offset of request_for_quote_ack::fields_type::sender_id");
static_assert(offsetof(request_for_quote_ack::fields_type, exchange_quote_req_id) == 288, "unexpected offset of request_for_quote_ack::fields_type::exchange_quote_req_id");
static_assert(offsetof(request_for_quote_ack::fields_type, party_details_list_req_id) == 305, "unexpected offset of request_for_quote_ack::fields_type::party_details_list_req_id");
static_assert(offsetof(request_for_quote_ack::fields_type, request_time) == 313, "unexpected offset of request_for_quote_ack::fields_type::request_time");
static_assert(offsetof(request_for_quote_ack::fields_type, sending_time_epoch) == 321, "unexpected offset of request_for_quote_ack::fields_type::sending_time_epoch");
static_assert(offsetof(request_for_quote_ack::fields_type, quote_req_id) == 329, "unexpected offset of request_for_quote_ack::fields_type::quote_req_id");
static_assert(offsetof(request_for_quote_ack::fields_type, location) == 337, "unexpected offset of request_for_quote_ack::fields_type::location");
static_assert(offsetof(request_for_quote_ack::fields_type, quote_reject_reason) == 342, "unexpected offset of request_for_quote_ack::fields_type::quote_reject_reason");
static_assert(offsetof(request_for_quote_ack::fields_type, delay_duration) == 344, "unexpected offset of request_for_quote_ack::fields_type::delay_duration");
static_assert(offsetof(request_for_quote_ack::fields_type, quote_ack_status) == 346, "unexpected offset of request_for_quote_ack::fields_type::quote_ack_status");
static_assert(offsetof(request_for_quote_ack::fields_type, manual_order_indicator) == 347, "unexpected offset of request_for_quote_ack::fields_type::manual_order_indicator");
static_assert(offsetof(request_for_quote_ack::fields_type, split_msg) == 348, "unexpected offset of request_for_quote_ack::fields_type::split_msg");
static_assert(offsetof(request_for_quote_ack::fields_type, poss_retrans_flag) == 349, "unexpected offset of request_for_quote_ack::fields_type::poss_retrans_flag");
static_assert(offsetof(request_for_quote_ack::fields_type, delay_to_time) == 350, "unexpected offset of request_for_quote_ack::fields_type::delay_to_time");
static_assert(sizeof(request_for_quote_ack::fields_type) == 358, "unexpected sizeof request_for_quote_ack::fields_type");
static_assert(sizeof(request_for_quote_ack) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 358, "unexpected sizeof request_for_quote_ack");

#pragma pack(pop)
}
