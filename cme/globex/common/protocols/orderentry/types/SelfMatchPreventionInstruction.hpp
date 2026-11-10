#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// self_match_prevention_instruction
struct self_match_prevention_instruction {

    enum class enum_type : char {
        cancel_newest = 'N',
        cancel_oldest = 'O'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Cancel Newest", enum_type::cancel_newest},
        {"Cancel Oldest", enum_type::cancel_oldest}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::cancel_newest: return "Cancel Newest";
            case enum_type::cancel_oldest: return "Cancel Oldest";
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

    static constexpr const char* name = "self_match_prevention_instruction";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<self_match_prevention_instruction::enum_type>;
    using storage_type = result_type;

    constexpr self_match_prevention_instruction()
     : value{ static_cast<enum_type>(0) } {}

    constexpr self_match_prevention_instruction(enum_type v)
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
