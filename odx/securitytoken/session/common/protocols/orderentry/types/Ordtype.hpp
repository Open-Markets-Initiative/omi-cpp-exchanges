#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// ordtype
struct ordtype {

    enum class enum_type : char {
        market = '1',
        limit = '2',
        market_on_close = '5',
        limit_on_close = 'B'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Limit", enum_type::limit},
        {"Limit on Close", enum_type::limit_on_close},
        {"Market", enum_type::market},
        {"Market on Close", enum_type::market_on_close}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::market: return "Market";
            case enum_type::limit: return "Limit";
            case enum_type::market_on_close: return "Market on Close";
            case enum_type::limit_on_close: return "Limit on Close";
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

    static constexpr const char* name = "ordtype";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<ordtype::enum_type>;
    using storage_type = result_type;

    constexpr ordtype()
     : value{ enum_type{} } {}

    constexpr ordtype(enum_type v)
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
            set(enum_type::limit_on_close);
    }

  protected:
    enum_type value;
};
}
