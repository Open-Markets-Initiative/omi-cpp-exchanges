#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// exectype
struct exectype {

    enum class enum_type : char {
        new_value = '0',
        partial_fill = '1',
        fill = '2',
        cancelled = '4',
        replaced = '5',
        rejected = '8'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 6> from_string_map = {{
        {"Cancelled", enum_type::cancelled},
        {"Fill", enum_type::fill},
        {"New", enum_type::new_value},
        {"Partial fill", enum_type::partial_fill},
        {"Rejected", enum_type::rejected},
        {"Replaced", enum_type::replaced}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::new_value: return "New";
            case enum_type::partial_fill: return "Partial fill";
            case enum_type::fill: return "Fill";
            case enum_type::cancelled: return "Cancelled";
            case enum_type::replaced: return "Replaced";
            case enum_type::rejected: return "Rejected";
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
            set(enum_type::rejected);
    }

  protected:
    enum_type value;
};
}
