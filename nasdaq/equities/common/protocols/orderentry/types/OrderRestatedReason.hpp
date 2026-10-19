#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// order_restated_reason
struct order_restated_reason {

    enum class enum_type : char {
        refresh_of_display = 'R',
        update_of_displayed_price = 'P'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Refresh Of Display", enum_type::refresh_of_display},
        {"Update Of Displayed Price", enum_type::update_of_displayed_price}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::refresh_of_display: return "Refresh Of Display";
            case enum_type::update_of_displayed_price: return "Update Of Displayed Price";
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

    static constexpr const char* name = "order_restated_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<order_restated_reason::enum_type>;
    using storage_type = result_type;

    constexpr order_restated_reason()
     : value{ enum_type{} } {}

    constexpr order_restated_reason(enum_type v)
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
            set(enum_type::update_of_displayed_price);
    }

  protected:
    enum_type value;
};
}
