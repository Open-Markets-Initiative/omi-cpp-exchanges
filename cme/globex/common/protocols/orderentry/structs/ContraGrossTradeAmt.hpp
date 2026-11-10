#pragma once

#include <cstddef>
#include "../types/Mantissa.hpp"
#include "../types/Exponent.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct contra_gross_trade_amt {

    sbe_ilink3::mantissa mantissa;
    sbe_ilink3::exponent exponent;

    // parse method
    static contra_gross_trade_amt* parse(std::byte* buffer) {
        return reinterpret_cast<contra_gross_trade_amt*>(buffer);
    }

    // parse method const
    static const contra_gross_trade_amt* parse(const std::byte* buffer) {
        return reinterpret_cast<const contra_gross_trade_amt*>(buffer);
    }
};

// layout verification
static_assert(offsetof(contra_gross_trade_amt, mantissa) == 0, "unexpected offset of contra_gross_trade_amt::mantissa");
static_assert(offsetof(contra_gross_trade_amt, exponent) == 8, "unexpected offset of contra_gross_trade_amt::exponent");
static_assert(sizeof(contra_gross_trade_amt) == 9, "unexpected sizeof contra_gross_trade_amt");

#pragma pack(pop)
}
