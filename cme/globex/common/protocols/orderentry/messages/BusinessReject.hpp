#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/Text.hpp"
#include "../types/SenderIdOptional.hpp"
#include "../types/PartyDetailsListReqIdOptional.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/BusinessRejectRefId.hpp"
#include "../types/LocationOptional.hpp"
#include "../types/RefSeqNum.hpp"
#include "../types/RefTagId.hpp"
#include "../types/BusinessRejectReason.hpp"
#include "../types/RefMsgType.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/ManualOrderIndicatorOptional.hpp"
#include "../types/SplitMsg.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Business Reject
struct business_reject {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::sender_id_optional sender_id_optional;
        sbe_ilink3::party_details_list_req_id_optional party_details_list_req_id_optional;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::business_reject_ref_id business_reject_ref_id;
        sbe_ilink3::location_optional location_optional;
        sbe_ilink3::ref_seq_num ref_seq_num;
        sbe_ilink3::ref_tag_id ref_tag_id;
        sbe_ilink3::business_reject_reason business_reject_reason;
        sbe_ilink3::ref_msg_type ref_msg_type;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::manual_order_indicator_optional manual_order_indicator_optional;
        sbe_ilink3::split_msg split_msg;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::business_reject, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static business_reject* parse(std::byte* buffer) {
        return reinterpret_cast<business_reject*>(buffer);
    }

    // parse method const
    static const business_reject* parse(const std::byte* buffer) {
        return reinterpret_cast<const business_reject*>(buffer);
    }

};

// layout verification
static_assert(offsetof(business_reject::fields_type, seq_num) == 0, "unexpected offset of business_reject::fields_type::seq_num");
static_assert(offsetof(business_reject::fields_type, uuid) == 4, "unexpected offset of business_reject::fields_type::uuid");
static_assert(offsetof(business_reject::fields_type, text) == 12, "unexpected offset of business_reject::fields_type::text");
static_assert(offsetof(business_reject::fields_type, sender_id_optional) == 268, "unexpected offset of business_reject::fields_type::sender_id_optional");
static_assert(offsetof(business_reject::fields_type, party_details_list_req_id_optional) == 288, "unexpected offset of business_reject::fields_type::party_details_list_req_id_optional");
static_assert(offsetof(business_reject::fields_type, sending_time_epoch) == 296, "unexpected offset of business_reject::fields_type::sending_time_epoch");
static_assert(offsetof(business_reject::fields_type, business_reject_ref_id) == 304, "unexpected offset of business_reject::fields_type::business_reject_ref_id");
static_assert(offsetof(business_reject::fields_type, location_optional) == 312, "unexpected offset of business_reject::fields_type::location_optional");
static_assert(offsetof(business_reject::fields_type, ref_seq_num) == 317, "unexpected offset of business_reject::fields_type::ref_seq_num");
static_assert(offsetof(business_reject::fields_type, ref_tag_id) == 321, "unexpected offset of business_reject::fields_type::ref_tag_id");
static_assert(offsetof(business_reject::fields_type, business_reject_reason) == 323, "unexpected offset of business_reject::fields_type::business_reject_reason");
static_assert(offsetof(business_reject::fields_type, ref_msg_type) == 325, "unexpected offset of business_reject::fields_type::ref_msg_type");
static_assert(offsetof(business_reject::fields_type, poss_retrans_flag) == 327, "unexpected offset of business_reject::fields_type::poss_retrans_flag");
static_assert(offsetof(business_reject::fields_type, manual_order_indicator_optional) == 328, "unexpected offset of business_reject::fields_type::manual_order_indicator_optional");
static_assert(offsetof(business_reject::fields_type, split_msg) == 329, "unexpected offset of business_reject::fields_type::split_msg");
static_assert(sizeof(business_reject::fields_type) == 330, "unexpected sizeof business_reject::fields_type");
static_assert(sizeof(business_reject) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 330, "unexpected sizeof business_reject");

#pragma pack(pop)
}
