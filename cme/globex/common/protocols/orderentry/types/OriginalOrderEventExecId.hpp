#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// original_order_event_exec_id
struct original_order_event_exec_id {

    static constexpr const char* name = "original_order_event_exec_id";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = true;
    static constexpr std::uint32_t null_value = 4294967295;

    using result_type = std::optional<std::uint32_t>;
    using storage_type = result_type;

    constexpr original_order_event_exec_id()
     : value{ null_value } {}

    constexpr original_order_event_exec_id(std::uint32_t v)
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
