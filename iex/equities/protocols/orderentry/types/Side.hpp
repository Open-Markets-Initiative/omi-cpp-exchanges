#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// side
struct side {

    enum class enum_type : char {
        buy = '1',
        sell_long = '2',
        sell_short = '5',
        sell_short_exempt = '6'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"BUY", enum_type::buy},
        {"Sell long", enum_type::sell_long},
        {"Sell short", enum_type::sell_short},
        {"Sell short exempt", enum_type::sell_short_exempt}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::buy: return "BUY";
            case enum_type::sell_long: return "Sell long";
            case enum_type::sell_short: return "Sell short";
            case enum_type::sell_short_exempt: return "Sell short exempt";
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

    static constexpr const char* name = "side";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<side::enum_type>;
    using storage_type = result_type;

    constexpr side()
     : value{ enum_type::buy } {}

    constexpr side(enum_type v)
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
            set(enum_type::sell_short_exempt);
    }

  protected:
    enum_type value;
};
}
