#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// sequenced_message_type
struct sequenced_message_type {

    enum class enum_type : char {
        system_event_message = 'S',
        order_accepted_message = 'A',
        replaced_message = 'U',
        canceled_message = 'C',
        aiq_canceled_message = 'D',
        order_executed_message = 'E',
        broken_trade_message = 'B',
        rejected_order_message = 'J',
        cancel_pending_message = 'P',
        cancel_reject_message = 'I',
        order_priority_update_message = 'T',
        order_modified_message = 'M',
        order_restated_message = 'R',
        account_query_response_message = 'Q',
        mass_cancel_response_message = 'X',
        disable_order_entry_response_message = 'G',
        enable_order_entry_response_message = 'K'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 17> from_string_map = {{
        {"Account Query Response Message", enum_type::account_query_response_message},
        {"Aiq Canceled Message", enum_type::aiq_canceled_message},
        {"Broken Trade Message", enum_type::broken_trade_message},
        {"Cancel Pending Message", enum_type::cancel_pending_message},
        {"Cancel Reject Message", enum_type::cancel_reject_message},
        {"Canceled Message", enum_type::canceled_message},
        {"Disable Order Entry Response Message", enum_type::disable_order_entry_response_message},
        {"Enable Order Entry Response Message", enum_type::enable_order_entry_response_message},
        {"Mass Cancel Response Message", enum_type::mass_cancel_response_message},
        {"Order Accepted Message", enum_type::order_accepted_message},
        {"Order Executed Message", enum_type::order_executed_message},
        {"Order Modified Message", enum_type::order_modified_message},
        {"Order Priority Update Message", enum_type::order_priority_update_message},
        {"Order Restated Message", enum_type::order_restated_message},
        {"Rejected Order Message", enum_type::rejected_order_message},
        {"Replaced Message", enum_type::replaced_message},
        {"System Event Message", enum_type::system_event_message}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::system_event_message: return "System Event Message";
            case enum_type::order_accepted_message: return "Order Accepted Message";
            case enum_type::replaced_message: return "Replaced Message";
            case enum_type::canceled_message: return "Canceled Message";
            case enum_type::aiq_canceled_message: return "Aiq Canceled Message";
            case enum_type::order_executed_message: return "Order Executed Message";
            case enum_type::broken_trade_message: return "Broken Trade Message";
            case enum_type::rejected_order_message: return "Rejected Order Message";
            case enum_type::cancel_pending_message: return "Cancel Pending Message";
            case enum_type::cancel_reject_message: return "Cancel Reject Message";
            case enum_type::order_priority_update_message: return "Order Priority Update Message";
            case enum_type::order_modified_message: return "Order Modified Message";
            case enum_type::order_restated_message: return "Order Restated Message";
            case enum_type::account_query_response_message: return "Account Query Response Message";
            case enum_type::mass_cancel_response_message: return "Mass Cancel Response Message";
            case enum_type::disable_order_entry_response_message: return "Disable Order Entry Response Message";
            case enum_type::enable_order_entry_response_message: return "Enable Order Entry Response Message";
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

    static constexpr const char* name = "sequenced_message_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<sequenced_message_type::enum_type>;
    using storage_type = result_type;

    constexpr sequenced_message_type()
     : value{ enum_type{} } {}

    constexpr sequenced_message_type(enum_type v)
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
            set(enum_type::enable_order_entry_response_message);
    }

  protected:
    enum_type value;
};
}
