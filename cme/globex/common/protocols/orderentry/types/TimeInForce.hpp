#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// time_in_force
struct time_in_force {

    enum class enum_type : std::uint8_t {
        no_value = 255,
        day = 0,
        good_till_cancel = 1,
        fill_and_kill = 3,
        fill_or_kill = 4,
        good_till_date = 6,
        good_for_session = 99
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 7> from_string_map = {{
        {"Day", enum_type::day},
        {"Fill And Kill", enum_type::fill_and_kill},
        {"Fill Or Kill", enum_type::fill_or_kill},
        {"Good For Session", enum_type::good_for_session},
        {"Good Till Cancel", enum_type::good_till_cancel},
        {"Good Till Date", enum_type::good_till_date},
        {"No Value", enum_type::no_value}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::no_value: return "No Value";
            case enum_type::day: return "Day";
            case enum_type::good_till_cancel: return "Good Till Cancel";
            case enum_type::fill_and_kill: return "Fill And Kill";
            case enum_type::fill_or_kill: return "Fill Or Kill";
            case enum_type::good_till_date: return "Good Till Date";
            case enum_type::good_for_session: return "Good For Session";
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
     : value{ static_cast<enum_type>(255) } {}

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
            set(static_cast<enum_type>(255));
    }

  protected:
    enum_type value;
};
}
