#pragma once

#include <cstddef>
#include "../types/Mantissa32.hpp"
#include "../types/Exponent.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct risk_free_rate {

    sbe_ilink3::mantissa_32 mantissa_32;
    sbe_ilink3::exponent exponent;

    // parse method
    static risk_free_rate* parse(std::byte* buffer) {
        return reinterpret_cast<risk_free_rate*>(buffer);
    }

    // parse method const
    static const risk_free_rate* parse(const std::byte* buffer) {
        return reinterpret_cast<const risk_free_rate*>(buffer);
    }
};

// layout verification
static_assert(offsetof(risk_free_rate, mantissa_32) == 0, "unexpected offset of risk_free_rate::mantissa_32");
static_assert(offsetof(risk_free_rate, exponent) == 4, "unexpected offset of risk_free_rate::exponent");
static_assert(sizeof(risk_free_rate) == 5, "unexpected sizeof risk_free_rate");

#pragma pack(pop)
}
