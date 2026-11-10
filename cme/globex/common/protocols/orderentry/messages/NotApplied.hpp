#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Uuid.hpp"
#include "../types/FromSeqNo.hpp"
#include "../types/MsgCount.hpp"
#include "../types/SplitMsg.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Not Applied
struct not_applied {

    struct fields_type {
        sbe_ilink3::uuid uuid;
        sbe_ilink3::from_seq_no from_seq_no;
        sbe_ilink3::msg_count msg_count;
        sbe_ilink3::split_msg split_msg;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::not_applied, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static not_applied* parse(std::byte* buffer) {
        return reinterpret_cast<not_applied*>(buffer);
    }

    // parse method const
    static const not_applied* parse(const std::byte* buffer) {
        return reinterpret_cast<const not_applied*>(buffer);
    }

};

// layout verification
static_assert(offsetof(not_applied::fields_type, uuid) == 0, "unexpected offset of not_applied::fields_type::uuid");
static_assert(offsetof(not_applied::fields_type, from_seq_no) == 8, "unexpected offset of not_applied::fields_type::from_seq_no");
static_assert(offsetof(not_applied::fields_type, msg_count) == 12, "unexpected offset of not_applied::fields_type::msg_count");
static_assert(offsetof(not_applied::fields_type, split_msg) == 16, "unexpected offset of not_applied::fields_type::split_msg");
static_assert(sizeof(not_applied::fields_type) == 17, "unexpected sizeof not_applied::fields_type");
static_assert(sizeof(not_applied) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 17, "unexpected sizeof not_applied");

#pragma pack(pop)
}
