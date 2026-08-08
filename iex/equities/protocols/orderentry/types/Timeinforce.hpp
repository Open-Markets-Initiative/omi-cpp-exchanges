#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// timeinforce
struct timeinforce {

    enum class enum_type : char {
        day = '0',
        at_the_open = '2',
        ioc = '3',
        fok = '4',
        good_til_extended_day = '5',
        good_til_time = '6',
        at_the_close = '7',
        system_hours = 'M'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 8> from_string_map = {{
        {"At the close", enum_type::at_the_close},
        {"At the open", enum_type::at_the_open},
        {"DAY", enum_type::day},
        {"FOK", enum_type::fok},
        {"Good til extended day", enum_type::good_til_extended_day},
        {"Good til time", enum_type::good_til_time},
        {"IOC", enum_type::ioc},
        {"System hours", enum_type::system_hours}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::day: return "DAY";
            case enum_type::at_the_open: return "At the open";
            case enum_type::ioc: return "IOC";
            case enum_type::fok: return "FOK";
            case enum_type::good_til_extended_day: return "Good til extended day";
            case enum_type::good_til_time: return "Good til time";
            case enum_type::at_the_close: return "At the close";
            case enum_type::system_hours: return "System hours";
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

    static constexpr const char* name = "timeinforce";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<timeinforce::enum_type>;
    using storage_type = result_type;

    constexpr timeinforce()
     : value{ enum_type::day } {}

    constexpr timeinforce(enum_type v)
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
            set(enum_type::system_hours);
    }

  protected:
    enum_type value;
};
}
