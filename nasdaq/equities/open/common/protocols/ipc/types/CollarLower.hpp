#pragma once

#include <cstddef>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// collar_lower
struct collar_lower {

    static constexpr const char* name = "collar_lower";
    static constexpr std::size_t size = 8;
    static constexpr bool is_optional = false;
    static constexpr int exponent = -4;

    using result_type = required<std::int64_t>;
    using storage_type = result_type;

    constexpr collar_lower()
     : value{ 0 } {}

    constexpr collar_lower(std::int64_t v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(std::int64_t v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    std::int64_t value;
};
}
