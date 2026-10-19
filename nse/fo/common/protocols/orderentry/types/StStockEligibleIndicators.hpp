#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// st_stock_eligible_indicators
struct st_stock_eligible_indicators {

    using type = uint16_t;

    static constexpr type books_merged = type{1} << 13;
    static constexpr type minimum_fill = type{1} << 14;
    static constexpr type eligible_aon = type{1} << 15;

    static constexpr const char* name = "st_stock_eligible_indicators";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<uint16_t>;
    using storage_type = result_type;

    constexpr st_stock_eligible_indicators()
     : value{ 0 } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(uint16_t v) {
        value = v;
    }

    constexpr void set(result_type v) {
        if (v.has_value())
            set(v.value());
        else
            set(type{0});
    }

    [[nodiscard]] constexpr bool has(type bit) const { return (value & bit) != 0; }
    constexpr void set(type bit, bool enabled) { if (enabled) value |= bit; else value &= static_cast<type>(~bit); }

    [[nodiscard]] constexpr bool has_books_merged() const { return has(books_merged); }
    [[nodiscard]] constexpr bool has_minimum_fill() const { return has(minimum_fill); }
    [[nodiscard]] constexpr bool has_eligible_aon() const { return has(eligible_aon); }

    constexpr void set_books_merged(bool v) { set(books_merged, v); }
    constexpr void set_minimum_fill(bool v) { set(minimum_fill, v); }
    constexpr void set_eligible_aon(bool v) { set(eligible_aon, v); }

  protected:
    type value;
};
}
