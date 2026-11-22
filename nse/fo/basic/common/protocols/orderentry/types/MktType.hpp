#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// mkt_type
struct mkt_type {

    enum class enum_type : char {
        normal_market = '1',
        odd_lot_market = '2',
        spot_market = '3',
        auction_market = '4'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Auction Market", enum_type::auction_market},
        {"Normal Market", enum_type::normal_market},
        {"Odd Lot Market", enum_type::odd_lot_market},
        {"Spot Market", enum_type::spot_market}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::normal_market: return "Normal Market";
            case enum_type::odd_lot_market: return "Odd Lot Market";
            case enum_type::spot_market: return "Spot Market";
            case enum_type::auction_market: return "Auction Market";
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

    static constexpr const char* name = "mkt_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<mkt_type::enum_type>;
    using storage_type = result_type;

    constexpr mkt_type()
     : value{ enum_type{} } {}

    constexpr mkt_type(enum_type v)
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
            set(enum_type::auction_market);
    }

  protected:
    enum_type value;
};
}
