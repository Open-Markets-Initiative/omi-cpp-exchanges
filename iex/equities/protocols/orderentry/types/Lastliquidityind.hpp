#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// lastliquidityind
struct lastliquidityind {

    enum class enum_type : std::int32_t {
        added_liquidity = 1,
        removed_liquidity = 2,
        routed = 3,
        auction = 4,
        removed_liquidity_on_recheck = 9
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Added liquidity", enum_type::added_liquidity},
        {"AUCTION", enum_type::auction},
        {"Removed liquidity", enum_type::removed_liquidity},
        {"Removed liquidity on recheck", enum_type::removed_liquidity_on_recheck},
        {"ROUTED", enum_type::routed}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::added_liquidity: return "Added liquidity";
            case enum_type::removed_liquidity: return "Removed liquidity";
            case enum_type::routed: return "ROUTED";
            case enum_type::auction: return "AUCTION";
            case enum_type::removed_liquidity_on_recheck: return "Removed liquidity on recheck";
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

    static constexpr const char* name = "lastliquidityind";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<lastliquidityind::enum_type>;
    using storage_type = result_type;

    constexpr lastliquidityind()
     : value{ enum_type::added_liquidity } {}

    constexpr lastliquidityind(enum_type v)
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
            set(enum_type::removed_liquidity_on_recheck);
    }

  protected:
    enum_type value;
};
}
