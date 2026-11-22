#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// cxlrejreason
struct cxlrejreason {

    enum class enum_type : std::int32_t {
        too_late_to_cancel = 0,
        unknown_order = 1
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Too Late to Cancel", enum_type::too_late_to_cancel},
        {"Unknown Order", enum_type::unknown_order}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::too_late_to_cancel: return "Too Late to Cancel";
            case enum_type::unknown_order: return "Unknown Order";
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

    static constexpr const char* name = "cxlrejreason";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<cxlrejreason::enum_type>;
    using storage_type = result_type;

    constexpr cxlrejreason()
     : value{ enum_type{} } {}

    constexpr cxlrejreason(enum_type v)
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
            set(enum_type::unknown_order);
    }

  protected:
    enum_type value;
};
}
