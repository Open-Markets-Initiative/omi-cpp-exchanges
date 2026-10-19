#pragma once

#include <cstddef>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// trigger_price_1
struct trigger_price_1 {

    static constexpr const char* name = "trigger_price_1";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;
    static constexpr int exponent = -2;

    using result_type = required<std::int32_t>;
    using storage_type = result_type;

    constexpr trigger_price_1()
     : value{ 0 } {}

    trigger_price_1(std::int32_t v)
     : value{ static_cast<std::int32_t>(__builtin_bswap32(v)) } {}

    [[nodiscard]] result_type get() const {
        return result_type{static_cast<std::int32_t>(__builtin_bswap32(value))};
    }

    void set(std::int32_t v) {
        value = __builtin_bswap32(v);
    }

    void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    std::int32_t value;
};
}
