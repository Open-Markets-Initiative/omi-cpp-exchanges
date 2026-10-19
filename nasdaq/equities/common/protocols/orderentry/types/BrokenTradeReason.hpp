#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// broken_trade_reason
struct broken_trade_reason {

    enum class enum_type : char {
        erroneous = 'E',
        consent = 'C',
        supervisory = 'S',
        external = 'X'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Consent", enum_type::consent},
        {"Erroneous", enum_type::erroneous},
        {"External", enum_type::external},
        {"Supervisory", enum_type::supervisory}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::erroneous: return "Erroneous";
            case enum_type::consent: return "Consent";
            case enum_type::supervisory: return "Supervisory";
            case enum_type::external: return "External";
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

    static constexpr const char* name = "broken_trade_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<broken_trade_reason::enum_type>;
    using storage_type = result_type;

    constexpr broken_trade_reason()
     : value{ enum_type{} } {}

    constexpr broken_trade_reason(enum_type v)
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
            set(enum_type::external);
    }

  protected:
    enum_type value;
};
}
