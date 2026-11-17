#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// peg
struct peg {

    enum class enum_type : char {
        limit = 'L',
        midpoint = 'M',
        market_peg = 'P',
        primary_peg = 'R',
        market_maker_peg = 'Q',
        midpoint_unexplained = 'm'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 6> from_string_map = {{
        {"Limit", enum_type::limit},
        {"Market Maker Peg", enum_type::market_maker_peg},
        {"Market Peg", enum_type::market_peg},
        {"Midpoint", enum_type::midpoint},
        {"Midpoint Unexplained", enum_type::midpoint_unexplained},
        {"Primary Peg", enum_type::primary_peg}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::limit: return "Limit";
            case enum_type::midpoint: return "Midpoint";
            case enum_type::market_peg: return "Market Peg";
            case enum_type::primary_peg: return "Primary Peg";
            case enum_type::market_maker_peg: return "Market Maker Peg";
            case enum_type::midpoint_unexplained: return "Midpoint Unexplained";
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

    static constexpr const char* name = "peg";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<peg::enum_type>;
    using storage_type = result_type;

    constexpr peg()
     : value{ enum_type{} } {}

    constexpr peg(enum_type v)
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
            set(enum_type::midpoint_unexplained);
    }

  protected:
    enum_type value;
};
}
