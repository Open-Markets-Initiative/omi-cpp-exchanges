#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderId.hpp"
#include "../types/QuoteReqIdOptional.hpp"
#include "../types/Location.hpp"
#include "../types/QuoteId.hpp"
#include "../types/TotNoQuoteEntries.hpp"
#include "../types/MmProtectionReset.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/ShortSaleType.hpp"
#include "../types/Unused30.hpp"
#include "../types/Future30.hpp"
#include "../types/QuoteEntryOpen.hpp"
#include "../types/BidPx.hpp"
#include "../types/OfferPx.hpp"
#include "../types/QuoteEntryId.hpp"
#include "../types/SecurityId.hpp"
#include "../types/BidSize.hpp"
#include "../types/OfferSize.hpp"
#include "../types/UnderlyingSecurityId.hpp"
#include "../types/QuoteSetId.hpp"
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct mass_quote_quote_entries_groups_entry {
    sbe_ilink3::bid_px bid_px;
    sbe_ilink3::offer_px offer_px;
    sbe_ilink3::quote_entry_id quote_entry_id;
    sbe_ilink3::security_id security_id;
    sbe_ilink3::bid_size bid_size;
    sbe_ilink3::offer_size offer_size;
    sbe_ilink3::underlying_security_id underlying_security_id;
    sbe_ilink3::quote_set_id quote_set_id;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Mass Quote
struct mass_quote {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::quote_req_id_optional quote_req_id_optional;
        sbe_ilink3::location location;
        sbe_ilink3::quote_id quote_id;
        sbe_ilink3::tot_no_quote_entries tot_no_quote_entries;
        sbe_ilink3::mm_protection_reset mm_protection_reset;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::short_sale_type short_sale_type;
        sbe_ilink3::unused_30 unused_30;
        sbe_ilink3::future_30 future_30;
        sbe_ilink3::quote_entry_open quote_entry_open;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::mass_quote, schema_id(8), version(9)};

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
    sbe_group_iterator<mass_quote_quote_entries_groups_entry, group_size> quote_entries_groups() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static mass_quote* parse(std::byte* buffer) {
        return reinterpret_cast<mass_quote*>(buffer);
    }

    // parse method const
    static const mass_quote* parse(const std::byte* buffer) {
        return reinterpret_cast<const mass_quote*>(buffer);
    }

};

// layout verification
static_assert(offsetof(mass_quote::fields_type, party_details_list_req_id) == 0, "unexpected offset of mass_quote::fields_type::party_details_list_req_id");
static_assert(offsetof(mass_quote::fields_type, sending_time_epoch) == 8, "unexpected offset of mass_quote::fields_type::sending_time_epoch");
static_assert(offsetof(mass_quote::fields_type, manual_order_indicator) == 16, "unexpected offset of mass_quote::fields_type::manual_order_indicator");
static_assert(offsetof(mass_quote::fields_type, seq_num) == 17, "unexpected offset of mass_quote::fields_type::seq_num");
static_assert(offsetof(mass_quote::fields_type, sender_id) == 21, "unexpected offset of mass_quote::fields_type::sender_id");
static_assert(offsetof(mass_quote::fields_type, quote_req_id_optional) == 41, "unexpected offset of mass_quote::fields_type::quote_req_id_optional");
static_assert(offsetof(mass_quote::fields_type, location) == 49, "unexpected offset of mass_quote::fields_type::location");
static_assert(offsetof(mass_quote::fields_type, quote_id) == 54, "unexpected offset of mass_quote::fields_type::quote_id");
static_assert(offsetof(mass_quote::fields_type, tot_no_quote_entries) == 58, "unexpected offset of mass_quote::fields_type::tot_no_quote_entries");
static_assert(offsetof(mass_quote::fields_type, mm_protection_reset) == 59, "unexpected offset of mass_quote::fields_type::mm_protection_reset");
static_assert(offsetof(mass_quote::fields_type, liquidity_flag) == 60, "unexpected offset of mass_quote::fields_type::liquidity_flag");
static_assert(offsetof(mass_quote::fields_type, short_sale_type) == 61, "unexpected offset of mass_quote::fields_type::short_sale_type");
static_assert(offsetof(mass_quote::fields_type, unused_30) == 62, "unexpected offset of mass_quote::fields_type::unused_30");
static_assert(offsetof(mass_quote::fields_type, future_30) == 92, "unexpected offset of mass_quote::fields_type::future_30");
static_assert(offsetof(mass_quote::fields_type, quote_entry_open) == 122, "unexpected offset of mass_quote::fields_type::quote_entry_open");
static_assert(sizeof(mass_quote::fields_type) == 123, "unexpected sizeof mass_quote::fields_type");

#pragma pack(pop)
}

#include "details/MassQuoteWriter.hpp"
#include "details/MassQuoteReader.hpp"
