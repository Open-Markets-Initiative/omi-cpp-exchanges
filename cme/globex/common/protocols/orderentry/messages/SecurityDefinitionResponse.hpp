#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/Text.hpp"
#include "../types/FinancialInstrumentFullName.hpp"
#include "../types/SenderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/SecurityReqId.hpp"
#include "../types/SecurityResponseId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/SecurityGroup.hpp"
#include "../types/SecurityType.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityIdOptional.hpp"
#include "../types/Currency.hpp"
#include "../structs/MaturityMonthYear.hpp"
#include "../types/DelayDuration.hpp"
#include "../types/StartDate.hpp"
#include "../types/EndDate.hpp"
#include "../types/MaxNoOfSubstitutions.hpp"
#include "../types/SourceRepoId.hpp"
#include "../types/TerminationType.hpp"
#include "../types/SecurityResponseType.hpp"
#include "../types/ExpirationCycle.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/AutoQuoteRequest.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/BrokenDateTermType.hpp"
#include "../types/LegPrice.hpp"
#include "../structs/LegOptionDelta.hpp"
#include "../types/LegSecurityId.hpp"
#include "../types/LegSide.hpp"
#include "../types/LegRatioQty.hpp"
#include "../structs/GroupSize.hpp"
#include "../types/BrokenDateGuid.hpp"
#include "../types/BrokenDateSecurityId.hpp"
#include "../types/BrokenDateStart.hpp"
#include "../types/BrokenDateEnd.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct security_definition_response_response_legs_groups_entry {
    sbe_ilink3::leg_price leg_price;
    sbe_ilink3::leg_option_delta leg_option_delta;
    sbe_ilink3::leg_security_id leg_security_id;
    sbe_ilink3::leg_side leg_side;
    sbe_ilink3::leg_ratio_qty leg_ratio_qty;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct security_definition_response_broken_dates_response_groups_entry {
    sbe_ilink3::broken_date_guid broken_date_guid;
    sbe_ilink3::broken_date_security_id broken_date_security_id;
    sbe_ilink3::broken_date_start broken_date_start;
    sbe_ilink3::broken_date_end broken_date_end;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Security Definition Response
struct security_definition_response {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::financial_instrument_full_name financial_instrument_full_name;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::symbol symbol;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::security_req_id security_req_id;
        sbe_ilink3::security_response_id security_response_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::security_group security_group;
        sbe_ilink3::security_type security_type;
        sbe_ilink3::location location;
        sbe_ilink3::security_id_optional security_id_optional;
        sbe_ilink3::currency currency;
        sbe_ilink3::maturity_month_year maturity_month_year;
        sbe_ilink3::delay_duration delay_duration;
        sbe_ilink3::start_date start_date;
        sbe_ilink3::end_date end_date;
        sbe_ilink3::max_no_of_substitutions max_no_of_substitutions;
        sbe_ilink3::source_repo_id source_repo_id;
        sbe_ilink3::termination_type termination_type;
        sbe_ilink3::security_response_type security_response_type;
        sbe_ilink3::expiration_cycle expiration_cycle;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::auto_quote_request auto_quote_request;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::broken_date_term_type broken_date_term_type;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::security_definition_response, schema_id(8), version(9)};

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
    sbe_group_iterator<security_definition_response_response_legs_groups_entry, group_size> response_legs_groups() const {
        return { tail_begin(), tail_end() };
    }

    sbe_group_iterator<security_definition_response_broken_dates_response_groups_entry, group_size> broken_dates_response_groups(const std::byte* position) const {
        return { position, tail_end() };
    }


    // parse method
    static security_definition_response* parse(std::byte* buffer) {
        return reinterpret_cast<security_definition_response*>(buffer);
    }

    // parse method const
    static const security_definition_response* parse(const std::byte* buffer) {
        return reinterpret_cast<const security_definition_response*>(buffer);
    }

};

