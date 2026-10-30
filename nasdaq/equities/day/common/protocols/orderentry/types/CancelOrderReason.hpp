#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// cancel_order_reason
struct cancel_order_reason {

    enum class enum_type : char {
        user_requested_cancel = 'U',
        immediate_or_cancel_order = 'I',
        timeout = 'T',
        supervisory = 'S',
        regulatory_restriction = 'D',
        self_match_prevention = 'Q',
        system_cancel = 'Z',
        this_order_cannot_be_executed = 'K',
        halted = 'H',
        open_protection = 'X',
        closed = 'E',
        post_only_cancel = 'F',
        post_only_cancel_contra_side_displayed = 'G',
        company_direct_listing_capital_raise_order_exceeds_allowable_shares_offered = 'e'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 14> from_string_map = {{
        {"Closed", enum_type::closed},
        {"Company Direct Listing Capital Raise Order Exceeds Allowable Shares Offered", enum_type::company_direct_listing_capital_raise_order_exceeds_allowable_shares_offered},
        {"Halted", enum_type::halted},
        {"Immediate Or Cancel Order", enum_type::immediate_or_cancel_order},
        {"Open Protection", enum_type::open_protection},
        {"Post Only Cancel", enum_type::post_only_cancel},
        {"Post Only Cancel Contra Side Displayed", enum_type::post_only_cancel_contra_side_displayed},
        {"Regulatory Restriction", enum_type::regulatory_restriction},
        {"Self Match Prevention", enum_type::self_match_prevention},
        {"Supervisory", enum_type::supervisory},
        {"System Cancel", enum_type::system_cancel},
        {"This Order Cannot Be Executed", enum_type::this_order_cannot_be_executed},
        {"Timeout", enum_type::timeout},
        {"User Requested Cancel", enum_type::user_requested_cancel}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::user_requested_cancel: return "User Requested Cancel";
            case enum_type::immediate_or_cancel_order: return "Immediate Or Cancel Order";
            case enum_type::timeout: return "Timeout";
            case enum_type::supervisory: return "Supervisory";
            case enum_type::regulatory_restriction: return "Regulatory Restriction";
            case enum_type::self_match_prevention: return "Self Match Prevention";
            case enum_type::system_cancel: return "System Cancel";
            case enum_type::this_order_cannot_be_executed: return "This Order Cannot Be Executed";
            case enum_type::halted: return "Halted";
            case enum_type::open_protection: return "Open Protection";
            case enum_type::closed: return "Closed";
            case enum_type::post_only_cancel: return "Post Only Cancel";
            case enum_type::post_only_cancel_contra_side_displayed: return "Post Only Cancel Contra Side Displayed";
            case enum_type::company_direct_listing_capital_raise_order_exceeds_allowable_shares_offered: return "Company Direct Listing Capital Raise Order Exceeds Allowable Shares Offered";
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

    static constexpr const char* name = "cancel_order_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<cancel_order_reason::enum_type>;
    using storage_type = result_type;

    constexpr cancel_order_reason()
     : value{ enum_type{} } {}

    constexpr cancel_order_reason(enum_type v)
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
            set(enum_type::company_direct_listing_capital_raise_order_exceeds_allowable_shares_offered);
    }

  protected:
    enum_type value;
};
}
