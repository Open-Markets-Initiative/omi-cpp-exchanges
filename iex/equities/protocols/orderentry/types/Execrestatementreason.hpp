#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// execrestatementreason
struct execrestatementreason {

    enum class enum_type : std::int32_t {
        repricing_of_order = 3,
        state_change = 4
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Repricing of order", enum_type::repricing_of_order},
        {"State change", enum_type::state_change}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::repricing_of_order: return "Repricing of order";
            case enum_type::state_change: return "State change";
            default: return "unknown";
        }
    }

    static constexpr std::optional<enum_type> from_string(std::string_view str) {
        auto it = std::lower_bound(
            from_string_map.begin(),
            from_string_map.end(),
            str,
            [](const auto& pair, std::string_view s) { return pair.first < s; }
        );
        if (it != from_string_map.end() && it->first == str) {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const char* name = "execrestatementreason";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<execrestatementreason::enum_type>;
    using storage_type = result_type;

    constexpr execrestatementreason()
     : value{ enum_type::repricing_of_order } {}

    constexpr execrestatementreason(enum_type v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(enum_type v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(enum_type::state_change);
    }

  protected:
    enum_type value;
};
}