// layout verification
static_assert(offsetof(security_definition_response::fields_type, seq_num) == 0, "unexpected offset of security_definition_response::fields_type::seq_num");
static_assert(offsetof(security_definition_response::fields_type, uuid) == 4, "unexpected offset of security_definition_response::fields_type::uuid");
static_assert(offsetof(security_definition_response::fields_type, text) == 12, "unexpected offset of security_definition_response::fields_type::text");
static_assert(offsetof(security_definition_response::fields_type, financial_instrument_full_name) == 268, "unexpected offset of security_definition_response::fields_type::financial_instrument_full_name");
static_assert(offsetof(security_definition_response::fields_type, sender_id) == 303, "unexpected offset of security_definition_response::fields_type::sender_id");
static_assert(offsetof(security_definition_response::fields_type, symbol) == 323, "unexpected offset of security_definition_response::fields_type::symbol");
static_assert(offsetof(security_definition_response::fields_type, party_details_list_req_id) == 343, "unexpected offset of security_definition_response::fields_type::party_details_list_req_id");
static_assert(offsetof(security_definition_response::fields_type, security_req_id) == 351, "unexpected offset of security_definition_response::fields_type::security_req_id");
static_assert(offsetof(security_definition_response::fields_type, security_response_id) == 359, "unexpected offset of security_definition_response::fields_type::security_response_id");
static_assert(offsetof(security_definition_response::fields_type, sending_time_epoch) == 367, "unexpected offset of security_definition_response::fields_type::sending_time_epoch");
static_assert(offsetof(security_definition_response::fields_type, security_group) == 375, "unexpected offset of security_definition_response::fields_type::security_group");
static_assert(offsetof(security_definition_response::fields_type, security_type) == 381, "unexpected offset of security_definition_response::fields_type::security_type");
static_assert(offsetof(security_definition_response::fields_type, location) == 387, "unexpected offset of security_definition_response::fields_type::location");
static_assert(offsetof(security_definition_response::fields_type, security_id_optional) == 392, "unexpected offset of security_definition_response::fields_type::security_id_optional");
static_assert(offsetof(security_definition_response::fields_type, currency) == 396, "unexpected offset of security_definition_response::fields_type::currency");
static_assert(offsetof(security_definition_response::fields_type, maturity_month_year) == 399, "unexpected offset of security_definition_response::fields_type::maturity_month_year");
static_assert(offsetof(security_definition_response::fields_type, delay_duration) == 404, "unexpected offset of security_definition_response::fields_type::delay_duration");
static_assert(offsetof(security_definition_response::fields_type, start_date) == 406, "unexpected offset of security_definition_response::fields_type::start_date");
static_assert(offsetof(security_definition_response::fields_type, end_date) == 408, "unexpected offset of security_definition_response::fields_type::end_date");
static_assert(offsetof(security_definition_response::fields_type, max_no_of_substitutions) == 410, "unexpected offset of security_definition_response::fields_type::max_no_of_substitutions");
static_assert(offsetof(security_definition_response::fields_type, source_repo_id) == 411, "unexpected offset of security_definition_response::fields_type::source_repo_id");
static_assert(offsetof(security_definition_response::fields_type, termination_type) == 415, "unexpected offset of security_definition_response::fields_type::termination_type");
static_assert(offsetof(security_definition_response::fields_type, security_response_type) == 423, "unexpected offset of security_definition_response::fields_type::security_response_type");
static_assert(offsetof(security_definition_response::fields_type, expiration_cycle) == 424, "unexpected offset of security_definition_response::fields_type::expiration_cycle");
static_assert(offsetof(security_definition_response::fields_type, manual_order_indicator) == 425, "unexpected offset of security_definition_response::fields_type::manual_order_indicator");
static_assert(offsetof(security_definition_response::fields_type, split_msg) == 426, "unexpected offset of security_definition_response::fields_type::split_msg");
static_assert(offsetof(security_definition_response::fields_type, auto_quote_request) == 427, "unexpected offset of security_definition_response::fields_type::auto_quote_request");
static_assert(offsetof(security_definition_response::fields_type, poss_retrans_flag) == 428, "unexpected offset of security_definition_response::fields_type::poss_retrans_flag");
static_assert(offsetof(security_definition_response::fields_type, broken_date_term_type) == 429, "unexpected offset of security_definition_response::fields_type::broken_date_term_type");
static_assert(sizeof(security_definition_response::fields_type) == 430, "unexpected sizeof security_definition_response::fields_type");

#pragma pack(pop)
}

#include "details/SecurityDefinitionResponseWriter.hpp"
#include "details/SecurityDefinitionResponseReader.hpp"
