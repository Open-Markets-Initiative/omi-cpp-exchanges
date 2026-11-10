#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Uuid.hpp"
#include "../types/LastUuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/FromSeqNo.hpp"
#include "../types/MsgCount16.hpp"
#include "../types/SplitMsg.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Retransmission
struct retransmission {

    struct fields_type {
        sbe_ilink3::uuid uuid;
        sbe_ilink3::last_uuid last_uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::from_seq_no from_seq_no;
        sbe_ilink3::msg_count_16 msg_count_16;
        sbe_ilink3::split_msg split_msg;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::retransmission, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static retransmission* parse(std::byte* buffer) {
        return reinterpret_cast<retransmission*>(buffer);
    }

    // parse method const
    static const retransmission* parse(const std::byte* buffer) {
        return reinterpret_cast<const retransmission*>(buffer);
    }

};

// layout verification
static_assert(offsetof(retransmission::fields_type, uuid) == 0, "unexpected offset of retransmission::fields_type::uuid");
static_assert(offsetof(retransmission::fields_type, last_uuid) == 8, "unexpected offset of retransmission::fields_type::last_uuid");
static_assert(offsetof(retransmission::fields_type, request_timestamp) == 16, "unexpected offset of retransmission::fields_type::request_timestamp");
static_assert(offsetof(retransmission::fields_type, from_seq_no) == 24, "unexpected offset of retransmission::fields_type::from_seq_no");
static_assert(offsetof(retransmission::fields_type, msg_count_16) == 28, "unexpected offset of retransmission::fields_type::msg_count_16");
static_assert(offsetof(retransmission::fields_type, split_msg) == 30, "unexpected offset of retransmission::fields_type::split_msg");
static_assert(sizeof(retransmission::fields_type) == 31, "unexpected sizeof retransmission::fields_type");
static_assert(sizeof(retransmission) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 31, "unexpected sizeof retransmission");

#pragma pack(pop)
}
