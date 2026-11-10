#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// short_sale_type
struct short_sale_type {

    enum class enum_type : std::uint8_t {
        no_value = 255,
        long_sell = 0,
        short_sale_with_no_exemption_sesh = 1,
        short_sale_with_exemption_ssex = 2,
        undisclosed_sell_information_not_available_undi = 3
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Long Sell", enum_type::long_sell},
        {"No Value", enum_type::no_value},
        {"Short Sale With Exemption Ssex", enum_type::short_sale_with_exemption_ssex},
        {"Short Sale With No Exemption Sesh", enum_type::short_sale_with_no_exemption_sesh},
        {"Undisclosed Sell Information Not Available Undi", enum_type::undisclosed_sell_information_not_available_undi}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::no_value: return "No Value";
            case enum_type::long_sell: return "Long Sell";
            case enum_type::short_sale_with_no_exemption_sesh: return "Short Sale With No Exemption Sesh";
            case enum_type::short_sale_with_exemption_ssex: return "Short Sale With Exemption Ssex";
            case enum_type::undisclosed_sell_information_not_available_undi: return "Undisclosed Sell Information Not Available Undi";
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

    static constexpr const char* name = "short_sale_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<short_sale_type::enum_type>;
    using storage_type = result_type;

    constexpr short_sale_type()
     : value{ static_cast<enum_type>(255) } {}

    constexpr short_sale_type(enum_type v)
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
