#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// user_order_buy_value_limit
struct user_order_buy_value_limit {

    static constexpr const char* name = "user_order_buy_value_limit";
    static constexpr std::size_t size = 8;
    static constexpr bool is_optional = false;

    using result_type = required<double>;
    using storage_type = result_type;

    constexpr user_order_buy_value_limit()
     : value{ 0 } {}

    user_order_buy_value_limit(double v)
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
