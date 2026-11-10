#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// ord_type
struct ord_type {

    enum class enum_type : char {
        marketwith_protection = '1',
        limit = '2',
        stopwith_protection = '3',
        stop_limit = '4',
        market_with_leftover_as_limit = 'K'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Limit", enum_type::limit},
        {"Market With Leftover As Limit", enum_type::market_with_leftover_as_limit},
        {"Marketwith Protection", enum_type::marketwith_protection},
        {"Stop Limit", enum_type::stop_limit},
        {"Stopwith Protection", enum_type::stopwith_protection}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::marketwith_protection: return "Marketwith Protection";
            case enum_type::limit: return "Limit";
            case enum_type::stopwith_protection: return "Stopwith Protection";
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

    static constexpr const char* name = "ord_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<ord_type::enum_type>;
    using storage_type = result_type;

    constexpr ord_type()
     : value{ enum_type{} } {}

    constexpr ord_type(enum_type v)
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
            set(enum_type::market_with_leftover_as_limit);
    }

  protected:
    enum_type value;
};
}
