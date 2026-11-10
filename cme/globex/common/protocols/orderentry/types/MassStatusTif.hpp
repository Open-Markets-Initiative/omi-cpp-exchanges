#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// mass_status_tif
struct mass_status_tif {

    enum class enum_type : std::uint8_t {
        no_value = 255,
        day = 0,
        gtc = 1,
        gtd = 6,
        gfs = 99
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Day", enum_type::day},
        {"Gfs", enum_type::gfs},
        {"Gtc", enum_type::gtc},
        {"Gtd", enum_type::gtd},
        {"No Value", enum_type::no_value}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::no_value: return "No Value";
            case enum_type::day: return "Day";
            case enum_type::gtc: return "Gtc";
            case enum_type::gtd: return "Gtd";
            case enum_type::gfs: return "Gfs";
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

    static constexpr const char* name = "mass_status_tif";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<mass_status_tif::enum_type>;
    using storage_type = result_type;

    constexpr mass_status_tif()
     : value{ static_cast<enum_type>(255) } {}

    constexpr mass_status_tif(enum_type v)
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
