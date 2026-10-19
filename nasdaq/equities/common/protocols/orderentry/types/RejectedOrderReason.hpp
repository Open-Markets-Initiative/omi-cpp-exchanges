#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// rejected_order_reason
struct rejected_order_reason {

    enum class enum_type : std::uint16_t {
        quote_unavailable = 0x0001,
        destination_closed = 0x0002,
        invalid_display = 0x0003,
        invalid_max_floor = 0x0004,
        invalid_peg_type = 0x0005,
        fat_finger = 0x0006,
        halted = 0x0007,
        iso_not_allowed = 0x0008,
        invalid_side = 0x0009,
        processing_error = 0x000A,
        cancel_pending = 0x000B,
        firm_not_authorized = 0x000C,
        invalid_min_quantity = 0x000D,
        no_closing_reference_price = 0x000E,
        other = 0x000F,
        cancel_not_allowed = 0x0010,
        pegging_not_allowed = 0x0011,
        crossed_market = 0x0012,
        invalid_quantity = 0x0013,
        invalid_cross_order = 0x0014,
        replace_not_allowed = 0x0015,
        routing_not_allowed = 0x0016,
        invalid_symbol = 0x0017,
        test = 0x0018,
        late_loc_too_aggressive = 0x0019,
        retail_not_allowed = 0x001A,
        invalid_midpoint_post_only_price = 0x001B,
        invalid_destination = 0x001C,
        invalid_price = 0x001D,
        shares_exceed_threshold = 0x001E,
        exceeds_maximum_allowed_notional_value = 0x001F,
        risk_aggregate_exposure_exceeded = 0x0020,
        risk_market_impact = 0x0021,
        risk_restricted_stock = 0x0022,
        risk_short_sell_restricted = 0x0023,
        risk_iso_not_allowed = 0x0024,
        risk_exceeds_adv_limit = 0x0025,
        risk_fat_finger = 0x0026,
        risk_locate_required = 0x0027,
        risk_symbol_message_rate_restriction = 0x0028,
        risk_port_message_rate_restriction = 0x0029,
        risk_duplicate_message_rate_restriction = 0x002A,
        risk_short_sell_not_allowed = 0x002B,
        risk_market_order_not_allowed = 0x002C,
        risk_pre_market_not_allowed = 0x002D,
        risk_post_market_not_allowed = 0x002E,
        risk_short_sell_exempt_not_allowed = 0x002F,
        risk_single_order_notional_exceeded = 0x0030,
        risk_max_quantity_exceeded = 0x0031,
        reg_sho_state_not_available = 0x0032,
        risk_ipo_market_buy_not_allowed = 0x0033,
        invalid_aiq = 0x0040
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 52> from_string_map = {{
        {"Cancel Not Allowed", enum_type::cancel_not_allowed},
        {"Cancel Pending", enum_type::cancel_pending},
        {"Crossed Market", enum_type::crossed_market},
        {"Destination Closed", enum_type::destination_closed},
        {"Exceeds Maximum Allowed Notional Value", enum_type::exceeds_maximum_allowed_notional_value},
        {"Fat Finger", enum_type::fat_finger},
        {"Firm Not Authorized", enum_type::firm_not_authorized},
        {"Halted", enum_type::halted},
        {"Invalid Aiq", enum_type::invalid_aiq},
        {"Invalid Cross Order", enum_type::invalid_cross_order},
        {"Invalid Destination", enum_type::invalid_destination},
        {"Invalid Display", enum_type::invalid_display},
        {"Invalid Max Floor", enum_type::invalid_max_floor},
        {"Invalid Midpoint Post Only Price", enum_type::invalid_midpoint_post_only_price},
        {"Invalid Min Quantity", enum_type::invalid_min_quantity},
        {"Invalid Peg Type", enum_type::invalid_peg_type},
        {"Invalid Price", enum_type::invalid_price},
        {"Invalid Quantity", enum_type::invalid_quantity},
        {"Invalid Side", enum_type::invalid_side},
        {"Invalid Symbol", enum_type::invalid_symbol},
        {"Iso Not Allowed", enum_type::iso_not_allowed},
        {"Late Loc Too Aggressive", enum_type::late_loc_too_aggressive},
        {"No Closing Reference Price", enum_type::no_closing_reference_price},
        {"Other", enum_type::other},
        {"Pegging Not Allowed", enum_type::pegging_not_allowed},
        {"Processing Error", enum_type::processing_error},
        {"Quote Unavailable", enum_type::quote_unavailable},
        {"Reg Sho State Not Available", enum_type::reg_sho_state_not_available},
        {"Replace Not Allowed", enum_type::replace_not_allowed},
        {"Retail Not Allowed", enum_type::retail_not_allowed},
        {"Risk Aggregate Exposure Exceeded", enum_type::risk_aggregate_exposure_exceeded},
        {"Risk Duplicate Message Rate Restriction", enum_type::risk_duplicate_message_rate_restriction},
        {"Risk Exceeds Adv Limit", enum_type::risk_exceeds_adv_limit},
        {"Risk Fat Finger", enum_type::risk_fat_finger},
        {"Risk Ipo Market Buy Not Allowed", enum_type::risk_ipo_market_buy_not_allowed},
        {"Risk Iso Not Allowed", enum_type::risk_iso_not_allowed},
        {"Risk Locate Required", enum_type::risk_locate_required},
        {"Risk Market Impact", enum_type::risk_market_impact},
        {"Risk Market Order Not Allowed", enum_type::risk_market_order_not_allowed},
        {"Risk Max Quantity Exceeded", enum_type::risk_max_quantity_exceeded},
        {"Risk Port Message Rate Restriction", enum_type::risk_port_message_rate_restriction},
        {"Risk Post Market Not Allowed", enum_type::risk_post_market_not_allowed},
        {"Risk Pre Market Not Allowed", enum_type::risk_pre_market_not_allowed},
        {"Risk Restricted Stock", enum_type::risk_restricted_stock},
        {"Risk Short Sell Exempt Not Allowed", enum_type::risk_short_sell_exempt_not_allowed},
        {"Risk Short Sell Not Allowed", enum_type::risk_short_sell_not_allowed},
        {"Risk Short Sell Restricted", enum_type::risk_short_sell_restricted},
        {"Risk Single Order Notional Exceeded", enum_type::risk_single_order_notional_exceeded},
        {"Risk Symbol Message Rate Restriction", enum_type::risk_symbol_message_rate_restriction},
        {"Routing Not Allowed", enum_type::routing_not_allowed},
        {"Shares Exceed Threshold", enum_type::shares_exceed_threshold},
        {"Test", enum_type::test}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::quote_unavailable: return "Quote Unavailable";
            case enum_type::destination_closed: return "Destination Closed";
            case enum_type::invalid_display: return "Invalid Display";
            case enum_type::invalid_max_floor: return "Invalid Max Floor";
            case enum_type::invalid_peg_type: return "Invalid Peg Type";
            case enum_type::fat_finger: return "Fat Finger";
            case enum_type::halted: return "Halted";
            case enum_type::iso_not_allowed: return "Iso Not Allowed";
            case enum_type::invalid_side: return "Invalid Side";
            case enum_type::processing_error: return "Processing Error";
            case enum_type::cancel_pending: return "Cancel Pending";
            case enum_type::firm_not_authorized: return "Firm Not Authorized";
            case enum_type::invalid_min_quantity: return "Invalid Min Quantity";
            case enum_type::no_closing_reference_price: return "No Closing Reference Price";
            case enum_type::other: return "Other";
            case enum_type::cancel_not_allowed: return "Cancel Not Allowed";
            case enum_type::pegging_not_allowed: return "Pegging Not Allowed";
            case enum_type::crossed_market: return "Crossed Market";
            case enum_type::invalid_quantity: return "Invalid Quantity";
            case enum_type::invalid_cross_order: return "Invalid Cross Order";
            case enum_type::replace_not_allowed: return "Replace Not Allowed";
            case enum_type::routing_not_allowed: return "Routing Not Allowed";
            case enum_type::invalid_symbol: return "Invalid Symbol";
            case enum_type::test: return "Test";
            case enum_type::late_loc_too_aggressive: return "Late Loc Too Aggressive";
            case enum_type::retail_not_allowed: return "Retail Not Allowed";
            case enum_type::invalid_midpoint_post_only_price: return "Invalid Midpoint Post Only Price";
            case enum_type::invalid_destination: return "Invalid Destination";
            case enum_type::invalid_price: return "Invalid Price";
            case enum_type::shares_exceed_threshold: return "Shares Exceed Threshold";
            case enum_type::exceeds_maximum_allowed_notional_value: return "Exceeds Maximum Allowed Notional Value";
            case enum_type::risk_aggregate_exposure_exceeded: return "Risk Aggregate Exposure Exceeded";
            case enum_type::risk_market_impact: return "Risk Market Impact";
            case enum_type::risk_restricted_stock: return "Risk Restricted Stock";
            case enum_type::risk_short_sell_restricted: return "Risk Short Sell Restricted";
            case enum_type::risk_iso_not_allowed: return "Risk Iso Not Allowed";
            case enum_type::risk_exceeds_adv_limit: return "Risk Exceeds Adv Limit";
            case enum_type::risk_fat_finger: return "Risk Fat Finger";
            case enum_type::risk_locate_required: return "Risk Locate Required";
            case enum_type::risk_symbol_message_rate_restriction: return "Risk Symbol Message Rate Restriction";
            case enum_type::risk_port_message_rate_restriction: return "Risk Port Message Rate Restriction";
            case enum_type::risk_duplicate_message_rate_restriction: return "Risk Duplicate Message Rate Restriction";
            case enum_type::risk_short_sell_not_allowed: return "Risk Short Sell Not Allowed";
            case enum_type::risk_market_order_not_allowed: return "Risk Market Order Not Allowed";
            case enum_type::risk_pre_market_not_allowed: return "Risk Pre Market Not Allowed";
            case enum_type::risk_post_market_not_allowed: return "Risk Post Market Not Allowed";
            case enum_type::risk_short_sell_exempt_not_allowed: return "Risk Short Sell Exempt Not Allowed";
            case enum_type::risk_single_order_notional_exceeded: return "Risk Single Order Notional Exceeded";
            case enum_type::risk_max_quantity_exceeded: return "Risk Max Quantity Exceeded";
            case enum_type::reg_sho_state_not_available: return "Reg Sho State Not Available";
            case enum_type::risk_ipo_market_buy_not_allowed: return "Risk Ipo Market Buy Not Allowed";
            case enum_type::invalid_aiq: return "Invalid Aiq";
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

    static constexpr const char* name = "rejected_order_reason";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<rejected_order_reason::enum_type>;
    using storage_type = result_type;

    constexpr rejected_order_reason()
     : value{ enum_type{} } {}

    constexpr rejected_order_reason(enum_type v)
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
            set(enum_type::invalid_aiq);
    }

  protected:
    enum_type value;
};
}
