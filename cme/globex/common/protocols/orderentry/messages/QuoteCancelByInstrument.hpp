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
#include "../types/Location.hpp"
#include "../types/QuoteId.hpp"
#include "../types/CxlLinkedSessions.hpp"
#include "../types/SecurityId.hpp"
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct quote_cancel_by_instrument_entries_groups_entry {
    sbe_ilink3::security_id security_id;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Quote Cancel By Instrument
struct quote_cancel_by_instrument {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::location location;
        sbe_ilink3::quote_id quote_id;
        sbe_ilink3::cxl_linked_sessions cxl_linked_sessions;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::quote_cancel_by_instrument, schema_id(8), version(9)};

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
    sbe_group_iterator<quote_cancel_by_instrument_entries_groups_entry, group_size> quote_cancel_by_instrument_entries_groups() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static quote_cancel_by_instrument* parse(std::byte* buffer) {
        return reinterpret_cast<quote_cancel_by_instrument*>(buffer);
    }

    // parse method const
    static const quote_cancel_by_instrument* parse(const std::byte* buffer) {
        return reinterpret_cast<const quote_cancel_by_instrument*>(buffer);
    }

};

// layout verification
static_assert(offsetof(quote_cancel_by_instrument::fields_type, party_details_list_req_id) == 0, "unexpected offset of quote_cancel_by_instrument::fields_type::party_details_list_req_id");
static_assert(offsetof(quote_cancel_by_instrument::fields_type, sending_time_epoch) == 8, "unexpected offset of quote_cancel_by_instrument::fields_type::sending_time_epoch");
static_assert(offsetof(quote_cancel_by_instrument::fields_type, manual_order_indicator) == 16, "unexpected offset of quote_cancel_by_instrument::fields_type::manual_order_indicator");
static_assert(offsetof(quote_cancel_by_instrument::fields_type, seq_num) == 17, "unexpected offset of quote_cancel_by_instrument::fields_type::seq_num");
static_assert(offsetof(quote_cancel_by_instrument::fields_type, sender_id) == 21, "unexpected offset of quote_cancel_by_instrument::fields_type::sender_id");
static_assert(offsetof(quote_cancel_by_instrument::fields_type, location) == 41, "unexpected offset of quote_cancel_by_instrument::fields_type::location");
static_assert(offsetof(quote_cancel_by_instrument::fields_type, quote_id) == 46, "unexpected offset of quote_cancel_by_instrument::fields_type::quote_id");
static_assert(offsetof(quote_cancel_by_instrument::fields_type, cxl_linked_sessions) == 50, "unexpected offset of quote_cancel_by_instrument::fields_type::cxl_linked_sessions");
static_assert(sizeof(quote_cancel_by_instrument::fields_type) == 51, "unexpected sizeof quote_cancel_by_instrument::fields_type");

#pragma pack(pop)
}

#include "details/QuoteCancelByInstrumentWriter.hpp"
#include "details/QuoteCancelByInstrumentReader.hpp"
