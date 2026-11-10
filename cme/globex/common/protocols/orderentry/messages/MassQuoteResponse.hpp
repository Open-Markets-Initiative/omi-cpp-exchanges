#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/Text.hpp"
#include "../types/SenderId.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/TransactTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/QuoteReqIdOptional.hpp"
#include "../types/Location.hpp"
#include "../types/QuoteId.hpp"
#include "../types/QuoteRejectReason.hpp"
#include "../types/QuoteAckStatus.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/NoProcessedEntries.hpp"
#include "../types/MmProtectionReset.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/ShortSaleType.hpp"
#include "../types/TotNoQuoteEntriesOptional.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/DelayToTime.hpp"
#include "../types/QuoteEntryOpen.hpp"
#include "../types/QuoteEntryControl.hpp"
#include "../types/OrderIdOptional.hpp"
#include "../types/QuoteEntryId.hpp"
#include "../types/QuoteSetId.hpp"
#include "../types/SecurityId.hpp"
#include "../types/SideSide.hpp"
#include "../types/QuoteEntryRejectReasonOptional.hpp"
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct mass_quote_response_entries_groups_entry {
    sbe_ilink3::order_id_optional order_id_optional;
    sbe_ilink3::quote_entry_id quote_entry_id;
    sbe_ilink3::quote_set_id quote_set_id;
    sbe_ilink3::security_id security_id;
    sbe_ilink3::side_side side_side;
    sbe_ilink3::quote_entry_reject_reason_optional quote_entry_reject_reason_optional;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Mass Quote Response
struct mass_quote_response {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::transact_time transact_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::quote_req_id_optional quote_req_id_optional;
        sbe_ilink3::location location;
        sbe_ilink3::quote_id quote_id;
        sbe_ilink3::quote_reject_reason quote_reject_reason;
        sbe_ilink3::quote_ack_status quote_ack_status;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::no_processed_entries no_processed_entries;
        sbe_ilink3::mm_protection_reset mm_protection_reset;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::short_sale_type short_sale_type;
        sbe_ilink3::tot_no_quote_entries_optional tot_no_quote_entries_optional;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::delay_to_time delay_to_time;
        sbe_ilink3::quote_entry_open quote_entry_open;
        sbe_ilink3::quote_entry_control quote_entry_control;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::mass_quote_response, schema_id(8), version(9)};

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
    sbe_group_iterator<mass_quote_response_entries_groups_entry, group_size> mass_quote_response_entries_groups() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static mass_quote_response* parse(std::byte* buffer) {
        return reinterpret_cast<mass_quote_response*>(buffer);
    }

    // parse method const
    static const mass_quote_response* parse(const std::byte* buffer) {
        return reinterpret_cast<const mass_quote_response*>(buffer);
    }

};

// layout verification
static_assert(offsetof(mass_quote_response::fields_type, seq_num) == 0, "unexpected offset of mass_quote_response::fields_type::seq_num");
static_assert(offsetof(mass_quote_response::fields_type, uuid) == 4, "unexpected offset of mass_quote_response::fields_type::uuid");
static_assert(offsetof(mass_quote_response::fields_type, text) == 12, "unexpected offset of mass_quote_response::fields_type::text");
static_assert(offsetof(mass_quote_response::fields_type, sender_id) == 268, "unexpected offset of mass_quote_response::fields_type::sender_id");
static_assert(offsetof(mass_quote_response::fields_type, party_details_list_req_id) == 288, "unexpected offset of mass_quote_response::fields_type::party_details_list_req_id");
static_assert(offsetof(mass_quote_response::fields_type, transact_time) == 296, "unexpected offset of mass_quote_response::fields_type::transact_time");
static_assert(offsetof(mass_quote_response::fields_type, sending_time_epoch) == 304, "unexpected offset of mass_quote_response::fields_type::sending_time_epoch");
static_assert(offsetof(mass_quote_response::fields_type, quote_req_id_optional) == 312, "unexpected offset of mass_quote_response::fields_type::quote_req_id_optional");
static_assert(offsetof(mass_quote_response::fields_type, location) == 320, "unexpected offset of mass_quote_response::fields_type::location");
static_assert(offsetof(mass_quote_response::fields_type, quote_id) == 325, "unexpected offset of mass_quote_response::fields_type::quote_id");
static_assert(offsetof(mass_quote_response::fields_type, quote_reject_reason) == 329, "unexpected offset of mass_quote_response::fields_type::quote_reject_reason");
static_assert(offsetof(mass_quote_response::fields_type, quote_ack_status) == 331, "unexpected offset of mass_quote_response::fields_type::quote_ack_status");
static_assert(offsetof(mass_quote_response::fields_type, manual_order_indicator) == 332, "unexpected offset of mass_quote_response::fields_type::manual_order_indicator");
static_assert(offsetof(mass_quote_response::fields_type, no_processed_entries) == 333, "unexpected offset of mass_quote_response::fields_type::no_processed_entries");
static_assert(offsetof(mass_quote_response::fields_type, mm_protection_reset) == 334, "unexpected offset of mass_quote_response::fields_type::mm_protection_reset");
static_assert(offsetof(mass_quote_response::fields_type, split_msg) == 335, "unexpected offset of mass_quote_response::fields_type::split_msg");
static_assert(offsetof(mass_quote_response::fields_type, liquidity_flag) == 336, "unexpected offset of mass_quote_response::fields_type::liquidity_flag");
static_assert(offsetof(mass_quote_response::fields_type, short_sale_type) == 337, "unexpected offset of mass_quote_response::fields_type::short_sale_type");
static_assert(offsetof(mass_quote_response::fields_type, tot_no_quote_entries_optional) == 338, "unexpected offset of mass_quote_response::fields_type::tot_no_quote_entries_optional");
static_assert(offsetof(mass_quote_response::fields_type, poss_retrans_flag) == 339, "unexpected offset of mass_quote_response::fields_type::poss_retrans_flag");
static_assert(offsetof(mass_quote_response::fields_type, delay_to_time) == 340, "unexpected offset of mass_quote_response::fields_type::delay_to_time");
static_assert(offsetof(mass_quote_response::fields_type, quote_entry_open) == 348, "unexpected offset of mass_quote_response::fields_type::quote_entry_open");
static_assert(offsetof(mass_quote_response::fields_type, quote_entry_control) == 349, "unexpected offset of mass_quote_response::fields_type::quote_entry_control");
static_assert(sizeof(mass_quote_response::fields_type) == 350, "unexpected sizeof mass_quote_response::fields_type");

#pragma pack(pop)
}

#include "details/MassQuoteResponseWriter.hpp"
#include "details/MassQuoteResponseReader.hpp"
