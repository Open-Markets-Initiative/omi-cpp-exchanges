#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// bid_size
struct bid_size {

    static constexpr const char* name = "bid_size";
    static constexpr std::size_t size = 8;
    static constexpr bool is_optional = false;

    using result_type = required<double>;
    using storage_type = result_type;

    constexpr bid_size()
     : value{ 0 } {}

    bid_size(double v)
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
