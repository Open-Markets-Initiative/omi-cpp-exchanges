#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

// exectype
struct exectype {

    enum class enum_type : char {
        new_value = '0',
        partial_fill = '1',
        fill = '2',
        canceled = '4',
        replace = '5',
        pending_cancel = '6',
        rejected = '8',
        pending_new = 'A',
        restated = 'D',
        pending_replace = 'E'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 10> from_string_map = {{
        {"CANCELED", enum_type::canceled},
        {"FILL", enum_type::fill},
        {"NEW", enum_type::new_value},
        {"Partial fill", enum_type::partial_fill},
        {"Pending cancel", enum_type::pending_cancel},
        {"Pending new", enum_type::pending_new},
        {"Pending replace", enum_type::pending_replace},
        {"REJECTED", enum_type::rejected},
        {"REPLACE", enum_type::replace},
        {"RESTATED", enum_type::restated}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::new_value: return "NEW";
            case enum_type::partial_fill: return "Partial fill";
            case enum_type::fill: return "FILL";
            case enum_type::canceled: return "CANCELED";
            case enum_type::replace: return "REPLACE";
            case enum_type::pending_cancel: return "Pending cancel";
            case enum_type::rejected: return "REJECTED";
            case enum_type::pending_new: return "Pending new";
            case enum_type::restated: return "RESTATED";
            case enum_type::pending_replace: return "Pending replace";
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

    static constexpr const char* name = "exectype";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<exectype::enum_type>;
    using storage_type = result_type;

    constexpr exectype()
     : value{ enum_type{} } {}

    constexpr exectype(enum_type v)
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
            set(enum_type::pending_replace);
    }

  protected:
    enum_type value;
};
}
