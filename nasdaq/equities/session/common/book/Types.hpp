#pragma once

#include <cstdint>
#include <optional>

namespace book {

// The internal order representation. Generated from the book definition;
// wire values are converted into these types at the handler boundary.

using Key = std::uint64_t;
using OrderId = std::uint64_t;
using Price = std::int64_t;  // fixed point, exponent -9
using Quantity = std::uint64_t;
enum class Side : std::uint8_t { Bid, Ask };

// Fixed-point exponent of the internal price. The internal scale (9)
// is fixed for every instrument; the protocol scale varies per instrument.
inline constexpr int price_exponent = -9;

// Exact integer power of ten — the price scale is fixed point, never floating.
constexpr Price pow10(int places) {
    Price value = 1;
    while (places-- > 0) { value *= 10; }
    return value;
}

// Multiplier that lifts a wire mantissa at `decimals` places to the internal
// scale. Exact: the internal scale is always >= the protocol scale.
constexpr Price price_scale(int decimals) {
    return pow10(-price_exponent - decimals);
}

// An optional wire mantissa (empty when the field is its null sentinel, e.g.
// a market order with no price) lifted to the internal price. Empty stays
// empty — a no-price order carries no price through the book to the output.
template <typename Mantissa>
inline std::optional<Price> price_of(std::optional<Mantissa> mantissa, Price multiplier) {
    return mantissa ? std::optional<Price>(static_cast<Price>(*mantissa) * multiplier) : std::nullopt;
}

// Canonical price -> floating price. The ONLY place a price becomes a double,
// used where a record column is written. Dividing by the exact power of ten is
// correctly rounded; multiplying by 1e-9 left 40% of prices one ulp off.
constexpr double decimal(Price price) { return static_cast<double>(price) / 1e9; }
inline std::optional<double> decimal(std::optional<Price> price) {
    return price ? std::optional<double>(decimal(*price)) : std::nullopt;
}

}
