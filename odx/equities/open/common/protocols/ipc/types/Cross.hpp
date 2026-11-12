#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace odx::odxequities::matchingengine::ipc::v2026 {

// cross
struct cross {

    static constexpr const char* name = "cross";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<char>;
    using storage_type = result_type;

    constexpr cross()
     : value{ 0 } {}

    constexpr cross(char v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(char v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    char value;
};
}
