#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// exectranstype
struct exectranstype {

    enum class enum_type : char {
        new_value = '0',
        cancel = '1',
        pending_new = 'A'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"CANCEL", enum_type::cancel},
        {"NEW", enum_type::new_value},
        {"Pending new", enum_type::pending_new}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::new_value: return "NEW";
            case enum_type::cancel: return "CANCEL";
            case enum_type::pending_new: return "Pending new";
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

    static constexpr const char* name = "exectranstype";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<exectranstype::enum_type>;
    using storage_type = result_type;

    constexpr exectranstype()
     : value{ enum_type::new_value } {}

    constexpr exectranstype(enum_type v)
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
            set(enum_type::pending_new);
    }

  protected:
    enum_type value;
};
}
