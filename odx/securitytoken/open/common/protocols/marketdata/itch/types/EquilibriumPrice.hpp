#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::pts::itch::v1_2 {

// equilibrium_price
struct equilibrium_price {

    static constexpr const char* name = "equilibrium_price";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = true;
    static constexpr std::uint32_t null_value = 2147483647;
    static constexpr int exponent = -2;

    using result_type = std::optional<std::uint32_t>;
    using storage_type = result_type;

    equilibrium_price()
     : value{ static_cast<std::uint32_t>(__builtin_bswap32(null_value)) } {}

    equilibrium_price(std::uint32_t v)
     : value{ static_cast<std::uint32_t>(__builtin_bswap32(v)) } {}

    [[nodiscard]] result_type get() const {
        auto host = static_cast<std::uint32_t>(__builtin_bswap32(value));
        return host == null_value ? std::nullopt : result_type{host};
    }

    void set(std::uint32_t v) {
        value = __builtin_bswap32(v);
    }

    void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(null_value);
    }

    constexpr void set_null() {
        value = null_value;
    }

  protected:
    std::uint32_t value;
};
}
