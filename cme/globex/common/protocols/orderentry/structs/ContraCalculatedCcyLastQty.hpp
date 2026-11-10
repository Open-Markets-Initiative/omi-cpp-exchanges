#pragma once

#include <cstddef>
#include "../types/Mantissa.hpp"
#include "../types/Exponent.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct contra_calculated_ccy_last_qty {

    sbe_ilink3::mantissa mantissa;
    sbe_ilink3::exponent exponent;

    // parse method
    static contra_calculated_ccy_last_qty* parse(std::byte* buffer) {
        return reinterpret_cast<contra_calculated_ccy_last_qty*>(buffer);
    }

    // parse method const
    static const contra_calculated_ccy_last_qty* parse(const std::byte* buffer) {
        return reinterpret_cast<const contra_calculated_ccy_last_qty*>(buffer);
    }
};

// layout verification
static_assert(offsetof(contra_calculated_ccy_last_qty, mantissa) == 0, "unexpected offset of contra_calculated_ccy_last_qty::mantissa");
static_assert(offsetof(contra_calculated_ccy_last_qty, exponent) == 8, "unexpected offset of contra_calculated_ccy_last_qty::exponent");
static_assert(sizeof(contra_calculated_ccy_last_qty) == 9, "unexpected sizeof contra_calculated_ccy_last_qty");

#pragma pack(pop)
}
