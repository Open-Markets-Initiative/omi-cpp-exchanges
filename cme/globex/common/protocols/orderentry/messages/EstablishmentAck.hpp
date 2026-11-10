#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Uuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/NextSeqNo.hpp"
#include "../types/PreviousSeqNo.hpp"
#include "../types/PreviousUuid.hpp"
#include "../types/KeepAliveInterval.hpp"
#include "../types/SecretKeySecureIdExpiration.hpp"
#include "../types/FaultToleranceIndicator.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/EnvironmentIndicator.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Establishment Ack
struct establishment_ack {

    struct fields_type {
        sbe_ilink3::uuid uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::next_seq_no next_seq_no;
        sbe_ilink3::previous_seq_no previous_seq_no;
        sbe_ilink3::previous_uuid previous_uuid;
        sbe_ilink3::keep_alive_interval keep_alive_interval;
        sbe_ilink3::secret_key_secure_id_expiration secret_key_secure_id_expiration;
        sbe_ilink3::fault_tolerance_indicator fault_tolerance_indicator;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::environment_indicator environment_indicator;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::establishment_ack, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static establishment_ack* parse(std::byte* buffer) {
        return reinterpret_cast<establishment_ack*>(buffer);
    }

    // parse method const
    static const establishment_ack* parse(const std::byte* buffer) {
        return reinterpret_cast<const establishment_ack*>(buffer);
    }

};

// layout verification
static_assert(offsetof(establishment_ack::fields_type, uuid) == 0, "unexpected offset of establishment_ack::fields_type::uuid");
static_assert(offsetof(establishment_ack::fields_type, request_timestamp) == 8, "unexpected offset of establishment_ack::fields_type::request_timestamp");
static_assert(offsetof(establishment_ack::fields_type, next_seq_no) == 16, "unexpected offset of establishment_ack::fields_type::next_seq_no");
static_assert(offsetof(establishment_ack::fields_type, previous_seq_no) == 20, "unexpected offset of establishment_ack::fields_type::previous_seq_no");
static_assert(offsetof(establishment_ack::fields_type, previous_uuid) == 24, "unexpected offset of establishment_ack::fields_type::previous_uuid");
static_assert(offsetof(establishment_ack::fields_type, keep_alive_interval) == 32, "unexpected offset of establishment_ack::fields_type::keep_alive_interval");
static_assert(offsetof(establishment_ack::fields_type, secret_key_secure_id_expiration) == 34, "unexpected offset of establishment_ack::fields_type::secret_key_secure_id_expiration");
static_assert(offsetof(establishment_ack::fields_type, fault_tolerance_indicator) == 36, "unexpected offset of establishment_ack::fields_type::fault_tolerance_indicator");
static_assert(offsetof(establishment_ack::fields_type, split_msg) == 37, "unexpected offset of establishment_ack::fields_type::split_msg");
static_assert(offsetof(establishment_ack::fields_type, environment_indicator) == 38, "unexpected offset of establishment_ack::fields_type::environment_indicator");
static_assert(sizeof(establishment_ack::fields_type) == 39, "unexpected sizeof establishment_ack::fields_type");
static_assert(sizeof(establishment_ack) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 39, "unexpected sizeof establishment_ack");

#pragma pack(pop)
}
