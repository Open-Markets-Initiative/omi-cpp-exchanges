#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// order_cancel_reason
struct order_cancel_reason {

    static constexpr const char* name = "order_cancel_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<char>;
    using storage_type = result_type;

    constexpr order_cancel_reason()
     : value{ 0 } {}

    constexpr order_cancel_reason(char v)
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
