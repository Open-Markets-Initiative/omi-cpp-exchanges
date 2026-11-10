#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// cmta_giveup_cd
struct cmta_giveup_cd {

    enum class enum_type : char {
        give_up = 'G',
        sg_xoffset = 'S'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Give Up", enum_type::give_up},
        {"Sg Xoffset", enum_type::sg_xoffset}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::give_up: return "Give Up";
            case enum_type::sg_xoffset: return "Sg Xoffset";
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

    static constexpr const char* name = "cmta_giveup_cd";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<cmta_giveup_cd::enum_type>;
    using storage_type = result_type;

    constexpr cmta_giveup_cd()
     : value{ static_cast<enum_type>(0) } {}

    constexpr cmta_giveup_cd(enum_type v)
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
            set(static_cast<enum_type>(0));
    }

  protected:
    enum_type value;
};
}
