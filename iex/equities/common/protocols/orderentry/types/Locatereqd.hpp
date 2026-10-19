#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

// locatereqd
struct locatereqd {

    enum class enum_type : char {
        locate_not_required = 'N'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 1> from_string_map = {{
        {"Locate not required", enum_type::locate_not_required}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::locate_not_required: return "Locate not required";
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

    static constexpr const char* name = "locatereqd";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<locatereqd::enum_type>;
    using storage_type = result_type;

    constexpr locatereqd()
     : value{ enum_type{} } {}

    constexpr locatereqd(enum_type v)
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
            set(enum_type::locate_not_required);
    }

  protected:
    enum_type value;
};
}
