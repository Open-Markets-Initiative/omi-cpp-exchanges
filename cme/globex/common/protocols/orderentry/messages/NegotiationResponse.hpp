#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/Uuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/SecretKeySecureIdExpiration.hpp"
#include "../types/FaultToleranceIndicator.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/PreviousSeqNo.hpp"
#include "../types/PreviousUuid.hpp"
#include "../types/EnvironmentIndicator.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;


#pragma pack(push, 1)

// Negotiation Response
struct negotiation_response {

    struct fields_type {
        sbe_ilink3::uuid uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::secret_key_secure_id_expiration secret_key_secure_id_expiration;
        sbe_ilink3::fault_tolerance_indicator fault_tolerance_indicator;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::previous_seq_no previous_seq_no;
        sbe_ilink3::previous_uuid previous_uuid;
        sbe_ilink3::environment_indicator environment_indicator;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::negotiation_response, schema_id(8), version(9)};

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
    static negotiation_response* parse(std::byte* buffer) {
        return reinterpret_cast<negotiation_response*>(buffer);
    }

    // parse method const
    static const negotiation_response* parse(const std::byte* buffer) {
        return reinterpret_cast<const negotiation_response*>(buffer);
    }

};

// layout verification
static_assert(offsetof(negotiation_response::fields_type, uuid) == 0, "unexpected offset of negotiation_response::fields_type::uuid");
static_assert(offsetof(negotiation_response::fields_type, request_timestamp) == 8, "unexpected offset of negotiation_response::fields_type::request_timestamp");
static_assert(offsetof(negotiation_response::fields_type, secret_key_secure_id_expiration) == 16, "unexpected offset of negotiation_response::fields_type::secret_key_secure_id_expiration");
static_assert(offsetof(negotiation_response::fields_type, fault_tolerance_indicator) == 18, "unexpected offset of negotiation_response::fields_type::fault_tolerance_indicator");
static_assert(offsetof(negotiation_response::fields_type, split_msg) == 19, "unexpected offset of negotiation_response::fields_type::split_msg");
static_assert(offsetof(negotiation_response::fields_type, previous_seq_no) == 20, "unexpected offset of negotiation_response::fields_type::previous_seq_no");
static_assert(offsetof(negotiation_response::fields_type, previous_uuid) == 24, "unexpected offset of negotiation_response::fields_type::previous_uuid");
static_assert(offsetof(negotiation_response::fields_type, environment_indicator) == 32, "unexpected offset of negotiation_response::fields_type::environment_indicator");
static_assert(sizeof(negotiation_response::fields_type) == 33, "unexpected sizeof negotiation_response::fields_type");

#pragma pack(pop)
}

#include "details/NegotiationResponseWriter.hpp"
#include "details/NegotiationResponseReader.hpp"
