#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/HmacSignature.hpp"
#include "../types/AccessKeyId.hpp"
#include "../types/Uuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/Session.hpp"
#include "../types/Firm.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;


#pragma pack(push, 1)

// Negotiate
struct negotiate {

    struct fields_type {
        sbe_ilink3::hmac_signature hmac_signature;
        sbe_ilink3::access_key_id access_key_id;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::session session;
        sbe_ilink3::firm firm;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::negotiate, schema_id(8), version(9)};

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
    sbe_var_data<std::uint16_t> credentials() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static negotiate* parse(std::byte* buffer) {
        return reinterpret_cast<negotiate*>(buffer);
    }

    // parse method const
    static const negotiate* parse(const std::byte* buffer) {
        return reinterpret_cast<const negotiate*>(buffer);
    }

};

// layout verification
static_assert(offsetof(negotiate::fields_type, hmac_signature) == 0, "unexpected offset of negotiate::fields_type::hmac_signature");
static_assert(offsetof(negotiate::fields_type, access_key_id) == 32, "unexpected offset of negotiate::fields_type::access_key_id");
static_assert(offsetof(negotiate::fields_type, uuid) == 52, "unexpected offset of negotiate::fields_type::uuid");
static_assert(offsetof(negotiate::fields_type, request_timestamp) == 60, "unexpected offset of negotiate::fields_type::request_timestamp");
static_assert(offsetof(negotiate::fields_type, session) == 68, "unexpected offset of negotiate::fields_type::session");
static_assert(offsetof(negotiate::fields_type, firm) == 71, "unexpected offset of negotiate::fields_type::firm");
static_assert(sizeof(negotiate::fields_type) == 76, "unexpected sizeof negotiate::fields_type");

#pragma pack(pop)
}

#include "details/NegotiateWriter.hpp"
#include "details/NegotiateReader.hpp"
