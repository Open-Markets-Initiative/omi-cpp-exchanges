#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// time_in_force
struct time_in_force {

    enum class enum_type : char {
        day = '0',
        ioc = '3',
        gtx_extended_hours = '5',
        gtt = '6',
        after_hours = 'E'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"After Hours", enum_type::after_hours},
        {"Day", enum_type::day},
        {"Gtt", enum_type::gtt},
        {"Gtx Extended Hours", enum_type::gtx_extended_hours},
        {"Ioc", enum_type::ioc}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::day: return "Day";
            case enum_type::ioc: return "Ioc";
            case enum_type::gtx_extended_hours: return "Gtx Extended Hours";
            case enum_type::gtt: return "Gtt";
            case enum_type::after_hours: return "After Hours";
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

    static constexpr const char* name = "time_in_force";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<time_in_force::enum_type>;
    using storage_type = result_type;

    constexpr time_in_force()
     : value{ enum_type{} } {}

    constexpr time_in_force(enum_type v)
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
            set(enum_type::after_hours);
    }

  protected:
    enum_type value;
};
}
