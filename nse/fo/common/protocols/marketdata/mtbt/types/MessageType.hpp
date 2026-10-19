#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::mtbt::binary::v7_1 {

// message_type
struct message_type {

    enum class enum_type : char {
        new_order_message = 'N',
        order_modification_message = 'M',
        order_cancellation_message = 'X',
        trade_message = 'T',
        new_spread_order_message = 'G',
        spread_order_modification_message = 'H',
        spread_order_cancellation_message = 'J',
        spread_trade_message = 'K',
        trade_cancel_message = 'C',
        heartbeat_message = 'Z'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 10> from_string_map = {{
        {"Heartbeat Message", enum_type::heartbeat_message},
        {"New Order Message", enum_type::new_order_message},
        {"New Spread Order Message", enum_type::new_spread_order_message},
        {"Order Cancellation Message", enum_type::order_cancellation_message},
        {"Order Modification Message", enum_type::order_modification_message},
        {"Spread Order Cancellation Message", enum_type::spread_order_cancellation_message},
        {"Spread Order Modification Message", enum_type::spread_order_modification_message},
        {"Spread Trade Message", enum_type::spread_trade_message},
        {"Trade Cancel Message", enum_type::trade_cancel_message},
        {"Trade Message", enum_type::trade_message}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::new_order_message: return "New Order Message";
            case enum_type::order_modification_message: return "Order Modification Message";
            case enum_type::order_cancellation_message: return "Order Cancellation Message";
            case enum_type::trade_message: return "Trade Message";
            case enum_type::new_spread_order_message: return "New Spread Order Message";
            case enum_type::spread_order_modification_message: return "Spread Order Modification Message";
            case enum_type::spread_order_cancellation_message: return "Spread Order Cancellation Message";
            case enum_type::spread_trade_message: return "Spread Trade Message";
            case enum_type::trade_cancel_message: return "Trade Cancel Message";
            case enum_type::heartbeat_message: return "Heartbeat Message";
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

    static constexpr const char* name = "message_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<message_type::enum_type>;
    using storage_type = result_type;

    constexpr message_type()
     : value{ enum_type{} } {}

    constexpr message_type(enum_type v)
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
            set(enum_type::heartbeat_message);
    }

  protected:
    enum_type value;
};
}
