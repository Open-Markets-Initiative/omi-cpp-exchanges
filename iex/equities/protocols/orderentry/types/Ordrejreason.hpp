#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// ordrejreason
struct ordrejreason {

    enum class enum_type : std::int32_t {
        broker_option = 0,
        unknown_symbol = 1,
        exchange_closed = 2,
        order_exceeds_limit = 3,
        too_late_to_enter = 4,
        unknown_order = 5
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 6> from_string_map = {{
        {"Broker option", enum_type::broker_option},
        {"Exchange closed", enum_type::exchange_closed},
        {"Order exceeds limit", enum_type::order_exceeds_limit},
        {"Too late to enter", enum_type::too_late_to_enter},
        {"Unknown order", enum_type::unknown_order},
        {"Unknown symbol", enum_type::unknown_symbol}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::broker_option: return "Broker option";
            case enum_type::unknown_symbol: return "Unknown symbol";
            case enum_type::exchange_closed: return "Exchange closed";
            case enum_type::order_exceeds_limit: return "Order exceeds limit";
            case enum_type::too_late_to_enter: return "Too late to enter";
            case enum_type::unknown_order: return "Unknown order";
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
     : value{ enum_type::broker_option } {}

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
            set(enum_type::unknown_order);
    }

  protected:
    enum_type value;
};
}
