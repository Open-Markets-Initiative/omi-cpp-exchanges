#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// liquidity_flag
struct liquidity_flag {

    enum class enum_type : char {
        added = 'A',
        closing = 'C',
        retail_designated = 'e',
        halt_ipo = 'H',
        after_hours_closing = 'i',
        nondisplayed_adding_liquidity = 'J',
        rpi_order_provides_liquidity = 'j',
        added_liquidity_via_a_midpoint_order = 'k',
        halt_cross = 'K',
        closing_cross = 'L',
        opening_cross = 'M',
        removed_liquidity_at_a_midpoint = 'm',
        passive_midpoint_execution = 'N',
        midpoint_extended_life_order_execution = 'n',
        opening = 'O',
        removed_price_improving_nondisplayed_liquidity = 'p',
        rmo_retail_order_removes_non_rpi_midpoint_liquidity = 'q',
        removed = 'R',
        retail_order_removes_rpi_liquidity = 'r',
        retail_order_removes_price_improving_nondisplayed_liquidity_other_than_rpi_liquidity = 't',
        added_nondisplayed_liquidity_via_a_reserve_order = 'u',
        supplemental = '0',
        displayed_liquidityadding_order_improves_the_nbbo = '7',
        displayed_liquidityadding_order_sets_the_qbbo_while_joining_the_nbbo = '8',
        rpi_order_provides_liquidity_no_rpii = '1',
        added_in_the_premarket = '2',
        removed_in_the_premarket = '3',
        displayed_liquidityadding_order_improves_the_nbbo_in_the_premarket = '5',
        nondisplayed_adding_liquidity_in_the_premarket = '9'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 29> from_string_map = {{
        {"Added", enum_type::added},
        {"Added In The Premarket", enum_type::added_in_the_premarket},
        {"Added Liquidity Via A Midpoint Order", enum_type::added_liquidity_via_a_midpoint_order},
        {"Added Nondisplayed Liquidity Via A Reserve Order", enum_type::added_nondisplayed_liquidity_via_a_reserve_order},
        {"After Hours Closing", enum_type::after_hours_closing},
        {"Closing", enum_type::closing},
        {"Closing Cross", enum_type::closing_cross},
        {"Displayed Liquidityadding Order Improves The Nbbo", enum_type::displayed_liquidityadding_order_improves_the_nbbo},
        {"Displayed Liquidityadding Order Improves The Nbbo In The Premarket", enum_type::displayed_liquidityadding_order_improves_the_nbbo_in_the_premarket},
        {"Displayed Liquidityadding Order Sets The Qbbo While Joining The Nbbo", enum_type::displayed_liquidityadding_order_sets_the_qbbo_while_joining_the_nbbo},
        {"Halt Cross", enum_type::halt_cross},
        {"Halt Ipo", enum_type::halt_ipo},
        {"Midpoint Extended Life Order Execution", enum_type::midpoint_extended_life_order_execution},
        {"Nondisplayed Adding Liquidity", enum_type::nondisplayed_adding_liquidity},
        {"Nondisplayed Adding Liquidity In The Premarket", enum_type::nondisplayed_adding_liquidity_in_the_premarket},
        {"Opening", enum_type::opening},
        {"Opening Cross", enum_type::opening_cross},
        {"Passive Midpoint Execution", enum_type::passive_midpoint_execution},
        {"Removed", enum_type::removed},
        {"Removed In The Premarket", enum_type::removed_in_the_premarket},
        {"Removed Liquidity At A Midpoint", enum_type::removed_liquidity_at_a_midpoint},
        {"Removed Price Improving Nondisplayed Liquidity", enum_type::removed_price_improving_nondisplayed_liquidity},
        {"Retail Designated", enum_type::retail_designated},
        {"Retail Order Removes Price Improving Nondisplayed Liquidity Other Than Rpi Liquidity", enum_type::retail_order_removes_price_improving_nondisplayed_liquidity_other_than_rpi_liquidity},
        {"Retail Order Removes Rpi Liquidity", enum_type::retail_order_removes_rpi_liquidity},
        {"Rmo Retail Order Removes Non Rpi Midpoint Liquidity", enum_type::rmo_retail_order_removes_non_rpi_midpoint_liquidity},
        {"Rpi Order Provides Liquidity", enum_type::rpi_order_provides_liquidity},
        {"Rpi Order Provides Liquidity No Rpii", enum_type::rpi_order_provides_liquidity_no_rpii},
        {"Supplemental", enum_type::supplemental}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::added: return "Added";
            case enum_type::closing: return "Closing";
            case enum_type::retail_designated: return "Retail Designated";
            case enum_type::halt_ipo: return "Halt Ipo";
            case enum_type::after_hours_closing: return "After Hours Closing";
            case enum_type::nondisplayed_adding_liquidity: return "Nondisplayed Adding Liquidity";
            case enum_type::rpi_order_provides_liquidity: return "Rpi Order Provides Liquidity";
            case enum_type::added_liquidity_via_a_midpoint_order: return "Added Liquidity Via A Midpoint Order";
            case enum_type::halt_cross: return "Halt Cross";
            case enum_type::closing_cross: return "Closing Cross";
            case enum_type::opening_cross: return "Opening Cross";
            case enum_type::removed_liquidity_at_a_midpoint: return "Removed Liquidity At A Midpoint";
            case enum_type::passive_midpoint_execution: return "Passive Midpoint Execution";
            case enum_type::midpoint_extended_life_order_execution: return "Midpoint Extended Life Order Execution";
            case enum_type::opening: return "Opening";
            case enum_type::removed_price_improving_nondisplayed_liquidity: return "Removed Price Improving Nondisplayed Liquidity";
            case enum_type::rmo_retail_order_removes_non_rpi_midpoint_liquidity: return "Rmo Retail Order Removes Non Rpi Midpoint Liquidity";
            case enum_type::removed: return "Removed";
            case enum_type::retail_order_removes_rpi_liquidity: return "Retail Order Removes Rpi Liquidity";
            case enum_type::retail_order_removes_price_improving_nondisplayed_liquidity_other_than_rpi_liquidity: return "Retail Order Removes Price Improving Nondisplayed Liquidity Other Than Rpi Liquidity";
            case enum_type::added_nondisplayed_liquidity_via_a_reserve_order: return "Added Nondisplayed Liquidity Via A Reserve Order";
            case enum_type::supplemental: return "Supplemental";
            case enum_type::displayed_liquidityadding_order_improves_the_nbbo: return "Displayed Liquidityadding Order Improves The Nbbo";
            case enum_type::displayed_liquidityadding_order_sets_the_qbbo_while_joining_the_nbbo: return "Displayed Liquidityadding Order Sets The Qbbo While Joining The Nbbo";
            case enum_type::rpi_order_provides_liquidity_no_rpii: return "Rpi Order Provides Liquidity No Rpii";
            case enum_type::added_in_the_premarket: return "Added In The Premarket";
            case enum_type::removed_in_the_premarket: return "Removed In The Premarket";
            case enum_type::displayed_liquidityadding_order_improves_the_nbbo_in_the_premarket: return "Displayed Liquidityadding Order Improves The Nbbo In The Premarket";
            case enum_type::nondisplayed_adding_liquidity_in_the_premarket: return "Nondisplayed Adding Liquidity In The Premarket";
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

    static constexpr const char* name = "liquidity_flag";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<liquidity_flag::enum_type>;
    using storage_type = result_type;

    constexpr liquidity_flag()
     : value{ enum_type{} } {}

    constexpr liquidity_flag(enum_type v)
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
            set(enum_type::nondisplayed_adding_liquidity_in_the_premarket);
    }

  protected:
    enum_type value;
};
}
