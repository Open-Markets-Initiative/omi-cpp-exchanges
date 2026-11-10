#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/Memo.hpp"
#include "../types/AvgPxGroupId.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/SelfMatchPreventionId.hpp"
#include "../types/PartyDetailRequestStatus.hpp"
#include "../types/CustOrderCapacity.hpp"
#include "../types/ClearingAccountType.hpp"
#include "../types/SelfMatchPreventionInstruction.hpp"
#include "../types/AvgPxIndicator.hpp"
#include "../types/ClearingTradePriceType.hpp"
#include "../types/CmtaGiveupCd.hpp"
#include "../types/CustOrderHandlingInst.hpp"
#include "../types/ListUpdateAction.hpp"
#include "../types/PartyDetailDefinitionStatus.hpp"
#include "../types/Executor.hpp"
#include "../types/IdmShortCode.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/PartyDetailId.hpp"
#include "../types/PartyDetailRole.hpp"
#include "../structs/GroupSize.hpp"
#include "../types/TrdRegPublicationType.hpp"
#include "../types/TrdRegPublicationReason.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct party_details_definition_request_ack_party_details_groups_entry {
    sbe_ilink3::party_detail_id party_detail_id;
    sbe_ilink3::party_detail_role party_detail_role;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct party_details_definition_request_ack_trd_reg_publications_groups_entry {
    sbe_ilink3::trd_reg_publication_type trd_reg_publication_type;
    sbe_ilink3::trd_reg_publication_reason trd_reg_publication_reason;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Party Details Definition Request Ack
struct party_details_definition_request_ack {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::memo memo;
        sbe_ilink3::avg_px_group_id avg_px_group_id;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::self_match_prevention_id self_match_prevention_id;
        sbe_ilink3::party_detail_request_status party_detail_request_status;
        sbe_ilink3::cust_order_capacity cust_order_capacity;
        sbe_ilink3::clearing_account_type clearing_account_type;
        sbe_ilink3::self_match_prevention_instruction self_match_prevention_instruction;
        sbe_ilink3::avg_px_indicator avg_px_indicator;
        sbe_ilink3::clearing_trade_price_type clearing_trade_price_type;
        sbe_ilink3::cmta_giveup_cd cmta_giveup_cd;
        sbe_ilink3::cust_order_handling_inst cust_order_handling_inst;
        sbe_ilink3::list_update_action list_update_action;
        sbe_ilink3::party_detail_definition_status party_detail_definition_status;
        sbe_ilink3::executor executor;
        sbe_ilink3::idm_short_code idm_short_code;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::split_msg split_msg;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::party_details_definition_request_ack, schema_id(8), version(9)};

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
    sbe_group_iterator<party_details_definition_request_ack_party_details_groups_entry, group_size> party_details_groups() const {
        return { tail_begin(), tail_end() };
    }

    sbe_group_iterator<party_details_definition_request_ack_trd_reg_publications_groups_entry, group_size> trd_reg_publications_groups(const std::byte* position) const {
        return { position, tail_end() };
    }


    // parse method
    static party_details_definition_request_ack* parse(std::byte* buffer) {
        return reinterpret_cast<party_details_definition_request_ack*>(buffer);
    }

    // parse method const
    static const party_details_definition_request_ack* parse(const std::byte* buffer) {
        return reinterpret_cast<const party_details_definition_request_ack*>(buffer);
    }

};

// layout verification
static_assert(offsetof(party_details_definition_request_ack::fields_type, seq_num) == 0, "unexpected offset of party_details_definition_request_ack::fields_type::seq_num");
static_assert(offsetof(party_details_definition_request_ack::fields_type, uuid) == 4, "unexpected offset of party_details_definition_request_ack::fields_type::uuid");
static_assert(offsetof(party_details_definition_request_ack::fields_type, memo) == 12, "unexpected offset of party_details_definition_request_ack::fields_type::memo");
static_assert(offsetof(party_details_definition_request_ack::fields_type, avg_px_group_id) == 87, "unexpected offset of party_details_definition_request_ack::fields_type::avg_px_group_id");
static_assert(offsetof(party_details_definition_request_ack::fields_type, party_details_list_req_id) == 107, "unexpected offset of party_details_definition_request_ack::fields_type::party_details_list_req_id");
static_assert(offsetof(party_details_definition_request_ack::fields_type, sending_time_epoch) == 115, "unexpected offset of party_details_definition_request_ack::fields_type::sending_time_epoch");
static_assert(offsetof(party_details_definition_request_ack::fields_type, self_match_prevention_id) == 123, "unexpected offset of party_details_definition_request_ack::fields_type::self_match_prevention_id");
static_assert(offsetof(party_details_definition_request_ack::fields_type, party_detail_request_status) == 131, "unexpected offset of party_details_definition_request_ack::fields_type::party_detail_request_status");
static_assert(offsetof(party_details_definition_request_ack::fields_type, cust_order_capacity) == 132, "unexpected offset of party_details_definition_request_ack::fields_type::cust_order_capacity");
static_assert(offsetof(party_details_definition_request_ack::fields_type, clearing_account_type) == 133, "unexpected offset of party_details_definition_request_ack::fields_type::clearing_account_type");
static_assert(offsetof(party_details_definition_request_ack::fields_type, self_match_prevention_instruction) == 134, "unexpected offset of party_details_definition_request_ack::fields_type::self_match_prevention_instruction");
static_assert(offsetof(party_details_definition_request_ack::fields_type, avg_px_indicator) == 135, "unexpected offset of party_details_definition_request_ack::fields_type::avg_px_indicator");
static_assert(offsetof(party_details_definition_request_ack::fields_type, clearing_trade_price_type) == 136, "unexpected offset of party_details_definition_request_ack::fields_type::clearing_trade_price_type");
static_assert(offsetof(party_details_definition_request_ack::fields_type, cmta_giveup_cd) == 137, "unexpected offset of party_details_definition_request_ack::fields_type::cmta_giveup_cd");
static_assert(offsetof(party_details_definition_request_ack::fields_type, cust_order_handling_inst) == 138, "unexpected offset of party_details_definition_request_ack::fields_type::cust_order_handling_inst");
static_assert(offsetof(party_details_definition_request_ack::fields_type, list_update_action) == 139, "unexpected offset of party_details_definition_request_ack::fields_type::list_update_action");
static_assert(offsetof(party_details_definition_request_ack::fields_type, party_detail_definition_status) == 140, "unexpected offset of party_details_definition_request_ack::fields_type::party_detail_definition_status");
static_assert(offsetof(party_details_definition_request_ack::fields_type, executor) == 141, "unexpected offset of party_details_definition_request_ack::fields_type::executor");
static_assert(offsetof(party_details_definition_request_ack::fields_type, idm_short_code) == 149, "unexpected offset of party_details_definition_request_ack::fields_type::idm_short_code");
static_assert(offsetof(party_details_definition_request_ack::fields_type, poss_retrans_flag) == 157, "unexpected offset of party_details_definition_request_ack::fields_type::poss_retrans_flag");
static_assert(offsetof(party_details_definition_request_ack::fields_type, split_msg) == 158, "unexpected offset of party_details_definition_request_ack::fields_type::split_msg");
static_assert(sizeof(party_details_definition_request_ack::fields_type) == 159, "unexpected sizeof party_details_definition_request_ack::fields_type");

#pragma pack(pop)
}

#include "details/PartyDetailsDefinitionRequestAckWriter.hpp"
#include "details/PartyDetailsDefinitionRequestAckReader.hpp"
