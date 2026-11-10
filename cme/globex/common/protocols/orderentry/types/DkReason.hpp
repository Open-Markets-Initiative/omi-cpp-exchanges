#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// dk_reason
struct dk_reason {

    enum class enum_type : char {
        unknown_security = 'A',
        wrong_side = 'B',
        quantity_exceeds_order = 'C',
        no_matching_order = 'D',
        price_exceeds_limit = 'E',
        calculation_difference = 'F',
        no_matching_execution_report = 'G',
        other = 'Z'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 8> from_string_map = {{
        {"Calculation Difference", enum_type::calculation_difference},
        {"No Matching Execution Report", enum_type::no_matching_execution_report},
        {"No Matching Order", enum_type::no_matching_order},
        {"Other", enum_type::other},
        {"Price Exceeds Limit", enum_type::price_exceeds_limit},
        {"Quantity Exceeds Order", enum_type::quantity_exceeds_order},
        {"Unknown Security", enum_type::unknown_security},
        {"Wrong Side", enum_type::wrong_side}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::unknown_security: return "Unknown Security";
            case enum_type::wrong_side: return "Wrong Side";
            case enum_type::quantity_exceeds_order: return "Quantity Exceeds Order";
            case enum_type::no_matching_order: return "No Matching Order";
            case enum_type::price_exceeds_limit: return "Price Exceeds Limit";
            case enum_type::calculation_difference: return "Calculation Difference";
            case enum_type::no_matching_execution_report: return "No Matching Execution Report";
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

    static constexpr const char* name = "dk_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<dk_reason::enum_type>;
    using storage_type = result_type;

    constexpr dk_reason()
     : value{ static_cast<enum_type>(0) } {}

    constexpr dk_reason(enum_type v)
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
