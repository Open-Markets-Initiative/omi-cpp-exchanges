#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdint>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// exec_inst
struct exec_inst {

    using type = uint8_t;

    static constexpr type aon = type{1} << 0;
    static constexpr type ob = type{1} << 1;
    static constexpr type nh = type{1} << 2;
    static constexpr type exec_inst_reserved_1 = type{1} << 3;
    static constexpr type exec_inst_reserved_2 = type{1} << 4;
    static constexpr type exec_inst_reserved_3 = type{1} << 5;
    static constexpr type exec_inst_reserved_4 = type{1} << 6;
    static constexpr type exec_inst_reserved_5 = type{1} << 7;

    static constexpr const char* name = "exec_inst";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<uint8_t>;
    using storage_type = result_type;

    constexpr exec_inst()
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

    [[nodiscard]] constexpr bool has_aon() const { return has(aon); }
    [[nodiscard]] constexpr bool has_ob() const { return has(ob); }
    [[nodiscard]] constexpr bool has_nh() const { return has(nh); }
    [[nodiscard]] constexpr bool has_exec_inst_reserved_1() const { return has(exec_inst_reserved_1); }
    [[nodiscard]] constexpr bool has_exec_inst_reserved_2() const { return has(exec_inst_reserved_2); }
    [[nodiscard]] constexpr bool has_exec_inst_reserved_3() const { return has(exec_inst_reserved_3); }
    [[nodiscard]] constexpr bool has_exec_inst_reserved_4() const { return has(exec_inst_reserved_4); }
    [[nodiscard]] constexpr bool has_exec_inst_reserved_5() const { return has(exec_inst_reserved_5); }

    constexpr void set_aon(bool v) { set(aon, v); }
    constexpr void set_ob(bool v) { set(ob, v); }
    constexpr void set_nh(bool v) { set(nh, v); }
    constexpr void set_exec_inst_reserved_1(bool v) { set(exec_inst_reserved_1, v); }
    constexpr void set_exec_inst_reserved_2(bool v) { set(exec_inst_reserved_2, v); }
    constexpr void set_exec_inst_reserved_3(bool v) { set(exec_inst_reserved_3, v); }
    constexpr void set_exec_inst_reserved_4(bool v) { set(exec_inst_reserved_4, v); }
    constexpr void set_exec_inst_reserved_5(bool v) { set(exec_inst_reserved_5, v); }

  protected:
    type value;
};
}
