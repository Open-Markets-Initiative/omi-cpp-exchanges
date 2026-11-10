#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// quote_cancel_type
struct quote_cancel_type {

    enum class enum_type : std::uint8_t {
        cancelper_instrument = 1,
        cancelper_instrumentgroup = 3,
        cancelallquotes = 4,
        cancelper_quote_set = 100
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Cancelallquotes", enum_type::cancelallquotes},
        {"Cancelper Instrument", enum_type::cancelper_instrument},
        {"Cancelper Instrumentgroup", enum_type::cancelper_instrumentgroup},
        {"Cancelper Quote Set", enum_type::cancelper_quote_set}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::cancelper_instrument: return "Cancelper Instrument";
            case enum_type::cancelper_instrumentgroup: return "Cancelper Instrumentgroup";
            case enum_type::cancelallquotes: return "Cancelallquotes";
            case enum_type::cancelper_quote_set: return "Cancelper Quote Set";
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

    static constexpr const char* name = "quote_cancel_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<quote_cancel_type::enum_type>;
    using storage_type = result_type;

    constexpr quote_cancel_type()
     : value{ enum_type{} } {}

    constexpr quote_cancel_type(enum_type v)
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
            set(enum_type::cancelper_quote_set);
    }

  protected:
    enum_type value;
};
}
