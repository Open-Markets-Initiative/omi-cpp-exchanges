#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// crosstradeflag
struct crosstradeflag {

    enum class enum_type : char {
        extended_trading_close = 'A',
        closing_cross = 'C',
        extended_life = 'E',
        opening_cross = 'O'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Closing Cross", enum_type::closing_cross},
        {"Extended Life", enum_type::extended_life},
        {"Extended Trading Close", enum_type::extended_trading_close},
        {"Opening Cross", enum_type::opening_cross}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::extended_trading_close: return "Extended Trading Close";
            case enum_type::closing_cross: return "Closing Cross";
            case enum_type::extended_life: return "Extended Life";
            case enum_type::opening_cross: return "Opening Cross";
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

    static constexpr const char* name = "crosstradeflag";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<crosstradeflag::enum_type>;
    using storage_type = result_type;

    constexpr crosstradeflag()
     : value{ enum_type{} } {}

    constexpr crosstradeflag(enum_type v)
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
            set(enum_type::opening_cross);
    }

  protected:
    enum_type value;
};
}
