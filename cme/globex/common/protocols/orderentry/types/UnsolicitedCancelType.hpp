#pragma once

#include <cstddef>
#include <optional>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// unsolicited_cancel_type
struct unsolicited_cancel_type {

    static constexpr const char* name = "unsolicited_cancel_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = true;
    static constexpr char null_value = 0;

    using result_type = std::optional<char>;
    using storage_type = result_type;

    constexpr unsolicited_cancel_type()
     : value{ null_value } {}

    constexpr unsolicited_cancel_type(char v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return value == null_value ? std::nullopt : result_type{value};
    }

    constexpr void set(char v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(null_value);
    }

    constexpr void set_null() {
        value = null_value;
    }

  protected:
    char value;
};
}
