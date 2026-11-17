#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// log_time
struct log_time {

    static constexpr const char* name = "log_time";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = true;
    static constexpr std::int32_t null_value = 0;

    using result_type = std::optional<std::int32_t>;
    using storage_type = result_type;

    log_time()
     : value{ static_cast<std::int32_t>(__builtin_bswap32(null_value)) } {}

    log_time(std::int32_t v)
     : value{ static_cast<std::int32_t>(__builtin_bswap32(v)) } {}

    [[nodiscard]] result_type get() const {
        auto host = static_cast<std::int32_t>(__builtin_bswap32(value));
        return host == null_value ? std::nullopt : result_type{host};
    }

    void set(std::int32_t v) {
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
    std::int32_t value;
};
}
