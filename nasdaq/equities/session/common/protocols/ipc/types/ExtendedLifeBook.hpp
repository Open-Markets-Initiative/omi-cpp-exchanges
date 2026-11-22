#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// extended_life_book
struct extended_life_book {

    enum class enum_type : char {
        alone = 'N',
        continuous = 'B'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Alone", enum_type::alone},
        {"Continuous", enum_type::continuous}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::alone: return "Alone";
            case enum_type::continuous: return "Continuous";
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

    static constexpr const char* name = "extended_life_book";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<extended_life_book::enum_type>;
    using storage_type = result_type;

    constexpr extended_life_book()
     : value{ enum_type{} } {}

    constexpr extended_life_book(enum_type v)
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
            set(enum_type::continuous);
    }

  protected:
    enum_type value;
};
}
