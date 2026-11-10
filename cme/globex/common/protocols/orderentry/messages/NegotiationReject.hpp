#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Reason.hpp"
#include "../types/Uuid.hpp"
#include "../types/RequestTimestamp.hpp"
#include "../types/ErrorCodes.hpp"
#include "../types/FaultToleranceIndicator.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/EnvironmentIndicator.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Negotiation Reject
struct negotiation_reject {

    struct fields_type {
        sbe_ilink3::reason reason;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::request_timestamp request_timestamp;
        sbe_ilink3::error_codes error_codes;
        sbe_ilink3::fault_tolerance_indicator fault_tolerance_indicator;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::environment_indicator environment_indicator;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::negotiation_reject, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static negotiation_reject* parse(std::byte* buffer) {
        return reinterpret_cast<negotiation_reject*>(buffer);
    }

    // parse method const
    static const negotiation_reject* parse(const std::byte* buffer) {
        return reinterpret_cast<const negotiation_reject*>(buffer);
    }

};

// layout verification
static_assert(offsetof(negotiation_reject::fields_type, reason) == 0, "unexpected offset of negotiation_reject::fields_type::reason");
static_assert(offsetof(negotiation_reject::fields_type, uuid) == 48, "unexpected offset of negotiation_reject::fields_type::uuid");
static_assert(offsetof(negotiation_reject::fields_type, request_timestamp) == 56, "unexpected offset of negotiation_reject::fields_type::request_timestamp");
static_assert(offsetof(negotiation_reject::fields_type, error_codes) == 64, "unexpected offset of negotiation_reject::fields_type::error_codes");
static_assert(offsetof(negotiation_reject::fields_type, fault_tolerance_indicator) == 66, "unexpected offset of negotiation_reject::fields_type::fault_tolerance_indicator");
static_assert(offsetof(negotiation_reject::fields_type, split_msg) == 67, "unexpected offset of negotiation_reject::fields_type::split_msg");
static_assert(offsetof(negotiation_reject::fields_type, environment_indicator) == 68, "unexpected offset of negotiation_reject::fields_type::environment_indicator");
static_assert(sizeof(negotiation_reject::fields_type) == 69, "unexpected sizeof negotiation_reject::fields_type");
static_assert(sizeof(negotiation_reject) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 69, "unexpected sizeof negotiation_reject");

#pragma pack(pop)
}
