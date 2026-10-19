#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// aiq_strategy
struct aiq_strategy {

    enum class enum_type : char {
        use_port_default = '*',
        disabled = 'N',
        firm_level_decrement_both_no_details = 'Y',
        firm_level_decrement_both = 'D',
        firm_level_cancel_oldest = 'O',
        firm_level_cancel_newest = 'W',
        firm_level_use_remover = 'R',
        organization_level_decrement_both_no_details = 'y',
        organization_level_decrement_both = 'd',
        organization_level_cancel_oldest = 'o',
        organization_level_cancel_newest = 'w',
        organization_level_use_remover = 'r',
        affiliate_level_decrement_both_no_details = 'Z',
        affiliate_level_decrement_both = 'E',
        affiliate_level_cancel_oldest = 'P',
        affiliate_level_cancel_newest = 'X',
        affiliate_level_use_remover = 'S',
        match_any_level_decrement_both_no_details = '0',
        match_any_level_decrement_both = '1',
        match_any_level_cancel_oldest = '2',
        match_any_level_cancel_newest = '4',
        match_any_level_use_remover = '5'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 22> from_string_map = {{
        {"Affiliate Level Cancel Newest", enum_type::affiliate_level_cancel_newest},
        {"Affiliate Level Cancel Oldest", enum_type::affiliate_level_cancel_oldest},
        {"Affiliate Level Decrement Both", enum_type::affiliate_level_decrement_both},
        {"Affiliate Level Decrement Both No Details", enum_type::affiliate_level_decrement_both_no_details},
        {"Affiliate Level Use Remover", enum_type::affiliate_level_use_remover},
        {"Disabled", enum_type::disabled},
        {"Firm Level Cancel Newest", enum_type::firm_level_cancel_newest},
        {"Firm Level Cancel Oldest", enum_type::firm_level_cancel_oldest},
        {"Firm Level Decrement Both", enum_type::firm_level_decrement_both},
        {"Firm Level Decrement Both No Details", enum_type::firm_level_decrement_both_no_details},
        {"Firm Level Use Remover", enum_type::firm_level_use_remover},
        {"Match Any Level Cancel Newest", enum_type::match_any_level_cancel_newest},
        {"Match Any Level Cancel Oldest", enum_type::match_any_level_cancel_oldest},
        {"Match Any Level Decrement Both", enum_type::match_any_level_decrement_both},
        {"Match Any Level Decrement Both No Details", enum_type::match_any_level_decrement_both_no_details},
        {"Match Any Level Use Remover", enum_type::match_any_level_use_remover},
        {"Organization Level Cancel Newest", enum_type::organization_level_cancel_newest},
        {"Organization Level Cancel Oldest", enum_type::organization_level_cancel_oldest},
        {"Organization Level Decrement Both", enum_type::organization_level_decrement_both},
        {"Organization Level Decrement Both No Details", enum_type::organization_level_decrement_both_no_details},
        {"Organization Level Use Remover", enum_type::organization_level_use_remover},
        {"Use Port Default", enum_type::use_port_default}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::use_port_default: return "Use Port Default";
            case enum_type::disabled: return "Disabled";
            case enum_type::firm_level_decrement_both_no_details: return "Firm Level Decrement Both No Details";
            case enum_type::firm_level_decrement_both: return "Firm Level Decrement Both";
            case enum_type::firm_level_cancel_oldest: return "Firm Level Cancel Oldest";
            case enum_type::firm_level_cancel_newest: return "Firm Level Cancel Newest";
            case enum_type::firm_level_use_remover: return "Firm Level Use Remover";
            case enum_type::organization_level_decrement_both_no_details: return "Organization Level Decrement Both No Details";
            case enum_type::organization_level_decrement_both: return "Organization Level Decrement Both";
            case enum_type::organization_level_cancel_oldest: return "Organization Level Cancel Oldest";
            case enum_type::organization_level_cancel_newest: return "Organization Level Cancel Newest";
            case enum_type::organization_level_use_remover: return "Organization Level Use Remover";
            case enum_type::affiliate_level_decrement_both_no_details: return "Affiliate Level Decrement Both No Details";
            case enum_type::affiliate_level_decrement_both: return "Affiliate Level Decrement Both";
            case enum_type::affiliate_level_cancel_oldest: return "Affiliate Level Cancel Oldest";
            case enum_type::affiliate_level_cancel_newest: return "Affiliate Level Cancel Newest";
            case enum_type::affiliate_level_use_remover: return "Affiliate Level Use Remover";
            case enum_type::match_any_level_decrement_both_no_details: return "Match Any Level Decrement Both No Details";
            case enum_type::match_any_level_decrement_both: return "Match Any Level Decrement Both";
            case enum_type::match_any_level_cancel_oldest: return "Match Any Level Cancel Oldest";
            case enum_type::match_any_level_cancel_newest: return "Match Any Level Cancel Newest";
            case enum_type::match_any_level_use_remover: return "Match Any Level Use Remover";
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

    static constexpr const char* name = "aiq_strategy";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<aiq_strategy::enum_type>;
    using storage_type = result_type;

    constexpr aiq_strategy()
     : value{ enum_type{} } {}

    constexpr aiq_strategy(enum_type v)
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
            set(enum_type::match_any_level_use_remover);
    }

  protected:
    enum_type value;
};
}
