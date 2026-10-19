#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// st_broker_eligibility_per_mkt
struct st_broker_eligibility_per_mkt {

    using type = uint16_t;

    static constexpr type pre_open = type{1} << 0;
    static constexpr type auction_market = type{1} << 12;
    static constexpr type spot_market = type{1} << 13;
    static constexpr type oddlot_market = type{1} << 14;
    static constexpr type normal_market = type{1} << 15;

    static constexpr const char* name = "st_broker_eligibility_per_mkt";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<uint16_t>;
    using storage_type = result_type;

    constexpr st_broker_eligibility_per_mkt()
     : value{ 0 } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(uint16_t v) {
        value = v;
    }

    constexpr void set(result_type v) {
        if (v.has_value())
            set(v.value());
        else
            set(type{0});
    }

    [[nodiscard]] constexpr bool has(type bit) const { return (value & bit) != 0; }
    constexpr void set(type bit, bool enabled) { if (enabled) value |= bit; else value &= static_cast<type>(~bit); }

    [[nodiscard]] constexpr bool has_pre_open() const { return has(pre_open); }
    [[nodiscard]] constexpr bool has_auction_market() const { return has(auction_market); }
    [[nodiscard]] constexpr bool has_spot_market() const { return has(spot_market); }
    [[nodiscard]] constexpr bool has_oddlot_market() const { return has(oddlot_market); }
    [[nodiscard]] constexpr bool has_normal_market() const { return has(normal_market); }

    constexpr void set_pre_open(bool v) { set(pre_open, v); }
    constexpr void set_auction_market(bool v) { set(auction_market, v); }
    constexpr void set_spot_market(bool v) { set(spot_market, v); }
    constexpr void set_oddlot_market(bool v) { set(oddlot_market, v); }
    constexpr void set_normal_market(bool v) { set(normal_market, v); }

  protected:
    type value;
};
}
