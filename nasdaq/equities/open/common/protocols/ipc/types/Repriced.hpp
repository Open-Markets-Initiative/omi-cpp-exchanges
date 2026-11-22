#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// repriced
struct repriced {

    enum class enum_type : char {
        standing = 'N',
        repriced = 'Y'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Repriced", enum_type::repriced},
        {"Standing", enum_type::standing}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::standing: return "Standing";
            case enum_type::repriced: return "Repriced";
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

    static constexpr const char* name = "repriced";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<repriced::enum_type>;
    using storage_type = result_type;

    constexpr repriced()
     : value{ enum_type{} } {}

    constexpr repriced(enum_type v)
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
            set(enum_type::repriced);
    }

  protected:
    enum_type value;
};
}
