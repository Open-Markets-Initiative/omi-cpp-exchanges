#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/SeqNum.hpp"
#include "../types/RequestingPartyId.hpp"
#include "../types/RequestingPartyIdSource.hpp"
#include "../types/RequestingPartyRole.hpp"
#include "../structs/GroupSize.hpp"
#include "../types/PartyId.hpp"
#include "../types/PartyIdSource.hpp"
#include "../types/PartyRole.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct party_details_list_request_requesting_party_ids_groups_entry {
    sbe_ilink3::requesting_party_id requesting_party_id;
    sbe_ilink3::requesting_party_id_source requesting_party_id_source;
    sbe_ilink3::requesting_party_role requesting_party_role;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct party_details_list_request_party_ids_groups_entry {
    sbe_ilink3::party_id party_id;
    sbe_ilink3::party_id_source party_id_source;
    sbe_ilink3::party_role party_role;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Party Details List Request
struct party_details_list_request {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::seq_num seq_num;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::party_details_list_request, schema_id(8), version(9)};

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
    sbe_group_iterator<party_details_list_request_requesting_party_ids_groups_entry, group_size> requesting_party_ids_groups() const {
        return { tail_begin(), tail_end() };
    }

    sbe_group_iterator<party_details_list_request_party_ids_groups_entry, group_size> party_ids_groups(const std::byte* position) const {
        return { position, tail_end() };
    }


    // parse method
    static party_details_list_request* parse(std::byte* buffer) {
        return reinterpret_cast<party_details_list_request*>(buffer);
    }

    // parse method const
    static const party_details_list_request* parse(const std::byte* buffer) {
        return reinterpret_cast<const party_details_list_request*>(buffer);
    }

};

// layout verification
static_assert(offsetof(party_details_list_request::fields_type, party_details_list_req_id) == 0, "unexpected offset of party_details_list_request::fields_type::party_details_list_req_id");
static_assert(offsetof(party_details_list_request::fields_type, sending_time_epoch) == 8, "unexpected offset of party_details_list_request::fields_type::sending_time_epoch");
static_assert(offsetof(party_details_list_request::fields_type, seq_num) == 16, "unexpected offset of party_details_list_request::fields_type::seq_num");
static_assert(sizeof(party_details_list_request::fields_type) == 20, "unexpected sizeof party_details_list_request::fields_type");

#pragma pack(pop)
}

#include "details/PartyDetailsListRequestWriter.hpp"
#include "details/PartyDetailsListRequestReader.hpp"
