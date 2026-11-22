#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// displayinst
struct displayinst {

    enum class enum_type : char {
        attributable_price_to_display = 'A',
        melo_and_continuous_book_midpoint = 'B',
        imbalance_only = 'I',
        postonly_and_attributable_price_to_display = 'L',
        midpoint_peg_tag_18_must_be_n = 'M',
        nondisplay_required_for_midpoint_orders = 'N',
        retail_order_type_1 = 'O',
        postonly = 'P',
        retail_price_improvement_order = 'Q',
        retail_order_type_2 = 'T',
        midpoint_peg_post_only = 'W',
        anonymous_price_to_comply = 'Y',
        direct_listing_capital_raise = 'd',
        midpoint_peg_and_trade_now = 'm',
        nondisplay_and_trade_now = 'n'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 15> from_string_map = {{
        {"Anonymous - Price to Comply", enum_type::anonymous_price_to_comply},
        {"Attributable - Price to Display", enum_type::attributable_price_to_display},
        {"Direct Listing Capital Raise", enum_type::direct_listing_capital_raise},
        {"Imbalance Only", enum_type::imbalance_only},
        {"M-ELO and Continuous Book Midpoint", enum_type::melo_and_continuous_book_midpoint},
        {"Mid-Point Peg (tag 18 must be N)", enum_type::midpoint_peg_tag_18_must_be_n},
        {"Mid-Point Peg and Trade Now", enum_type::midpoint_peg_and_trade_now},
        {"Mid-Point Peg Post Only", enum_type::midpoint_peg_post_only},
        {"Non-Display (required for midpoint orders)", enum_type::nondisplay_required_for_midpoint_orders},
        {"Non-Display and Trade Now", enum_type::nondisplay_and_trade_now},
        {"Post-Only", enum_type::postonly},
        {"Post-Only and Attributable - Price to Display", enum_type::postonly_and_attributable_price_to_display},
        {"Retail Order Type 1", enum_type::retail_order_type_1},
        {"Retail Order Type 2", enum_type::retail_order_type_2},
        {"Retail Price Improvement Order", enum_type::retail_price_improvement_order}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::attributable_price_to_display: return "Attributable - Price to Display";
            case enum_type::melo_and_continuous_book_midpoint: return "M-ELO and Continuous Book Midpoint";
            case enum_type::imbalance_only: return "Imbalance Only";
            case enum_type::postonly_and_attributable_price_to_display: return "Post-Only and Attributable - Price to Display";
            case enum_type::midpoint_peg_tag_18_must_be_n: return "Mid-Point Peg (tag 18 must be N)";
            case enum_type::nondisplay_required_for_midpoint_orders: return "Non-Display (required for midpoint orders)";
            case enum_type::retail_order_type_1: return "Retail Order Type 1";
            case enum_type::postonly: return "Post-Only";
            case enum_type::retail_price_improvement_order: return "Retail Price Improvement Order";
            case enum_type::retail_order_type_2: return "Retail Order Type 2";
            case enum_type::midpoint_peg_post_only: return "Mid-Point Peg Post Only";
            case enum_type::anonymous_price_to_comply: return "Anonymous - Price to Comply";
            case enum_type::direct_listing_capital_raise: return "Direct Listing Capital Raise";
            case enum_type::midpoint_peg_and_trade_now: return "Mid-Point Peg and Trade Now";
            case enum_type::nondisplay_and_trade_now: return "Non-Display and Trade Now";
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

    static constexpr const char* name = "displayinst";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<displayinst::enum_type>;
    using storage_type = result_type;

    constexpr displayinst()
     : value{ enum_type{} } {}

    constexpr displayinst(enum_type v)
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
            set(enum_type::nondisplay_and_trade_now);
    }

  protected:
    enum_type value;
};
}
