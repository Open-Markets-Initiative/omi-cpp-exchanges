#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

// sequenced_message_type
struct sequenced_message_type {

    enum class enum_type : char {
        system_event_message = 'S',
        add_order_message = 'A',
        add_order_mpid_attribution_message = 'F',
        stock_directory_message = 'R',
        stock_trading_action_message = 'H',
        reg_sho_restriction_message = 'Y',
        retail_interest_message = 'N',
        end_of_snapshot_message = 'G',
        operational_halt_message = 'h'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 9> from_string_map = {{
        {"Add Order Message", enum_type::add_order_message},
        {"Add Order Mpid Attribution Message", enum_type::add_order_mpid_attribution_message},
        {"End Of Snapshot Message", enum_type::end_of_snapshot_message},
        {"Operational Halt Message", enum_type::operational_halt_message},
        {"Reg Sho Restriction Message", enum_type::reg_sho_restriction_message},
        {"Retail Interest Message", enum_type::retail_interest_message},
        {"Stock Directory Message", enum_type::stock_directory_message},
        {"Stock Trading Action Message", enum_type::stock_trading_action_message},
        {"System Event Message", enum_type::system_event_message}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::system_event_message: return "System Event Message";
            case enum_type::add_order_message: return "Add Order Message";
            case enum_type::add_order_mpid_attribution_message: return "Add Order Mpid Attribution Message";
            case enum_type::stock_directory_message: return "Stock Directory Message";
            case enum_type::stock_trading_action_message: return "Stock Trading Action Message";
            case enum_type::reg_sho_restriction_message: return "Reg Sho Restriction Message";
            case enum_type::retail_interest_message: return "Retail Interest Message";
            case enum_type::end_of_snapshot_message: return "End Of Snapshot Message";
            case enum_type::operational_halt_message: return "Operational Halt Message";
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
            set(enum_type::operational_halt_message);
    }

  protected:
    enum_type value;
};
}
