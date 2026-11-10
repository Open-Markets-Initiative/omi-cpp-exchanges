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
#include "../types/RequestTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/QuoteReqIdOptional.hpp"
#include "../types/Location.hpp"
#include "../types/QuoteId.hpp"
#include "../types/QuoteRejectReason.hpp"
#include "../types/DelayDuration.hpp"
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
#include "../types/QuoteEntryId.hpp"
#include "../types/SecurityId.hpp"
#include "../types/QuoteSetId.hpp"
#include "../types/QuoteEntryRejectReason.hpp"
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct mass_quote_ack_quote_ack_entries_groups_entry {
    sbe_ilink3::quote_entry_id quote_entry_id;
    sbe_ilink3::security_id security_id;
    sbe_ilink3::quote_set_id quote_set_id;
    sbe_ilink3::quote_entry_reject_reason quote_entry_reject_reason;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Mass Quote Ack
struct mass_quote_ack {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::request_time request_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::quote_req_id_optional quote_req_id_optional;
        sbe_ilink3::location location;
        sbe_ilink3::quote_id quote_id;
        sbe_ilink3::quote_reject_reason quote_reject_reason;
        sbe_ilink3::delay_duration delay_duration;
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
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::mass_quote_ack, schema_id(8), version(9)};

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
    sbe_group_iterator<mass_quote_ack_quote_ack_entries_groups_entry, group_size> quote_ack_entries_groups() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static mass_quote_ack* parse(std::byte* buffer) {
        return reinterpret_cast<mass_quote_ack*>(buffer);
    }

    // parse method const
    static const mass_quote_ack* parse(const std::byte* buffer) {
        return reinterpret_cast<const mass_quote_ack*>(buffer);
    }

};

// layout verification
static_assert(offsetof(mass_quote_ack::fields_type, seq_num) == 0, "unexpected offset of mass_quote_ack::fields_type::seq_num");
static_assert(offsetof(mass_quote_ack::fields_type, uuid) == 4, "unexpected offset of mass_quote_ack::fields_type::uuid");
static_assert(offsetof(mass_quote_ack::fields_type, text) == 12, "unexpected offset of mass_quote_ack::fields_type::text");
static_assert(offsetof(mass_quote_ack::fields_type, sender_id) == 268, "unexpected offset of mass_quote_ack::fields_type::sender_id");
static_assert(offsetof(mass_quote_ack::fields_type, party_details_list_req_id) == 288, "unexpected offset of mass_quote_ack::fields_type::party_details_list_req_id");
static_assert(offsetof(mass_quote_ack::fields_type, request_time) == 296, "unexpected offset of mass_quote_ack::fields_type::request_time");
static_assert(offsetof(mass_quote_ack::fields_type, sending_time_epoch) == 304, "unexpected offset of mass_quote_ack::fields_type::sending_time_epoch");
static_assert(offsetof(mass_quote_ack::fields_type, quote_req_id_optional) == 312, "unexpected offset of mass_quote_ack::fields_type::quote_req_id_optional");
static_assert(offsetof(mass_quote_ack::fields_type, location) == 320, "unexpected offset of mass_quote_ack::fields_type::location");
static_assert(offsetof(mass_quote_ack::fields_type, quote_id) == 325, "unexpected offset of mass_quote_ack::fields_type::quote_id");
static_assert(offsetof(mass_quote_ack::fields_type, quote_reject_reason) == 329, "unexpected offset of mass_quote_ack::fields_type::quote_reject_reason");
static_assert(offsetof(mass_quote_ack::fields_type, delay_duration) == 331, "unexpected offset of mass_quote_ack::fields_type::delay_duration");
static_assert(offsetof(mass_quote_ack::fields_type, quote_ack_status) == 333, "unexpected offset of mass_quote_ack::fields_type::quote_ack_status");
static_assert(offsetof(mass_quote_ack::fields_type, manual_order_indicator) == 334, "unexpected offset of mass_quote_ack::fields_type::manual_order_indicator");
static_assert(offsetof(mass_quote_ack::fields_type, no_processed_entries) == 335, "unexpected offset of mass_quote_ack::fields_type::no_processed_entries");
static_assert(offsetof(mass_quote_ack::fields_type, mm_protection_reset) == 336, "unexpected offset of mass_quote_ack::fields_type::mm_protection_reset");
static_assert(offsetof(mass_quote_ack::fields_type, split_msg) == 337, "unexpected offset of mass_quote_ack::fields_type::split_msg");
static_assert(offsetof(mass_quote_ack::fields_type, liquidity_flag) == 338, "unexpected offset of mass_quote_ack::fields_type::liquidity_flag");
static_assert(offsetof(mass_quote_ack::fields_type, short_sale_type) == 339, "unexpected offset of mass_quote_ack::fields_type::short_sale_type");
static_assert(offsetof(mass_quote_ack::fields_type, tot_no_quote_entries_optional) == 340, "unexpected offset of mass_quote_ack::fields_type::tot_no_quote_entries_optional");
static_assert(offsetof(mass_quote_ack::fields_type, poss_retrans_flag) == 341, "unexpected offset of mass_quote_ack::fields_type::poss_retrans_flag");
static_assert(offsetof(mass_quote_ack::fields_type, delay_to_time) == 342, "unexpected offset of mass_quote_ack::fields_type::delay_to_time");
static_assert(offsetof(mass_quote_ack::fields_type, quote_entry_open) == 350, "unexpected offset of mass_quote_ack::fields_type::quote_entry_open");
static_assert(offsetof(mass_quote_ack::fields_type, quote_entry_control) == 351, "unexpected offset of mass_quote_ack::fields_type::quote_entry_control");
static_assert(sizeof(mass_quote_ack::fields_type) == 352, "unexpected sizeof mass_quote_ack::fields_type");

#pragma pack(pop)
}

#include "details/MassQuoteAckWriter.hpp"
#include "details/MassQuoteAckReader.hpp"
