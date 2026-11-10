#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/SecurityReqId.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderIdOptional.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/SecuritySubType.hpp"
#include "../types/Location.hpp"
#include "../types/StartDate.hpp"
#include "../types/EndDate.hpp"
#include "../types/MaxNoOfSubstitutions.hpp"
#include "../types/SourceRepoId.hpp"
#include "../types/BrokenDateTermType.hpp"
#include "../types/LegPrice.hpp"
#include "../types/LegSecurityId.hpp"
#include "../structs/LegOptionDelta.hpp"
#include "../types/LegSide.hpp"
#include "../types/LegRatioQty.hpp"
#include "../structs/GroupSize.hpp"
#include "../types/BrokenDateStart.hpp"
#include "../types/BrokenDateEnd.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct security_definition_request_request_legs_groups_entry {
    sbe_ilink3::leg_price leg_price;
    sbe_ilink3::leg_security_id leg_security_id;
    sbe_ilink3::leg_option_delta leg_option_delta;
    sbe_ilink3::leg_side leg_side;
    sbe_ilink3::leg_ratio_qty leg_ratio_qty;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct security_definition_request_broken_dates_request_groups_entry {
    sbe_ilink3::broken_date_start broken_date_start;
    sbe_ilink3::broken_date_end broken_date_end;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Security Definition Request
struct security_definition_request {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::security_req_id security_req_id;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id_optional sender_id_optional;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::security_sub_type security_sub_type;
        sbe_ilink3::location location;
        sbe_ilink3::start_date start_date;
        sbe_ilink3::end_date end_date;
        sbe_ilink3::max_no_of_substitutions max_no_of_substitutions;
        sbe_ilink3::source_repo_id source_repo_id;
        sbe_ilink3::broken_date_term_type broken_date_term_type;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::security_definition_request, schema_id(8), version(9)};

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
    sbe_group_iterator<security_definition_request_request_legs_groups_entry, group_size> request_legs_groups() const {
        return { tail_begin(), tail_end() };
    }

    sbe_group_iterator<security_definition_request_broken_dates_request_groups_entry, group_size> broken_dates_request_groups(const std::byte* position) const {
        return { position, tail_end() };
    }


    // parse method
    static security_definition_request* parse(std::byte* buffer) {
        return reinterpret_cast<security_definition_request*>(buffer);
    }

    // parse method const
    static const security_definition_request* parse(const std::byte* buffer) {
        return reinterpret_cast<const security_definition_request*>(buffer);
    }

};

// layout verification
static_assert(offsetof(security_definition_request::fields_type, party_details_list_req_id) == 0, "unexpected offset of security_definition_request::fields_type::party_details_list_req_id");
static_assert(offsetof(security_definition_request::fields_type, security_req_id) == 8, "unexpected offset of security_definition_request::fields_type::security_req_id");
static_assert(offsetof(security_definition_request::fields_type, manual_order_indicator) == 16, "unexpected offset of security_definition_request::fields_type::manual_order_indicator");
static_assert(offsetof(security_definition_request::fields_type, seq_num) == 17, "unexpected offset of security_definition_request::fields_type::seq_num");
static_assert(offsetof(security_definition_request::fields_type, sender_id_optional) == 21, "unexpected offset of security_definition_request::fields_type::sender_id_optional");
static_assert(offsetof(security_definition_request::fields_type, sending_time_epoch) == 41, "unexpected offset of security_definition_request::fields_type::sending_time_epoch");
static_assert(offsetof(security_definition_request::fields_type, security_sub_type) == 49, "unexpected offset of security_definition_request::fields_type::security_sub_type");
static_assert(offsetof(security_definition_request::fields_type, location) == 57, "unexpected offset of security_definition_request::fields_type::location");
static_assert(offsetof(security_definition_request::fields_type, start_date) == 62, "unexpected offset of security_definition_request::fields_type::start_date");
static_assert(offsetof(security_definition_request::fields_type, end_date) == 64, "unexpected offset of security_definition_request::fields_type::end_date");
static_assert(offsetof(security_definition_request::fields_type, max_no_of_substitutions) == 66, "unexpected offset of security_definition_request::fields_type::max_no_of_substitutions");
static_assert(offsetof(security_definition_request::fields_type, source_repo_id) == 67, "unexpected offset of security_definition_request::fields_type::source_repo_id");
static_assert(offsetof(security_definition_request::fields_type, broken_date_term_type) == 71, "unexpected offset of security_definition_request::fields_type::broken_date_term_type");
static_assert(sizeof(security_definition_request::fields_type) == 72, "unexpected sizeof security_definition_request::fields_type");

#pragma pack(pop)
}

#include "details/SecurityDefinitionRequestWriter.hpp"
#include "details/SecurityDefinitionRequestReader.hpp"
