#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// initiated_by_control
struct initiated_by_control {

    static constexpr const char* name = "initiated_by_control";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<char>;
    using storage_type = result_type;

    constexpr initiated_by_control()
     : value{ 0 } {}

    constexpr initiated_by_control(char v)
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
