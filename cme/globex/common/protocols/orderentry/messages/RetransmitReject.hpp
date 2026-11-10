#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Reason.hpp"
#include "../types/Uuid.hpp"
#include "../types/LastUuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/ErrorCodes.hpp"
#include "../types/SplitMsg.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Retransmit Reject
struct retransmit_reject {

    struct fields_type {
        sbe_ilink3::reason reason;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::last_uuid last_uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::error_codes error_codes;
        sbe_ilink3::split_msg split_msg;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::retransmit_reject, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static retransmit_reject* parse(std::byte* buffer) {
        return reinterpret_cast<retransmit_reject*>(buffer);
    }

    // parse method const
    static const retransmit_reject* parse(const std::byte* buffer) {
        return reinterpret_cast<const retransmit_reject*>(buffer);
    }

};

// layout verification
static_assert(offsetof(retransmit_reject::fields_type, reason) == 0, "unexpected offset of retransmit_reject::fields_type::reason");
static_assert(offsetof(retransmit_reject::fields_type, uuid) == 48, "unexpected offset of retransmit_reject::fields_type::uuid");
static_assert(offsetof(retransmit_reject::fields_type, last_uuid) == 56, "unexpected offset of retransmit_reject::fields_type::last_uuid");
static_assert(offsetof(retransmit_reject::fields_type, request_timestamp) == 64, "unexpected offset of retransmit_reject::fields_type::request_timestamp");
static_assert(offsetof(retransmit_reject::fields_type, error_codes) == 72, "unexpected offset of retransmit_reject::fields_type::error_codes");
static_assert(offsetof(retransmit_reject::fields_type, split_msg) == 74, "unexpected offset of retransmit_reject::fields_type::split_msg");
static_assert(sizeof(retransmit_reject::fields_type) == 75, "unexpected sizeof retransmit_reject::fields_type");
static_assert(sizeof(retransmit_reject) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 75, "unexpected sizeof retransmit_reject");

#pragma pack(pop)
}
