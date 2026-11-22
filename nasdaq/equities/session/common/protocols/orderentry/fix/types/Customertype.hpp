#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// customertype
struct customertype {

    enum class enum_type : char {
        not_a_retail_designated_order_default = 'N',
        retail_designated_order = 'R'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Not a retail designated order (default)", enum_type::not_a_retail_designated_order_default},
        {"Retail designated order", enum_type::retail_designated_order}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::not_a_retail_designated_order_default: return "Not a retail designated order (default)";
            case enum_type::retail_designated_order: return "Retail designated order";
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

    static constexpr const char* name = "customertype";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<customertype::enum_type>;
    using storage_type = result_type;

    constexpr customertype()
     : value{ enum_type{} } {}

    constexpr customertype(enum_type v)
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
            set(enum_type::retail_designated_order);
    }

  protected:
    enum_type value;
};
}
