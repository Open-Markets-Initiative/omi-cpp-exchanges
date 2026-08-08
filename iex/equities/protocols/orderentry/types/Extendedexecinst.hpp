#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// extendedexecinst
struct extendedexecinst {

    enum class enum_type : char {
        retail = 'R',
        retail_liquidity_provider = 'T'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"RETAIL", enum_type::retail},
        {"Retail liquidity provider", enum_type::retail_liquidity_provider}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::retail: return "RETAIL";
            case enum_type::retail_liquidity_provider: return "Retail liquidity provider";
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

    static constexpr const char* name = "extendedexecinst";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<extendedexecinst::enum_type>;
    using storage_type = result_type;

    constexpr extendedexecinst()
     : value{ enum_type::retail } {}

    constexpr extendedexecinst(enum_type v)
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
            set(enum_type::retail_liquidity_provider);
    }

  protected:
    enum_type value;
};
}
