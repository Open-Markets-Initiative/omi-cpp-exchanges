#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// book_type
struct book_type {

    enum class enum_type : std::int16_t {
        regular_lot_order = 1,
        special_terms_order = 2,
        stop_loss_or_mit_order = 3,
        negotiated_order = 4,
        odd_lot_order = 5,
        spot_order = 6,
        auction_order = 7
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 7> from_string_map = {{
        {"Auction Order", enum_type::auction_order},
        {"Negotiated Order", enum_type::negotiated_order},
        {"Odd Lot Order", enum_type::odd_lot_order},
        {"Regular Lot Order", enum_type::regular_lot_order},
        {"Special Terms Order", enum_type::special_terms_order},
        {"Spot Order", enum_type::spot_order},
        {"Stop Loss Or Mit Order", enum_type::stop_loss_or_mit_order}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::regular_lot_order: return "Regular Lot Order";
            case enum_type::special_terms_order: return "Special Terms Order";
            case enum_type::stop_loss_or_mit_order: return "Stop Loss Or Mit Order";
            case enum_type::negotiated_order: return "Negotiated Order";
            case enum_type::odd_lot_order: return "Odd Lot Order";
            case enum_type::spot_order: return "Spot Order";
            case enum_type::auction_order: return "Auction Order";
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

    static constexpr const char* name = "book_type";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<book_type::enum_type>;
    using storage_type = result_type;

    constexpr book_type()
     : value{ enum_type{} } {}

    constexpr book_type(enum_type v)
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
            set(enum_type::auction_order);
    }

  protected:
    enum_type value;
};
}
