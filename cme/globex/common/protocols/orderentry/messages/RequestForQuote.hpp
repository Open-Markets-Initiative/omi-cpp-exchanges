#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/QuoteReqId.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/Location.hpp"
#include "../types/QuoteType.hpp"
#include "../types/SecurityId.hpp"
#include "../types/OrderQtyOptional.hpp"
#include "../types/RfqSide.hpp"
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct request_for_quote_related_sym_groups_entry {
    sbe_ilink3::security_id security_id;
    sbe_ilink3::order_qty_optional order_qty_optional;
    sbe_ilink3::rfq_side rfq_side;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Request For Quote
struct request_for_quote {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::quote_req_id quote_req_id;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::location location;
        sbe_ilink3::quote_type quote_type;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::request_for_quote, schema_id(8), version(9)};

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
    sbe_group_iterator<request_for_quote_related_sym_groups_entry, group_size> related_sym_groups() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static request_for_quote* parse(std::byte* buffer) {
        return reinterpret_cast<request_for_quote*>(buffer);
    }

    // parse method const
    static const request_for_quote* parse(const std::byte* buffer) {
        return reinterpret_cast<const request_for_quote*>(buffer);
    }

};

// layout verification
static_assert(offsetof(request_for_quote::fields_type, party_details_list_req_id) == 0, "unexpected offset of request_for_quote::fields_type::party_details_list_req_id");
static_assert(offsetof(request_for_quote::fields_type, quote_req_id) == 8, "unexpected offset of request_for_quote::fields_type::quote_req_id");
static_assert(offsetof(request_for_quote::fields_type, manual_order_indicator) == 16, "unexpected offset of request_for_quote::fields_type::manual_order_indicator");
static_assert(offsetof(request_for_quote::fields_type, seq_num) == 17, "unexpected offset of request_for_quote::fields_type::seq_num");
static_assert(offsetof(request_for_quote::fields_type, sender_id) == 21, "unexpected offset of request_for_quote::fields_type::sender_id");
static_assert(offsetof(request_for_quote::fields_type, sending_time_epoch) == 41, "unexpected offset of request_for_quote::fields_type::sending_time_epoch");
static_assert(offsetof(request_for_quote::fields_type, location) == 49, "unexpected offset of request_for_quote::fields_type::location");
static_assert(offsetof(request_for_quote::fields_type, quote_type) == 54, "unexpected offset of request_for_quote::fields_type::quote_type");
static_assert(sizeof(request_for_quote::fields_type) == 55, "unexpected sizeof request_for_quote::fields_type");

#pragma pack(pop)
}

#include "details/RequestForQuoteWriter.hpp"
#include "details/RequestForQuoteReader.hpp"
