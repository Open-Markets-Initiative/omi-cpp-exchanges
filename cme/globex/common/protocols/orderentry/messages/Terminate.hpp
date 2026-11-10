#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Reason.hpp"
#include "../types/Uuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/ErrorCodes.hpp"
#include "../types/SplitMsg.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Terminate
struct terminate {

    struct fields_type {
        sbe_ilink3::reason reason;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::error_codes error_codes;
        sbe_ilink3::split_msg split_msg;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::terminate, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static terminate* parse(std::byte* buffer) {
        return reinterpret_cast<terminate*>(buffer);
    }

    // parse method const
    static const terminate* parse(const std::byte* buffer) {
        return reinterpret_cast<const terminate*>(buffer);
    }

};

// layout verification
static_assert(offsetof(terminate::fields_type, reason) == 0, "unexpected offset of terminate::fields_type::reason");
static_assert(offsetof(terminate::fields_type, uuid) == 48, "unexpected offset of terminate::fields_type::uuid");
static_assert(offsetof(terminate::fields_type, request_timestamp) == 56, "unexpected offset of terminate::fields_type::request_timestamp");
static_assert(offsetof(terminate::fields_type, error_codes) == 64, "unexpected offset of terminate::fields_type::error_codes");
static_assert(offsetof(terminate::fields_type, split_msg) == 66, "unexpected offset of terminate::fields_type::split_msg");
static_assert(sizeof(terminate::fields_type) == 67, "unexpected sizeof terminate::fields_type");
static_assert(sizeof(terminate) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 67, "unexpected sizeof terminate");

#pragma pack(pop)
}
