#pragma once

#include <cstddef>
#include <array>
#include <span>
#include <algorithm>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// reserved_52
struct reserved_52 {

    static constexpr const char* name = "reserved_52";
    static constexpr std::size_t size = 52;
    static constexpr bool is_optional = false;

    using result_type = required<const std::array<std::uint8_t, 52>&>;
    using storage_type = result_type;

    constexpr reserved_52()
     : value{} {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(std::span<const std::uint8_t> src) {
        auto len = std::min(src.size(), value.size());
        std::copy_n(src.begin(), len, value.begin());
    }

    constexpr void set(result_type v) {
        if (v.has_value())
            set(std::span<const std::uint8_t>(v.value().data(), v.value().size()));
    }

  protected:
    std::array<std::uint8_t, 52> value;
};
}
