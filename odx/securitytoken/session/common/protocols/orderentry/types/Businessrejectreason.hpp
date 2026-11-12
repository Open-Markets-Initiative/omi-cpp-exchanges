#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// businessrejectreason
struct businessrejectreason {

    enum class enum_type : std::int32_t {
        throttle_limit_exceeded = 99
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 1> from_string_map = {{
        {"Throttle Limit Exceeded", enum_type::throttle_limit_exceeded}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::throttle_limit_exceeded: return "Throttle Limit Exceeded";
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

    static constexpr const char* name = "businessrejectreason";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<businessrejectreason::enum_type>;
    using storage_type = result_type;

    constexpr businessrejectreason()
     : value{ enum_type{} } {}

    constexpr businessrejectreason(enum_type v)
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
            set(enum_type::throttle_limit_exceeded);
    }

  protected:
    enum_type value;
};
}
