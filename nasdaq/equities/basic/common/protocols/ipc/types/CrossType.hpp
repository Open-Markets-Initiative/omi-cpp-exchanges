#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// cross_type
struct cross_type {

    enum class enum_type : char {
        continuous = 'N',
        opening = 'O',
        closing = 'C',
        halt = 'H',
        supplemental = 'S',
        retail = 'R',
        extended_life = 'E',
        after_hours_close = 'A'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 8> from_string_map = {{
        {"After Hours Close", enum_type::after_hours_close},
        {"Closing", enum_type::closing},
        {"Continuous", enum_type::continuous},
        {"Extended Life", enum_type::extended_life},
        {"Halt", enum_type::halt},
        {"Opening", enum_type::opening},
        {"Retail", enum_type::retail},
        {"Supplemental", enum_type::supplemental}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::continuous: return "Continuous";
            case enum_type::opening: return "Opening";
            case enum_type::closing: return "Closing";
            case enum_type::halt: return "Halt";
            case enum_type::supplemental: return "Supplemental";
            case enum_type::retail: return "Retail";
            case enum_type::extended_life: return "Extended Life";
            case enum_type::after_hours_close: return "After Hours Close";
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

    static constexpr const char* name = "cross_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<cross_type::enum_type>;
    using storage_type = result_type;

    constexpr cross_type()
     : value{ enum_type{} } {}

    constexpr cross_type(enum_type v)
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
            set(enum_type::after_hours_close);
    }

  protected:
    enum_type value;
};
}
