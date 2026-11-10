#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// side_time_in_force
struct side_time_in_force {

    enum class enum_type : std::uint8_t {
        day = 0,
        fak = 3
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Day", enum_type::day},
        {"Fak", enum_type::fak}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::day: return "Day";
            case enum_type::fak: return "Fak";
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

    static constexpr const char* name = "side_time_in_force";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<side_time_in_force::enum_type>;
    using storage_type = result_type;

    constexpr side_time_in_force()
     : value{ enum_type{} } {}

    constexpr side_time_in_force(enum_type v)
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
            set(enum_type::fak);
    }

  protected:
    enum_type value;
};
}
