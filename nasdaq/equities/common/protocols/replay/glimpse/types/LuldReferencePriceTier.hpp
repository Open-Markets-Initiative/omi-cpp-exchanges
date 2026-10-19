#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

// luld_reference_price_tier
struct luld_reference_price_tier {

    static constexpr const char* name = "luld_reference_price_tier";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<char>;
    using storage_type = result_type;

    constexpr luld_reference_price_tier()
     : value{ 0 } {}

    constexpr luld_reference_price_tier(char v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(char v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    char value;
};
}
