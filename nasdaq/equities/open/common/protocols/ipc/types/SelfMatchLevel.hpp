#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// self_match_level
struct self_match_level {

    enum class enum_type : char {
        firm = 'F',
        organization = 'O',
        affiliate = 'A',
        any = 'M',
        none = 'N'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Affiliate", enum_type::affiliate},
        {"Any", enum_type::any},
        {"Firm", enum_type::firm},
        {"None", enum_type::none},
        {"Organization", enum_type::organization}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::firm: return "Firm";
            case enum_type::organization: return "Organization";
            case enum_type::affiliate: return "Affiliate";
            case enum_type::any: return "Any";
            case enum_type::none: return "None";
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

    static constexpr const char* name = "self_match_level";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<self_match_level::enum_type>;
    using storage_type = result_type;

    constexpr self_match_level()
     : value{ enum_type{} } {}

    constexpr self_match_level(enum_type v)
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
            set(enum_type::none);
    }

  protected:
    enum_type value;
};
}
