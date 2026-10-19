#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// additional_order_flags
struct additional_order_flags {

    using type = uint8_t;

    static constexpr type boc = type{1} << 0;
    static constexpr type col = type{1} << 1;
    static constexpr type stpc = type{1} << 4;

    static constexpr const char* name = "additional_order_flags";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<uint8_t>;
    using storage_type = result_type;

    constexpr additional_order_flags()
     : value{ 0 } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(uint8_t v) {
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

    [[nodiscard]] constexpr bool has_boc() const { return has(boc); }
    [[nodiscard]] constexpr bool has_col() const { return has(col); }
    [[nodiscard]] constexpr bool has_stpc() const { return has(stpc); }

    constexpr void set_boc(bool v) { set(boc, v); }
    constexpr void set_col(bool v) { set(col, v); }
    constexpr void set_stpc(bool v) { set(stpc, v); }

  protected:
    type value;
};
}
