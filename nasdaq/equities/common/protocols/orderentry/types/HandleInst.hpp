#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// handle_inst
struct handle_inst {

    enum class enum_type : char {
        imbalance_only = 'I',
        retail_order_type_1 = 'O',
        retail_order_type_2 = 'T',
        retail_price_improvement = 'Q',
        extended_life_continuous = 'B',
        direct_listing_capital_raise = 'D'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 6> from_string_map = {{
        {"Direct Listing Capital Raise", enum_type::direct_listing_capital_raise},
        {"Extended Life Continuous", enum_type::extended_life_continuous},
        {"Imbalance Only", enum_type::imbalance_only},
        {"Retail Order Type 1", enum_type::retail_order_type_1},
        {"Retail Order Type 2", enum_type::retail_order_type_2},
        {"Retail Price Improvement", enum_type::retail_price_improvement}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::imbalance_only: return "Imbalance Only";
            case enum_type::retail_order_type_1: return "Retail Order Type 1";
            case enum_type::retail_order_type_2: return "Retail Order Type 2";
            case enum_type::retail_price_improvement: return "Retail Price Improvement";
            case enum_type::extended_life_continuous: return "Extended Life Continuous";
            case enum_type::direct_listing_capital_raise: return "Direct Listing Capital Raise";
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

    static constexpr const char* name = "handle_inst";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<handle_inst::enum_type>;
    using storage_type = result_type;

    constexpr handle_inst()
     : value{ enum_type{} } {}

    constexpr handle_inst(enum_type v)
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
            set(enum_type::direct_listing_capital_raise);
    }

  protected:
    enum_type value;
};
}
