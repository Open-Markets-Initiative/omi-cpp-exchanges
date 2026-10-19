#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace nse::nsefo::mtbt::binary::v7_1 {

// order_id
struct order_id {

    static constexpr const char* name = "order_id";
    static constexpr std::size_t size = 8;
    static constexpr bool is_optional = false;

    using result_type = required<double>;
    using storage_type = result_type;

    constexpr order_id()
     : value{ 0 } {}

    constexpr order_id(double v)
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
