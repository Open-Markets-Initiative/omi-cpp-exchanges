#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// aiq_canceled_optional_field
struct aiq_canceled_optional_field {

    enum class enum_type : std::uint8_t {
        userrefidx = 28
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 1> from_string_map = {{
        {"UserRefIdx", enum_type::userrefidx}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::userrefidx: return "UserRefIdx";
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

    static constexpr const char* name = "aiq_canceled_optional_field";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<aiq_canceled_optional_field::enum_type>;
    using storage_type = result_type;

    constexpr aiq_canceled_optional_field()
     : value{ enum_type{} } {}

    constexpr aiq_canceled_optional_field(enum_type v)
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
            set(enum_type::userrefidx);
    }

  protected:
    enum_type value;
};
}
