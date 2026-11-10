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
#include "../types/CancelledSymbol.hpp"
#include "../types/Location.hpp"
#include "../types/QuoteId.hpp"
#include "../types/QuoteRejectReason.hpp"
#include "../types/DelayDuration.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/QuoteCxlStatus.hpp"
#include "../types/NoProcessedEntries32.hpp"
#include "../types/MmProtectionReset.hpp"
#include "../types/UnsolicitedCancelType.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/TotNoQuoteEntriesOptional.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/DelayToTime.hpp"
#include "../types/OrigOrderUser.hpp"
#include "../types/CancelText.hpp"
#include "../types/QuoteEntryOpen.hpp"
#include "../types/CxlLinkedSessions.hpp"
#include "../types/QuoteEntryControl.hpp"
#include "../types/QuoteEntryId.hpp"
#include "../types/SecurityId.hpp"
#include "../types/QuoteEntryRejectReason.hpp"
#include "../structs/GroupSize.hpp"
#include "../types/QuoteSetId.hpp"
#include "../types/QuoteErrorCode.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct quote_cancel_ack_entries_groups_entry {
    sbe_ilink3::quote_entry_id quote_entry_id;
    sbe_ilink3::security_id security_id;
    sbe_ilink3::quote_entry_reject_reason quote_entry_reject_reason;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct quote_cancel_ack_sets_groups_entry {
    sbe_ilink3::quote_set_id quote_set_id;
    sbe_ilink3::quote_error_code quote_error_code;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Quote Cancel Ack
struct quote_cancel_ack {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::text text;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::request_time request_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::cancelled_symbol cancelled_symbol;
        sbe_ilink3::location location;
        sbe_ilink3::quote_id quote_id;
        sbe_ilink3::quote_reject_reason quote_reject_reason;
        sbe_ilink3::delay_duration delay_duration;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::quote_cxl_status quote_cxl_status;
        sbe_ilink3::no_processed_entries_32 no_processed_entries_32;
        sbe_ilink3::mm_protection_reset mm_protection_reset;
        sbe_ilink3::unsolicited_cancel_type unsolicited_cancel_type;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::tot_no_quote_entries_optional tot_no_quote_entries_optional;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::delay_to_time delay_to_time;
        sbe_ilink3::orig_order_user orig_order_user;
        sbe_ilink3::cancel_text cancel_text;
        sbe_ilink3::quote_entry_open quote_entry_open;
        sbe_ilink3::cxl_linked_sessions cxl_linked_sessions;
        sbe_ilink3::quote_entry_control quote_entry_control;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::quote_cancel_ack, schema_id(8), version(9)};

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
    sbe_group_iterator<quote_cancel_ack_entries_groups_entry, group_size> quote_cancel_ack_entries_groups() const {
        return { tail_begin(), tail_end() };
    }

    sbe_group_iterator<quote_cancel_ack_sets_groups_entry, group_size> quote_cancel_ack_sets_groups(const std::byte* position) const {
        return { position, tail_end() };
    }


    // parse method
    static quote_cancel_ack* parse(std::byte* buffer) {
        return reinterpret_cast<quote_cancel_ack*>(buffer);
    }

    // parse method const
    static const quote_cancel_ack* parse(const std::byte* buffer) {
        return reinterpret_cast<const quote_cancel_ack*>(buffer);
    }

};

