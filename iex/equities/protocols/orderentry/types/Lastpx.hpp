#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// lastpx
struct lastpx {

    static constexpr const char* name = "lastpx";
    static constexpr std::size_t size = 8;
    static constexpr bool is_optional = false;

    using result_type = required<double>;
    using storage_type = result_type;

    constexpr lastpx()
     : value{ 0 } {}

    constexpr lastpx(double v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(double v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    double value;
};
}
