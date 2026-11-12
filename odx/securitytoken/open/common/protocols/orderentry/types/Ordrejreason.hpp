#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// ordrejreason
struct ordrejreason {

    enum class enum_type : std::int32_t {
        unknown_symbol = 1,
        incorrect_quantity = 13,
        price_exceeds_current_price_band = 16,
        duplicate_order_eg_duplicate_clordid_11 = 6,
        other = 99
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Duplicate Order (e.g. duplicate ClOrdID (11))", enum_type::duplicate_order_eg_duplicate_clordid_11},
        {"Incorrect quantity", enum_type::incorrect_quantity},
        {"Other", enum_type::other},
        {"Price exceeds current price band", enum_type::price_exceeds_current_price_band},
        {"Unknown symbol", enum_type::unknown_symbol}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::unknown_symbol: return "Unknown symbol";
            case enum_type::incorrect_quantity: return "Incorrect quantity";
            case enum_type::price_exceeds_current_price_band: return "Price exceeds current price band";
            case enum_type::duplicate_order_eg_duplicate_clordid_11: return "Duplicate Order (e.g. duplicate ClOrdID (11))";
            case enum_type::other: return "Other";
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

    static constexpr const char* name = "ordrejreason";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<ordrejreason::enum_type>;
    using storage_type = result_type;

    constexpr ordrejreason()
     : value{ enum_type{} } {}

    constexpr ordrejreason(enum_type v)
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
            set(enum_type::other);
    }

  protected:
    enum_type value;
};
}
