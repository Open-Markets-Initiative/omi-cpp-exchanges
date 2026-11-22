#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// displayexecinst
struct displayexecinst {

    enum class enum_type : char {
        inav_peg = 'I',
        mid_peg = 'M',
        market_peg = 'P',
        quoting_peg = 'Q',
        primary_peg = 'R'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"INAV Peg", enum_type::inav_peg},
        {"Market Peg", enum_type::market_peg},
        {"Mid Peg", enum_type::mid_peg},
        {"Primary Peg", enum_type::primary_peg},
        {"Quoting Peg", enum_type::quoting_peg}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::inav_peg: return "INAV Peg";
            case enum_type::mid_peg: return "Mid Peg";
            case enum_type::market_peg: return "Market Peg";
            case enum_type::quoting_peg: return "Quoting Peg";
            case enum_type::primary_peg: return "Primary Peg";
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

    static constexpr const char* name = "displayexecinst";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<displayexecinst::enum_type>;
    using storage_type = result_type;

    constexpr displayexecinst()
     : value{ enum_type{} } {}

    constexpr displayexecinst(enum_type v)
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
            set(enum_type::primary_peg);
    }

  protected:
    enum_type value;
};
}