// layout verification
static_assert(offsetof(quote_cancel_ack::fields_type, seq_num) == 0, "unexpected offset of quote_cancel_ack::fields_type::seq_num");
static_assert(offsetof(quote_cancel_ack::fields_type, uuid) == 4, "unexpected offset of quote_cancel_ack::fields_type::uuid");
static_assert(offsetof(quote_cancel_ack::fields_type, text) == 12, "unexpected offset of quote_cancel_ack::fields_type::text");
static_assert(offsetof(quote_cancel_ack::fields_type, sender_id) == 268, "unexpected offset of quote_cancel_ack::fields_type::sender_id");
static_assert(offsetof(quote_cancel_ack::fields_type, party_details_list_req_id) == 288, "unexpected offset of quote_cancel_ack::fields_type::party_details_list_req_id");
static_assert(offsetof(quote_cancel_ack::fields_type, request_time) == 296, "unexpected offset of quote_cancel_ack::fields_type::request_time");
static_assert(offsetof(quote_cancel_ack::fields_type, sending_time_epoch) == 304, "unexpected offset of quote_cancel_ack::fields_type::sending_time_epoch");
static_assert(offsetof(quote_cancel_ack::fields_type, cancelled_symbol) == 312, "unexpected offset of quote_cancel_ack::fields_type::cancelled_symbol");
static_assert(offsetof(quote_cancel_ack::fields_type, location) == 318, "unexpected offset of quote_cancel_ack::fields_type::location");
static_assert(offsetof(quote_cancel_ack::fields_type, quote_id) == 323, "unexpected offset of quote_cancel_ack::fields_type::quote_id");
static_assert(offsetof(quote_cancel_ack::fields_type, quote_reject_reason) == 327, "unexpected offset of quote_cancel_ack::fields_type::quote_reject_reason");
static_assert(offsetof(quote_cancel_ack::fields_type, delay_duration) == 329, "unexpected offset of quote_cancel_ack::fields_type::delay_duration");
static_assert(offsetof(quote_cancel_ack::fields_type, manual_order_indicator) == 331, "unexpected offset of quote_cancel_ack::fields_type::manual_order_indicator");
static_assert(offsetof(quote_cancel_ack::fields_type, quote_cxl_status) == 332, "unexpected offset of quote_cancel_ack::fields_type::quote_cxl_status");
static_assert(offsetof(quote_cancel_ack::fields_type, no_processed_entries_32) == 333, "unexpected offset of quote_cancel_ack::fields_type::no_processed_entries_32");
static_assert(offsetof(quote_cancel_ack::fields_type, mm_protection_reset) == 337, "unexpected offset of quote_cancel_ack::fields_type::mm_protection_reset");
static_assert(offsetof(quote_cancel_ack::fields_type, unsolicited_cancel_type) == 338, "unexpected offset of quote_cancel_ack::fields_type::unsolicited_cancel_type");
static_assert(offsetof(quote_cancel_ack::fields_type, split_msg) == 339, "unexpected offset of quote_cancel_ack::fields_type::split_msg");
static_assert(offsetof(quote_cancel_ack::fields_type, tot_no_quote_entries_optional) == 340, "unexpected offset of quote_cancel_ack::fields_type::tot_no_quote_entries_optional");
static_assert(offsetof(quote_cancel_ack::fields_type, liquidity_flag) == 341, "unexpected offset of quote_cancel_ack::fields_type::liquidity_flag");
static_assert(offsetof(quote_cancel_ack::fields_type, poss_retrans_flag) == 342, "unexpected offset of quote_cancel_ack::fields_type::poss_retrans_flag");
static_assert(offsetof(quote_cancel_ack::fields_type, delay_to_time) == 343, "unexpected offset of quote_cancel_ack::fields_type::delay_to_time");
static_assert(offsetof(quote_cancel_ack::fields_type, orig_order_user) == 351, "unexpected offset of quote_cancel_ack::fields_type::orig_order_user");
static_assert(offsetof(quote_cancel_ack::fields_type, cancel_text) == 359, "unexpected offset of quote_cancel_ack::fields_type::cancel_text");
static_assert(offsetof(quote_cancel_ack::fields_type, quote_entry_open) == 367, "unexpected offset of quote_cancel_ack::fields_type::quote_entry_open");
static_assert(offsetof(quote_cancel_ack::fields_type, cxl_linked_sessions) == 368, "unexpected offset of quote_cancel_ack::fields_type::cxl_linked_sessions");
static_assert(offsetof(quote_cancel_ack::fields_type, quote_entry_control) == 369, "unexpected offset of quote_cancel_ack::fields_type::quote_entry_control");
static_assert(sizeof(quote_cancel_ack::fields_type) == 370, "unexpected sizeof quote_cancel_ack::fields_type");

#pragma pack(pop)
}

#include "details/QuoteCancelAckWriter.hpp"
#include "details/QuoteCancelAckReader.hpp"
