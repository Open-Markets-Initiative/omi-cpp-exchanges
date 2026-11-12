#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// cxlrejreason
struct cxlrejreason {

    enum class enum_type : std::int32_t {
        too_late_to_cancel = 0,
        unknown_order = 1,
        duplicate_clordid_11_received = 6,
        price_exceeds_current_price_band = 8,
        other = 99
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Duplicate ClOrdID (11) received", enum_type::duplicate_clordid_11_received},
        {"Other", enum_type::other},
        {"Price exceeds current price band", enum_type::price_exceeds_current_price_band},
        {"Too late to cancel", enum_type::too_late_to_cancel},
        {"Unknown order", enum_type::unknown_order}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::too_late_to_cancel: return "Too late to cancel";
            case enum_type::unknown_order: return "Unknown order";
            case enum_type::duplicate_clordid_11_received: return "Duplicate ClOrdID (11) received";
            case enum_type::price_exceeds_current_price_band: return "Price exceeds current price band";
            case enum_type::other: return "Other";
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
            set(enum_type::other);
    }

  protected:
    enum_type value;
};
}
