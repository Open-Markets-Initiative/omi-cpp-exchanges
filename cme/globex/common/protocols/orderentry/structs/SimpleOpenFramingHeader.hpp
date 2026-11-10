#pragma once

#include <cstddef>
#include "../types/MessageLength.hpp"
#include "../types/EncodingType.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct simple_open_framing_header {

    sbe_ilink3::message_length message_length;
    sbe_ilink3::encoding_type encoding_type;

    // parse method
    static simple_open_framing_header* parse(std::byte* buffer) {
        return reinterpret_cast<simple_open_framing_header*>(buffer);
    }

    // parse method const
    static const simple_open_framing_header* parse(const std::byte* buffer) {
        return reinterpret_cast<const simple_open_framing_header*>(buffer);
    }
};

// layout verification
static_assert(offsetof(simple_open_framing_header, message_length) == 0, "unexpected offset of simple_open_framing_header::message_length");
static_assert(offsetof(simple_open_framing_header, encoding_type) == 2, "unexpected offset of simple_open_framing_header::encoding_type");
static_assert(sizeof(simple_open_framing_header) == 4, "unexpected sizeof simple_open_framing_header");

#pragma pack(pop)
}
