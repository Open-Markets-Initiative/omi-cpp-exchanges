#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// liquidityflag
struct liquidityflag {

    enum class enum_type : char {
        added_in_the_premarket = '2',
        removed_in_the_premarket = '3',
        displayed_liquidityadding_order_improves_the_nbbo_in_the_premarket = '5',
        displayed_liquidityadding_order_improves_the_nbbo = '7',
        displayed_liquidityadding_order_sets_the_qbbo_while_joining_the_nbbo = '8',
        nondisplayed_adding_liquidity_in_the_premarket = '9',
        added = 'A',
        routed_to_bx = 'B',
        closing_cross = 'C',
        dot = 'D',
        nyse_other = 'E',
        opening_trade_on_nyse = 'F',
        onclose_order_on_nyse = 'G',
        haltipo_cross = 'H',
        nondisplayed_adding_liquidity = 'J',
        halt_cross = 'K',
        closing_cross_imbalanceonly = 'L',
        open_cross_imbalanceonly = 'M',
        passive_midpoint_execution = 'N',
        open_cross = 'O',
        routed_to_psx = 'P',
        removed = 'R',
        odd_lot_execution_on_nyse = 'S',
        executed_in_open_close_or_reopen_on_arca = 'T',
        added_liquidity_on_nyse = 'U',
        added_postonly = 'W',
        routed = 'X',
        rerouted_by_nyse = 'Y',
        retail_designated_execution_that_removed_liquidity = 'd',
        retail_designated_execution_that_added_displayed_liquidity = 'e',
        retail_designated_execution_that_added_nondisplayed_liquidity = 'f',
        extended_trading_close_execution = 'i',
        rpi_retail_price_improving_order_provides_liquidity = 'j',
        added_liquidity_via_a_midpoint_order = 'k',
        removed_liquidity_at_a_midpoint = 'm',
        midpoint_extended_life_order_execution = 'n',
        retail_order_removes_rpi_liquidity = 'r',
        retail_order_removes_price_improving_nondisplayed_liquidity_other_than_rpi_liquidity = 't',
        added_nondisplayed_liquidity_via_a_reserve_order = 'u',
        midp_order_executed_on_away_nms_protected_market_center = 'v',
        midp_order_executed_on_offexchange_venue = 'w'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 41> from_string_map = {{
        {"Added", enum_type::added},
        {"Added in the Pre-Market", enum_type::added_in_the_premarket},
        {"Added Liquidity (on NYSE)", enum_type::added_liquidity_on_nyse},
        {"Added Liquidity via a Midpoint Order", enum_type::added_liquidity_via_a_midpoint_order},
        {"Added Non-Displayed Liquidity via a Reserve Order", enum_type::added_nondisplayed_liquidity_via_a_reserve_order},
        {"Added Post-Only", enum_type::added_postonly},
        {"Closing Cross", enum_type::closing_cross},
        {"Closing Cross (imbalance-only)", enum_type::closing_cross_imbalanceonly},
        {"Displayed, Liquidity-Adding Order Improves the NBBO", enum_type::displayed_liquidityadding_order_improves_the_nbbo},
        {"Displayed, Liquidity-Adding Order Improves the NBBO in the Pre-Market", enum_type::displayed_liquidityadding_order_improves_the_nbbo_in_the_premarket},
        {"Displayed, Liquidity-Adding Order Sets the QBBO While Joining the NBBO", enum_type::displayed_liquidityadding_order_sets_the_qbbo_while_joining_the_nbbo},
        {"DOT", enum_type::dot},
        {"Executed in Open, Close, or Re-open on ARCA", enum_type::executed_in_open_close_or_reopen_on_arca},
        {"Extended Trading Close Execution", enum_type::extended_trading_close_execution},
        {"Halt Cross", enum_type::halt_cross},
        {"Halt/IPO Cross", enum_type::haltipo_cross},
        {"MIDP Order Executed on Away NMS Protected Market Center", enum_type::midp_order_executed_on_away_nms_protected_market_center},
        {"MIDP Order Executed on Off-Exchange Venue", enum_type::midp_order_executed_on_offexchange_venue},
        {"Midpoint Extended Life Order Execution", enum_type::midpoint_extended_life_order_execution},
        {"Non-Displayed Adding Liquidity", enum_type::nondisplayed_adding_liquidity},
        {"Non-Displayed Adding Liquidity in the Pre-Market", enum_type::nondisplayed_adding_liquidity_in_the_premarket},
        {"NYSE Other", enum_type::nyse_other},
        {"Odd Lot Execution (on NYSE)", enum_type::odd_lot_execution_on_nyse},
        {"On-Close Order (on NYSE)", enum_type::onclose_order_on_nyse},
        {"Open Cross", enum_type::open_cross},
        {"Open Cross (imbalance-only)", enum_type::open_cross_imbalanceonly},
        {"Opening Trade (on NYSE)", enum_type::opening_trade_on_nyse},
        {"Passive Midpoint Execution", enum_type::passive_midpoint_execution},
        {"Re-Routed by NYSE", enum_type::rerouted_by_nyse},
        {"Removed", enum_type::removed},
        {"Removed in the Pre-Market", enum_type::removed_in_the_premarket},
        {"Removed Liquidity at a Midpoint", enum_type::removed_liquidity_at_a_midpoint},
        {"Retail Designated Execution That Added Displayed Liquidity", enum_type::retail_designated_execution_that_added_displayed_liquidity},
        {"Retail Designated Execution That Added Non-Displayed Liquidity", enum_type::retail_designated_execution_that_added_nondisplayed_liquidity},
        {"Retail Designated Execution That Removed Liquidity", enum_type::retail_designated_execution_that_removed_liquidity},
        {"Retail Order Removes Price Improving Non-Displayed Liquidity Other Than RPI Liquidity", enum_type::retail_order_removes_price_improving_nondisplayed_liquidity_other_than_rpi_liquidity},
        {"Retail Order Removes RPI Liquidity", enum_type::retail_order_removes_rpi_liquidity},
        {"Routed", enum_type::routed},
        {"Routed to BX", enum_type::routed_to_bx},
        {"Routed to PSX", enum_type::routed_to_psx},
        {"RPI (Retail Price Improving) Order Provides Liquidity", enum_type::rpi_retail_price_improving_order_provides_liquidity}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::added_in_the_premarket: return "Added in the Pre-Market";
            case enum_type::removed_in_the_premarket: return "Removed in the Pre-Market";
            case enum_type::displayed_liquidityadding_order_improves_the_nbbo_in_the_premarket: return "Displayed, Liquidity-Adding Order Improves the NBBO in the Pre-Market";
            case enum_type::displayed_liquidityadding_order_improves_the_nbbo: return "Displayed, Liquidity-Adding Order Improves the NBBO";
            case enum_type::displayed_liquidityadding_order_sets_the_qbbo_while_joining_the_nbbo: return "Displayed, Liquidity-Adding Order Sets the QBBO While Joining the NBBO";
            case enum_type::nondisplayed_adding_liquidity_in_the_premarket: return "Non-Displayed Adding Liquidity in the Pre-Market";
            case enum_type::added: return "Added";
            case enum_type::routed_to_bx: return "Routed to BX";
            case enum_type::closing_cross: return "Closing Cross";
            case enum_type::dot: return "DOT";
            case enum_type::nyse_other: return "NYSE Other";
            case enum_type::opening_trade_on_nyse: return "Opening Trade (on NYSE)";
            case enum_type::onclose_order_on_nyse: return "On-Close Order (on NYSE)";
            case enum_type::haltipo_cross: return "Halt/IPO Cross";
            case enum_type::nondisplayed_adding_liquidity: return "Non-Displayed Adding Liquidity";
            case enum_type::halt_cross: return "Halt Cross";
            case enum_type::closing_cross_imbalanceonly: return "Closing Cross (imbalance-only)";
            case enum_type::open_cross_imbalanceonly: return "Open Cross (imbalance-only)";
            case enum_type::passive_midpoint_execution: return "Passive Midpoint Execution";
            case enum_type::open_cross: return "Open Cross";
            case enum_type::routed_to_psx: return "Routed to PSX";
            case enum_type::removed: return "Removed";
            case enum_type::odd_lot_execution_on_nyse: return "Odd Lot Execution (on NYSE)";
            case enum_type::executed_in_open_close_or_reopen_on_arca: return "Executed in Open, Close, or Re-open on ARCA";
            case enum_type::added_liquidity_on_nyse: return "Added Liquidity (on NYSE)";
            case enum_type::added_postonly: return "Added Post-Only";
            case enum_type::routed: return "Routed";
            case enum_type::rerouted_by_nyse: return "Re-Routed by NYSE";
            case enum_type::retail_designated_execution_that_removed_liquidity: return "Retail Designated Execution That Removed Liquidity";
            case enum_type::retail_designated_execution_that_added_displayed_liquidity: return "Retail Designated Execution That Added Displayed Liquidity";
            case enum_type::retail_designated_execution_that_added_nondisplayed_liquidity: return "Retail Designated Execution That Added Non-Displayed Liquidity";
            case enum_type::extended_trading_close_execution: return "Extended Trading Close Execution";
            case enum_type::rpi_retail_price_improving_order_provides_liquidity: return "RPI (Retail Price Improving) Order Provides Liquidity";
            case enum_type::added_liquidity_via_a_midpoint_order: return "Added Liquidity via a Midpoint Order";
            case enum_type::removed_liquidity_at_a_midpoint: return "Removed Liquidity at a Midpoint";
            case enum_type::midpoint_extended_life_order_execution: return "Midpoint Extended Life Order Execution";
            case enum_type::retail_order_removes_rpi_liquidity: return "Retail Order Removes RPI Liquidity";
            case enum_type::retail_order_removes_price_improving_nondisplayed_liquidity_other_than_rpi_liquidity: return "Retail Order Removes Price Improving Non-Displayed Liquidity Other Than RPI Liquidity";
            case enum_type::added_nondisplayed_liquidity_via_a_reserve_order: return "Added Non-Displayed Liquidity via a Reserve Order";
            case enum_type::midp_order_executed_on_away_nms_protected_market_center: return "MIDP Order Executed on Away NMS Protected Market Center";
            case enum_type::midp_order_executed_on_offexchange_venue: return "MIDP Order Executed on Off-Exchange Venue";
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

    static constexpr const char* name = "liquidityflag";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<liquidityflag::enum_type>;
    using storage_type = result_type;

    constexpr liquidityflag()
     : value{ enum_type{} } {}

    constexpr liquidityflag(enum_type v)
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
            set(enum_type::midp_order_executed_on_offexchange_venue);
    }

  protected:
    enum_type value;
};
}
