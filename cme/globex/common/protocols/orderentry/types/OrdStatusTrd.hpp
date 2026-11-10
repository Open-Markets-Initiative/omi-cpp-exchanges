#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// ord_status_trd
struct ord_status_trd {

    enum class enum_type : std::uint8_t {
        partially_filled = 1,
        filled = 2
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Filled", enum_type::filled},
        {"Partially Filled", enum_type::partially_filled}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::partially_filled: return "Partially Filled";
            case enum_type::filled: return "Filled";
            default: return "unknown";
        }
    }

    static constexpr std::optional<enum_type> from_string(std::string_view str) {
        auto it = std::lower_bound(
            from_string_map.begin(),
            from_string_map.end(),
            str,
            [](const auto& pair, std::string_view s) { return pair.first < s; }
        );
        if (it != from_string_map.end() && it->first == str) {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const char* name = "ord_status_trd";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<ord_status_trd::enum_type>;
    using storage_type = result_type;

    constexpr ord_status_trd()
     : value{ enum_type{} } {}

    constexpr ord_status_trd(enum_type v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(enum_type v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(enum_type::filled);
    }

  protected:
    enum_type value;
};
}
