#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/HmacSignature.hpp"
#include "../types/AccessKeyId.hpp"
#include "../types/TradingSystemName.hpp"
#include "../types/TradingSystemVersion.hpp"
#include "../types/TradingSystemVendor.hpp"
#include "../types/Uuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/NextSeqNo.hpp"
#include "../types/Session.hpp"
#include "../types/Firm.hpp"
#include "../types/KeepAliveInterval.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;


#pragma pack(push, 1)

// Establish
struct establish {

    struct fields_type {
        sbe_ilink3::hmac_signature hmac_signature;
        sbe_ilink3::access_key_id access_key_id;
        sbe_ilink3::trading_system_name trading_system_name;
        sbe_ilink3::trading_system_version trading_system_version;
        sbe_ilink3::trading_system_vendor trading_system_vendor;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::next_seq_no next_seq_no;
        sbe_ilink3::session session;
        sbe_ilink3::firm firm;
        sbe_ilink3::keep_alive_interval keep_alive_interval;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::establish, schema_id(8), version(9)};

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
    static establish* parse(std::byte* buffer) {
        return reinterpret_cast<establish*>(buffer);
    }

    // parse method const
    static const establish* parse(const std::byte* buffer) {
        return reinterpret_cast<const establish*>(buffer);
    }

};

// layout verification
static_assert(offsetof(establish::fields_type, hmac_signature) == 0, "unexpected offset of establish::fields_type::hmac_signature");
static_assert(offsetof(establish::fields_type, access_key_id) == 32, "unexpected offset of establish::fields_type::access_key_id");
static_assert(offsetof(establish::fields_type, trading_system_name) == 52, "unexpected offset of establish::fields_type::trading_system_name");
static_assert(offsetof(establish::fields_type, trading_system_version) == 82, "unexpected offset of establish::fields_type::trading_system_version");
static_assert(offsetof(establish::fields_type, trading_system_vendor) == 92, "unexpected offset of establish::fields_type::trading_system_vendor");
static_assert(offsetof(establish::fields_type, uuid) == 102, "unexpected offset of establish::fields_type::uuid");
static_assert(offsetof(establish::fields_type, request_timestamp) == 110, "unexpected offset of establish::fields_type::request_timestamp");
static_assert(offsetof(establish::fields_type, next_seq_no) == 118, "unexpected offset of establish::fields_type::next_seq_no");
static_assert(offsetof(establish::fields_type, session) == 122, "unexpected offset of establish::fields_type::session");
static_assert(offsetof(establish::fields_type, firm) == 125, "unexpected offset of establish::fields_type::firm");
static_assert(offsetof(establish::fields_type, keep_alive_interval) == 130, "unexpected offset of establish::fields_type::keep_alive_interval");
static_assert(sizeof(establish::fields_type) == 132, "unexpected sizeof establish::fields_type");

#pragma pack(pop)
}

#include "details/EstablishWriter.hpp"
#include "details/EstablishReader.hpp"
