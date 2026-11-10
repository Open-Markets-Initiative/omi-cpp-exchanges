#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// ord_type_optional
struct ord_type_optional {

    enum class enum_type : char {
        market_with_protection = '1',
        limit = '2',
        stop_limit = '4',
        market_with_leftover_as_limit = 'K'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Limit", enum_type::limit},
        {"Market With Leftover As Limit", enum_type::market_with_leftover_as_limit},
        {"Market With Protection", enum_type::market_with_protection},
        {"Stop Limit", enum_type::stop_limit}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::market_with_protection: return "Market With Protection";
            case enum_type::limit: return "Limit";
            case enum_type::stop_limit: return "Stop Limit";
            case enum_type::market_with_leftover_as_limit: return "Market With Leftover As Limit";
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

    static constexpr const char* name = "ord_type_optional";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<ord_type_optional::enum_type>;
    using storage_type = result_type;

    constexpr ord_type_optional()
     : value{ static_cast<enum_type>(0) } {}

    constexpr ord_type_optional(enum_type v)
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
            set(static_cast<enum_type>(0));
    }

  protected:
    enum_type value;
};
}
