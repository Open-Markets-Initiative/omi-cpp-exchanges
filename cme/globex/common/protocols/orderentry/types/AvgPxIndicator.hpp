#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// avg_px_indicator
struct avg_px_indicator {

    enum class enum_type : std::uint8_t {
        no_value = 255,
        no_average_pricing = 0,
        tradeispartofan_average_price_group_identifiedbythe_avg_px_grp_id = 1,
        tradeispartofa_notional_value_average_price_group = 3
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"No Average Pricing", enum_type::no_average_pricing},
        {"No Value", enum_type::no_value},
        {"Tradeispartofa Notional Value Average Price Group", enum_type::tradeispartofa_notional_value_average_price_group},
        {"Tradeispartofan Average Price Group Identifiedbythe Avg Px Grp Id", enum_type::tradeispartofan_average_price_group_identifiedbythe_avg_px_grp_id}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::no_value: return "No Value";
            case enum_type::no_average_pricing: return "No Average Pricing";
            case enum_type::tradeispartofan_average_price_group_identifiedbythe_avg_px_grp_id: return "Tradeispartofan Average Price Group Identifiedbythe Avg Px Grp Id";
            case enum_type::tradeispartofa_notional_value_average_price_group: return "Tradeispartofa Notional Value Average Price Group";
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

    static constexpr const char* name = "avg_px_indicator";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<avg_px_indicator::enum_type>;
    using storage_type = result_type;

    constexpr avg_px_indicator()
     : value{ static_cast<enum_type>(255) } {}

    constexpr avg_px_indicator(enum_type v)
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
            set(static_cast<enum_type>(255));
    }

  protected:
    enum_type value;
};
}
