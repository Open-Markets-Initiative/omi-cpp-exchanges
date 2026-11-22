#pragma once

#include <cstddef>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// broad_cast_message_length
struct broad_cast_message_length {

    static constexpr const char* name = "broad_cast_message_length";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<std::int16_t>;
    using storage_type = result_type;

    constexpr broad_cast_message_length()
     : value{ 0 } {}

    broad_cast_message_length(std::int16_t v)
     : value{ static_cast<std::int16_t>(__builtin_bswap16(v)) } {}

    [[nodiscard]] result_type get() const {
        return result_type{static_cast<std::int16_t>(__builtin_bswap16(value))};
    }

    void set(std::int16_t v) {
        value = __builtin_bswap16(v);
    }

    void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    std::int16_t value;
};
}
