#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::pts::itch::v1_2 {

// occurred_as_cross
struct occurred_as_cross {

    enum class enum_type : char {
        trade_occurred_at_the_cross = 'Y'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 1> from_string_map = {{
        {"Trade Occurred At The Cross", enum_type::trade_occurred_at_the_cross}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::trade_occurred_at_the_cross: return "Trade Occurred At The Cross";
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

    static constexpr const char* name = "occurred_as_cross";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<occurred_as_cross::enum_type>;
    using storage_type = result_type;

    constexpr occurred_as_cross()
     : value{ enum_type{} } {}

    constexpr occurred_as_cross(enum_type v)
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
            set(enum_type::trade_occurred_at_the_cross);
    }

  protected:
    enum_type value;
};
}
