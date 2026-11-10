#pragma once

#include <cstddef>
#include "../types/Mantissa.hpp"
#include "../types/Exponent.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct volatility {

    sbe_ilink3::mantissa mantissa;
    sbe_ilink3::exponent exponent;

    // parse method
    static volatility* parse(std::byte* buffer) {
        return reinterpret_cast<volatility*>(buffer);
    }

    // parse method const
    static const volatility* parse(const std::byte* buffer) {
        return reinterpret_cast<const volatility*>(buffer);
    }
};

// layout verification
static_assert(offsetof(volatility, mantissa) == 0, "unexpected offset of volatility::mantissa");
static_assert(offsetof(volatility, exponent) == 8, "unexpected offset of volatility::exponent");
static_assert(sizeof(volatility) == 9, "unexpected sizeof volatility");

#pragma pack(pop)
}
