#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// exectype
struct exectype {

    enum class enum_type : char {
        new_value = '0',
        partial_fill = '1',
        filled = '2',
        canceled = '4',
        replaced = '5',
        pending_cancel = '6',
        rejected = '8',
        pending_new = 'A',
        restated = 'D'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 9> from_string_map = {{
        {"Canceled", enum_type::canceled},
        {"Filled", enum_type::filled},
        {"New", enum_type::new_value},
        {"Partial Fill", enum_type::partial_fill},
        {"Pending Cancel", enum_type::pending_cancel},
        {"Pending New", enum_type::pending_new},
        {"Rejected", enum_type::rejected},
        {"Replaced", enum_type::replaced},
        {"Restated", enum_type::restated}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::new_value: return "New";
            case enum_type::partial_fill: return "Partial Fill";
            case enum_type::filled: return "Filled";
            case enum_type::canceled: return "Canceled";
            case enum_type::replaced: return "Replaced";
            case enum_type::pending_cancel: return "Pending Cancel";
            case enum_type::rejected: return "Rejected";
            case enum_type::pending_new: return "Pending New";
            case enum_type::restated: return "Restated";
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
            set(enum_type::restated);
    }

  protected:
    enum_type value;
};
}
