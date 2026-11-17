#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

// peg
struct peg {

    enum class enum_type : char {
        unpegged = 'N',
        midpoint = 'M',
        primary = 'R',
        discretionary = 'd',
        corporate_discretionary = 'z',
        market_maker = 'Q',
        offset = 'o',
        market = 'P'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 8> from_string_map = {{
        {"Corporate Discretionary", enum_type::corporate_discretionary},
        {"Discretionary", enum_type::discretionary},
        {"Market", enum_type::market},
        {"Market Maker", enum_type::market_maker},
        {"Midpoint", enum_type::midpoint},
        {"Offset", enum_type::offset},
        {"Primary", enum_type::primary},
        {"Unpegged", enum_type::unpegged}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::unpegged: return "Unpegged";
            case enum_type::midpoint: return "Midpoint";
            case enum_type::primary: return "Primary";
            case enum_type::discretionary: return "Discretionary";
            case enum_type::corporate_discretionary: return "Corporate Discretionary";
            case enum_type::market_maker: return "Market Maker";
            case enum_type::offset: return "Offset";
            case enum_type::market: return "Market";
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
            set(enum_type::market);
    }

  protected:
    enum_type value;
};
}
