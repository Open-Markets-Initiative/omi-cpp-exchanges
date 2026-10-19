#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// st_order_flags
struct st_order_flags {

    using type = uint16_t;

    static constexpr type order_pre_open = type{1} << 2;
    static constexpr type frozen = type{1} << 3;
    static constexpr type modified = type{1} << 4;
    static constexpr type traded = type{1} << 5;
    static constexpr type matched_ind = type{1} << 6;
    static constexpr type mf = type{1} << 7;
    static constexpr type aon = type{1} << 8;
    static constexpr type ioc = type{1} << 9;
    static constexpr type gtc = type{1} << 10;
    static constexpr type day = type{1} << 11;
    static constexpr type mit = type{1} << 12;
    static constexpr type sl = type{1} << 13;
    static constexpr type market = type{1} << 14;
    static constexpr type ato = type{1} << 15;

    static constexpr const char* name = "st_order_flags";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<uint16_t>;
    using storage_type = result_type;

    constexpr st_order_flags()
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

    [[nodiscard]] constexpr bool has_order_pre_open() const { return has(order_pre_open); }
    [[nodiscard]] constexpr bool has_frozen() const { return has(frozen); }
    [[nodiscard]] constexpr bool has_modified() const { return has(modified); }
    [[nodiscard]] constexpr bool has_traded() const { return has(traded); }
    [[nodiscard]] constexpr bool has_matched_ind() const { return has(matched_ind); }
    [[nodiscard]] constexpr bool has_mf() const { return has(mf); }
    [[nodiscard]] constexpr bool has_aon() const { return has(aon); }
    [[nodiscard]] constexpr bool has_ioc() const { return has(ioc); }
    [[nodiscard]] constexpr bool has_gtc() const { return has(gtc); }
    [[nodiscard]] constexpr bool has_day() const { return has(day); }
    [[nodiscard]] constexpr bool has_mit() const { return has(mit); }
    [[nodiscard]] constexpr bool has_sl() const { return has(sl); }
    [[nodiscard]] constexpr bool has_market() const { return has(market); }
    [[nodiscard]] constexpr bool has_ato() const { return has(ato); }

    constexpr void set_order_pre_open(bool v) { set(order_pre_open, v); }
    constexpr void set_frozen(bool v) { set(frozen, v); }
    constexpr void set_modified(bool v) { set(modified, v); }
    constexpr void set_traded(bool v) { set(traded, v); }
    constexpr void set_matched_ind(bool v) { set(matched_ind, v); }
    constexpr void set_mf(bool v) { set(mf, v); }
    constexpr void set_aon(bool v) { set(aon, v); }
    constexpr void set_ioc(bool v) { set(ioc, v); }
    constexpr void set_gtc(bool v) { set(gtc, v); }
    constexpr void set_day(bool v) { set(day, v); }
    constexpr void set_mit(bool v) { set(mit, v); }
    constexpr void set_sl(bool v) { set(sl, v); }
    constexpr void set_market(bool v) { set(market, v); }
    constexpr void set_ato(bool v) { set(ato, v); }

  protected:
    type value;
};
}
