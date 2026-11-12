#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::matchingengine::ipc::v2026 {

// refusal_reason
struct refusal_reason {

    enum class enum_type : char {
        other = 'O',
        not_allowed = 'R',
        type = 'T'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"Not Allowed", enum_type::not_allowed},
        {"Other", enum_type::other},
        {"Type", enum_type::type}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::other: return "Other";
            case enum_type::not_allowed: return "Not Allowed";
            case enum_type::type: return "Type";
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

    static constexpr const char* name = "refusal_reason";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<refusal_reason::enum_type>;
    using storage_type = result_type;

    constexpr refusal_reason()
     : value{ enum_type{} } {}

    constexpr refusal_reason(enum_type v)
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
            set(enum_type::type);
    }

  protected:
    enum_type value;
};
}
