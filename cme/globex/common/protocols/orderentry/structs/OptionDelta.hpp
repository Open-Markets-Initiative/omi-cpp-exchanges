#pragma once

#include <cstddef>
#include "../types/Mantissa32.hpp"
#include "../types/Exponent.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct option_delta {

    sbe_ilink3::mantissa_32 mantissa_32;
    sbe_ilink3::exponent exponent;

    // parse method
    static option_delta* parse(std::byte* buffer) {
        return reinterpret_cast<option_delta*>(buffer);
    }

    // parse method const
    static const option_delta* parse(const std::byte* buffer) {
        return reinterpret_cast<const option_delta*>(buffer);
    }
};

// layout verification
static_assert(offsetof(option_delta, mantissa_32) == 0, "unexpected offset of option_delta::mantissa_32");
static_assert(offsetof(option_delta, exponent) == 4, "unexpected offset of option_delta::exponent");
static_assert(sizeof(option_delta) == 5, "unexpected sizeof option_delta");

#pragma pack(pop)
}
