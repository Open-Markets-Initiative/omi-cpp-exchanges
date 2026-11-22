#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// refusal_reason
struct refusal_reason {

    enum class enum_type : char {
        closed = 'E',
        invalid_peg_type = 'P',
        market_not_allowed = 'T',
        other = 'O',
        invalid_cross = 'C',
        post_only_slid = 'G',
        no_reference = 'N',
        too_aggressive = 'A',
        fat_finger = 'F',
        limit_order_protection = 'L'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 10> from_string_map = {{
        {"Closed", enum_type::closed},
        {"Fat Finger", enum_type::fat_finger},
        {"Invalid Cross", enum_type::invalid_cross},
        {"Invalid Peg Type", enum_type::invalid_peg_type},
        {"Limit Order Protection", enum_type::limit_order_protection},
        {"Market Not Allowed", enum_type::market_not_allowed},
        {"No Reference", enum_type::no_reference},
        {"Other", enum_type::other},
        {"Post Only Slid", enum_type::post_only_slid},
        {"Too Aggressive", enum_type::too_aggressive}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::closed: return "Closed";
            case enum_type::invalid_peg_type: return "Invalid Peg Type";
            case enum_type::market_not_allowed: return "Market Not Allowed";
            case enum_type::other: return "Other";
            case enum_type::invalid_cross: return "Invalid Cross";
            case enum_type::post_only_slid: return "Post Only Slid";
            case enum_type::no_reference: return "No Reference";
            case enum_type::too_aggressive: return "Too Aggressive";
            case enum_type::fat_finger: return "Fat Finger";
            case enum_type::limit_order_protection: return "Limit Order Protection";
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

    static constexpr const char* name = "refusal_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<refusal_reason::enum_type>;
    using storage_type = result_type;

    constexpr refusal_reason()
     : value{ enum_type{} } {}

    constexpr refusal_reason(enum_type v)
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
            set(enum_type::limit_order_protection);
    }

  protected:
    enum_type value;
};
}
