#pragma once

#include <cstddef>
#include "../types/Mantissa32.hpp"
#include "../types/Exponent.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct time_to_expiration {

    sbe_ilink3::mantissa_32 mantissa_32;
    sbe_ilink3::exponent exponent;

    // parse method
    static time_to_expiration* parse(std::byte* buffer) {
        return reinterpret_cast<time_to_expiration*>(buffer);
    }

    // parse method const
    static const time_to_expiration* parse(const std::byte* buffer) {
        return reinterpret_cast<const time_to_expiration*>(buffer);
    }
};

// layout verification
static_assert(offsetof(time_to_expiration, mantissa_32) == 0, "unexpected offset of time_to_expiration::mantissa_32");
static_assert(offsetof(time_to_expiration, exponent) == 4, "unexpected offset of time_to_expiration::exponent");
static_assert(sizeof(time_to_expiration) == 5, "unexpected sizeof time_to_expiration");

#pragma pack(pop)
}
