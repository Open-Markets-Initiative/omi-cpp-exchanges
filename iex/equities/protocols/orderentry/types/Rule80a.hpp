#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// rule80a
struct rule80a {

    static constexpr const char* name = "rule80a";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<char>;
    using storage_type = result_type;

    constexpr rule80a()
     : value{ 0 } {}

    constexpr rule80a(char v)
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
