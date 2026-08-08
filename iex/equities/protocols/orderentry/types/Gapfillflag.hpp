#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// gapfillflag
struct gapfillflag {

    enum class enum_type : char {
        gap_fill_message = 'Y',
        sequence_reset = 'N'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Gap fill message", enum_type::gap_fill_message},
        {"Sequence reset", enum_type::sequence_reset}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::gap_fill_message: return "Gap fill message";
            case enum_type::sequence_reset: return "Sequence reset";
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

    static constexpr const char* name = "gapfillflag";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<gapfillflag::enum_type>;
    using storage_type = result_type;

    constexpr gapfillflag()
     : value{ enum_type::gap_fill_message } {}

    constexpr gapfillflag(enum_type v)
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
            set(enum_type::sequence_reset);
    }

  protected:
    enum_type value;
};
}
