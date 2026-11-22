#pragma once

#include <cstddef>
#include <cstdint>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// number_of_industry_records
struct number_of_industry_records {

    static constexpr const char* name = "number_of_industry_records";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<std::int16_t>;
    using storage_type = result_type;

    constexpr number_of_industry_records()
     : value{ 0 } {}

    number_of_industry_records(std::int16_t v)
     : value{ static_cast<std::int16_t>(__builtin_bswap16(v)) } {}

    [[nodiscard]] result_type get() const {
        return result_type{static_cast<std::int16_t>(__builtin_bswap16(value))};
    }

    void set(std::int16_t v) {
        value = __builtin_bswap16(v);
    }

    void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(0);
    }

  protected:
    std::int16_t value;
};
}
