#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// timeinforce
struct timeinforce {

    enum class enum_type : char {
        market_hours_day = '0',
        extended_hours_order_1 = '1',
        on_open = '2',
        immediate_or_cancel = '3',
        extended_hours_order_5 = '5',
        good_till_time = '6',
        on_close = '7',
        extended_hours_order_A = 'A',
        extended_trading_close = 'E'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 9> from_string_map = {{
        {"Extended Hours Order", enum_type::extended_hours_order_1},
        {"Extended Hours Order", enum_type::extended_hours_order_5},
        {"Extended Hours Order", enum_type::extended_hours_order_A},
        {"Extended Trading Close", enum_type::extended_trading_close},
        {"Good Till Time", enum_type::good_till_time},
        {"Immediate or Cancel", enum_type::immediate_or_cancel},
        {"Market Hours (Day)", enum_type::market_hours_day},
        {"On Close", enum_type::on_close},
        {"On Open", enum_type::on_open}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::market_hours_day: return "Market Hours (Day)";
            case enum_type::extended_hours_order_1: return "Extended Hours Order";
            case enum_type::on_open: return "On Open";
            case enum_type::immediate_or_cancel: return "Immediate or Cancel";
            case enum_type::extended_hours_order_5: return "Extended Hours Order";
            case enum_type::good_till_time: return "Good Till Time";
            case enum_type::on_close: return "On Close";
            case enum_type::extended_hours_order_A: return "Extended Hours Order";
            case enum_type::extended_trading_close: return "Extended Trading Close";
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

    static constexpr const char* name = "timeinforce";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<timeinforce::enum_type>;
    using storage_type = result_type;

    constexpr timeinforce()
     : value{ enum_type{} } {}

    constexpr timeinforce(enum_type v)
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
            set(enum_type::extended_trading_close);
    }

  protected:
    enum_type value;
};
}
