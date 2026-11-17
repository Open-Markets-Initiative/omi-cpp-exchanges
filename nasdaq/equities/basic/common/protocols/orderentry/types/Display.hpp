#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// display
struct display {

    enum class enum_type : char {
        attributable = 'A',
        visible = 'Y',
        hidden = 'N',
        conformant = 'Z'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Attributable", enum_type::attributable},
        {"Conformant", enum_type::conformant},
        {"Hidden", enum_type::hidden},
        {"Visible", enum_type::visible}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::attributable: return "Attributable";
            case enum_type::visible: return "Visible";
            case enum_type::hidden: return "Hidden";
            case enum_type::conformant: return "Conformant";
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

    static constexpr const char* name = "display";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<display::enum_type>;
    using storage_type = result_type;

    constexpr display()
     : value{ enum_type{} } {}

    constexpr display(enum_type v)
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
            set(enum_type::conformant);
    }

  protected:
    enum_type value;
};
}
