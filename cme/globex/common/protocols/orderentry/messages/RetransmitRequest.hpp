#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Uuid.hpp"
#include "../types/LastUuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/FromSeqNo.hpp"
#include "../types/MsgCount16.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Retransmit Request
struct retransmit_request {

    struct fields_type {
        sbe_ilink3::uuid uuid;
        sbe_ilink3::last_uuid last_uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::from_seq_no from_seq_no;
        sbe_ilink3::msg_count_16 msg_count_16;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::retransmit_request, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static retransmit_request* parse(std::byte* buffer) {
        return reinterpret_cast<retransmit_request*>(buffer);
    }

    // parse method const
    static const retransmit_request* parse(const std::byte* buffer) {
        return reinterpret_cast<const retransmit_request*>(buffer);
    }

};

// layout verification
static_assert(offsetof(retransmit_request::fields_type, uuid) == 0, "unexpected offset of retransmit_request::fields_type::uuid");
static_assert(offsetof(retransmit_request::fields_type, last_uuid) == 8, "unexpected offset of retransmit_request::fields_type::last_uuid");
static_assert(offsetof(retransmit_request::fields_type, request_timestamp) == 16, "unexpected offset of retransmit_request::fields_type::request_timestamp");
static_assert(offsetof(retransmit_request::fields_type, from_seq_no) == 24, "unexpected offset of retransmit_request::fields_type::from_seq_no");
static_assert(offsetof(retransmit_request::fields_type, msg_count_16) == 28, "unexpected offset of retransmit_request::fields_type::msg_count_16");
static_assert(sizeof(retransmit_request::fields_type) == 30, "unexpected sizeof retransmit_request::fields_type");
static_assert(sizeof(retransmit_request) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 30, "unexpected sizeof retransmit_request");

#pragma pack(pop)
}
