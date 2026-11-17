#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// cross_type
struct cross_type {

    enum class enum_type : char {
        continuous_market = 'N',
        opening_cross = 'O',
        closing_cross = 'C',
        halt_ipo = 'H',
        supplemental = 'S',
        retail = 'R',
        extended_life = 'E',
        after_hours_close = 'A'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 8> from_string_map = {{
        {"After Hours Close", enum_type::after_hours_close},
        {"Closing Cross", enum_type::closing_cross},
        {"Continuous Market", enum_type::continuous_market},
        {"Extended Life", enum_type::extended_life},
        {"Halt Ipo", enum_type::halt_ipo},
        {"Opening Cross", enum_type::opening_cross},
        {"Retail", enum_type::retail},
        {"Supplemental", enum_type::supplemental}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::continuous_market: return "Continuous Market";
            case enum_type::opening_cross: return "Opening Cross";
            case enum_type::closing_cross: return "Closing Cross";
            case enum_type::halt_ipo: return "Halt Ipo";
            case enum_type::supplemental: return "Supplemental";
            case enum_type::retail: return "Retail";
            case enum_type::extended_life: return "Extended Life";
            case enum_type::after_hours_close: return "After Hours Close";
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

    static constexpr const char* name = "cross_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<cross_type::enum_type>;
    using storage_type = result_type;

    constexpr cross_type()
     : value{ enum_type{} } {}

    constexpr cross_type(enum_type v)
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
            set(enum_type::after_hours_close);
    }

  protected:
    enum_type value;
};
}
