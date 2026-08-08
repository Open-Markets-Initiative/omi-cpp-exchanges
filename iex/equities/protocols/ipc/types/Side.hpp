#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

// side
struct side {

    static constexpr const char* name = "side";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<char>;
    using storage_type = result_type;

    constexpr side()
     : value{ 0 } {}

    constexpr side(char v)
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
