#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// resting_display
struct resting_display {

    enum class enum_type : char {
        visible = 'Y',
        hidden = 'N'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Hidden", enum_type::hidden},
        {"Visible", enum_type::visible}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::visible: return "Visible";
            case enum_type::hidden: return "Hidden";
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

    static constexpr const char* name = "resting_display";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<resting_display::enum_type>;
    using storage_type = result_type;

    constexpr resting_display()
     : value{ enum_type{} } {}

    constexpr resting_display(enum_type v)
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
            set(enum_type::hidden);
    }

  protected:
    enum_type value;
};
}
