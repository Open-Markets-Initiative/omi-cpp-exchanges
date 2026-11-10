#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// exec_restatement_reason
struct exec_restatement_reason {

    enum class enum_type : std::uint8_t {
        no_value = 255,
        market_exchange_option = 8,
        cancelled_not_best = 9,
        cancel_on_disconnect = 100,
        self_match_prevention_oldest_order_cancelled = 103,
        cancel_on_globex_credit_controls_violation = 104,
        cancel_from_firmsoft = 105,
        cancel_from_risk_management_api = 106,
        self_match_prevention_newest_order_cancelled = 107,
        cancelduetovolquotedoptionorderrestedqtylessthanminordersize = 108,
        cancel_rfc_order = 109,
        cancel_upon_contract_expiration = 110,
        system_cancel = 111
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 13> from_string_map = {{
        {"Cancel From Firmsoft", enum_type::cancel_from_firmsoft},
        {"Cancel From Risk Management Api", enum_type::cancel_from_risk_management_api},
        {"Cancel On Disconnect", enum_type::cancel_on_disconnect},
        {"Cancel On Globex Credit Controls Violation", enum_type::cancel_on_globex_credit_controls_violation},
        {"Cancel Rfc Order", enum_type::cancel_rfc_order},
        {"Cancel Upon Contract Expiration", enum_type::cancel_upon_contract_expiration},
        {"Cancelduetovolquotedoptionorderrestedqtylessthanminordersize", enum_type::cancelduetovolquotedoptionorderrestedqtylessthanminordersize},
        {"Cancelled Not Best", enum_type::cancelled_not_best},
        {"Market Exchange Option", enum_type::market_exchange_option},
        {"No Value", enum_type::no_value},
        {"Self Match Prevention Newest Order Cancelled", enum_type::self_match_prevention_newest_order_cancelled},
        {"Self Match Prevention Oldest Order Cancelled", enum_type::self_match_prevention_oldest_order_cancelled},
        {"System Cancel", enum_type::system_cancel}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::no_value: return "No Value";
            case enum_type::market_exchange_option: return "Market Exchange Option";
            case enum_type::cancelled_not_best: return "Cancelled Not Best";
            case enum_type::cancel_on_disconnect: return "Cancel On Disconnect";
            case enum_type::self_match_prevention_oldest_order_cancelled: return "Self Match Prevention Oldest Order Cancelled";
            case enum_type::cancel_on_globex_credit_controls_violation: return "Cancel On Globex Credit Controls Violation";
            case enum_type::cancel_from_firmsoft: return "Cancel From Firmsoft";
            case enum_type::cancel_from_risk_management_api: return "Cancel From Risk Management Api";
            case enum_type::self_match_prevention_newest_order_cancelled: return "Self Match Prevention Newest Order Cancelled";
            case enum_type::cancelduetovolquotedoptionorderrestedqtylessthanminordersize: return "Cancelduetovolquotedoptionorderrestedqtylessthanminordersize";
            case enum_type::cancel_rfc_order: return "Cancel Rfc Order";
            case enum_type::cancel_upon_contract_expiration: return "Cancel Upon Contract Expiration";
            case enum_type::system_cancel: return "System Cancel";
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

    static constexpr const char* name = "exec_restatement_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<exec_restatement_reason::enum_type>;
    using storage_type = result_type;

    constexpr exec_restatement_reason()
     : value{ static_cast<enum_type>(255) } {}

    constexpr exec_restatement_reason(enum_type v)
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
