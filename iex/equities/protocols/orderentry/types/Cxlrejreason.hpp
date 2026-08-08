#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// cxlrejreason
struct cxlrejreason {

    enum class enum_type : std::int32_t {
        too_late_to_cancel = 0,
        unknown_order = 1,
        exchange_option = 2,
        already_pending_cancel_or_pending_replace = 3
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Already pending cancel or pending replace", enum_type::already_pending_cancel_or_pending_replace},
        {"Exchange option", enum_type::exchange_option},
        {"Too late to cancel", enum_type::too_late_to_cancel},
        {"Unknown order", enum_type::unknown_order}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::too_late_to_cancel: return "Too late to cancel";
            case enum_type::unknown_order: return "Unknown order";
            case enum_type::exchange_option: return "Exchange option";
            case enum_type::already_pending_cancel_or_pending_replace: return "Already pending cancel or pending replace";
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
     : value{ enum_type::too_late_to_cancel } {}

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
            set(enum_type::already_pending_cancel_or_pending_replace);
    }

  protected:
    enum_type value;
};
}
