#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/Uuid.hpp"
#include "../types/NextSeqNo.hpp"
#include "../types/FaultToleranceIndicator.hpp"
#include "../types/KeepAliveIntervalLapsed.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Sequence
struct sequence {

    struct fields_type {
        sbe_ilink3::uuid uuid;
        sbe_ilink3::next_seq_no next_seq_no;
        sbe_ilink3::fault_tolerance_indicator fault_tolerance_indicator;
        sbe_ilink3::keep_alive_interval_lapsed keep_alive_interval_lapsed;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::sequence, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static sequence* parse(std::byte* buffer) {
        return reinterpret_cast<sequence*>(buffer);
    }

    // parse method const
    static const sequence* parse(const std::byte* buffer) {
        return reinterpret_cast<const sequence*>(buffer);
    }

};

// layout verification
static_assert(offsetof(sequence::fields_type, uuid) == 0, "unexpected offset of sequence::fields_type::uuid");
static_assert(offsetof(sequence::fields_type, next_seq_no) == 8, "unexpected offset of sequence::fields_type::next_seq_no");
static_assert(offsetof(sequence::fields_type, fault_tolerance_indicator) == 12, "unexpected offset of sequence::fields_type::fault_tolerance_indicator");
static_assert(offsetof(sequence::fields_type, keep_alive_interval_lapsed) == 13, "unexpected offset of sequence::fields_type::keep_alive_interval_lapsed");
static_assert(sizeof(sequence::fields_type) == 14, "unexpected sizeof sequence::fields_type");
static_assert(sizeof(sequence) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 14, "unexpected sizeof sequence");

#pragma pack(pop)
}
