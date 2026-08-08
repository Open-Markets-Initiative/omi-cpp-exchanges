#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// ordtype
struct ordtype {

    enum class enum_type : char {
        market = '1',
        limit = '2',
        pegged = 'P'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"LIMIT", enum_type::limit},
        {"MARKET", enum_type::market},
        {"PEGGED", enum_type::pegged}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::market: return "MARKET";
            case enum_type::limit: return "LIMIT";
            case enum_type::pegged: return "PEGGED";
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
     : value{ enum_type::market } {}

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
            set(enum_type::pegged);
    }

  protected:
    enum_type value;
};
}
