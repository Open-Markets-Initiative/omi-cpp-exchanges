#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// orig_side_trade_id
struct orig_side_trade_id {

    static constexpr const char* name = "orig_side_trade_id";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = true;
    static constexpr std::uint32_t null_value = 4294967295;

    using result_type = std::optional<std::uint32_t>;
    using storage_type = result_type;

    constexpr orig_side_trade_id()
     : value{ null_value } {}

    constexpr orig_side_trade_id(std::uint32_t v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return value == null_value ? std::nullopt : result_type{value};
    }

    constexpr void set(std::uint32_t v) {
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
    std::uint32_t value;
};
}
