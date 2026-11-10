#pragma once

#include <cstddef>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// requesting_party_id_source
struct requesting_party_id_source {

    static constexpr const char* name = "requesting_party_id_source";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<char>;
    using storage_type = result_type;

    constexpr requesting_party_id_source()
     : value{ 0 } {}

    constexpr requesting_party_id_source(char v)
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
