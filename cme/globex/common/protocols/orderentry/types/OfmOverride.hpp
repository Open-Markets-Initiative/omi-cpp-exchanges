#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// ofm_override
struct ofm_override {

    enum class enum_type : std::uint8_t {
        disabled = 0,
        enabled = 1
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Disabled", enum_type::disabled},
        {"Enabled", enum_type::enabled}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::disabled: return "Disabled";
            case enum_type::enabled: return "Enabled";
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

    static constexpr const char* name = "ofm_override";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<ofm_override::enum_type>;
    using storage_type = result_type;

    constexpr ofm_override()
     : value{ enum_type{} } {}

    constexpr ofm_override(enum_type v)
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
            set(enum_type::enabled);
    }

  protected:
    enum_type value;
};
}
