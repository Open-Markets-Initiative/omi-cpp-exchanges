#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// open_interest_double
struct open_interest_double {

    static constexpr const char* name = "open_interest_double";
    static constexpr std::size_t size = 8;
    static constexpr bool is_optional = false;

    using result_type = required<double>;
    using storage_type = result_type;

    constexpr open_interest_double()
     : value{ 0 } {}

    open_interest_double(double v)
     : value{ static_cast<double>(__builtin_bswap64(v)) } {}

    [[nodiscard]] result_type get() const {
        return result_type{static_cast<double>(__builtin_bswap64(value))};
    }

    void set(double v) {
        value = __builtin_bswap64(v);
    }

    void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    double value;
};
}
