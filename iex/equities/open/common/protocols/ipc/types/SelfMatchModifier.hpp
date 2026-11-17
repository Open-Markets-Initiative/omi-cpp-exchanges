#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

// self_match_modifier
struct self_match_modifier {

    enum class enum_type : char {
        cancel_newest = 'N',
        cancel_oldest = 'O',
        cancel_both = 'B',
        cancel_smaller = 'S',
        free = '-'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Cancel Both", enum_type::cancel_both},
        {"Cancel Newest", enum_type::cancel_newest},
        {"Cancel Oldest", enum_type::cancel_oldest},
        {"Cancel Smaller", enum_type::cancel_smaller},
        {"Free", enum_type::free}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::cancel_newest: return "Cancel Newest";
            case enum_type::cancel_oldest: return "Cancel Oldest";
            case enum_type::cancel_both: return "Cancel Both";
            case enum_type::cancel_smaller: return "Cancel Smaller";
            case enum_type::free: return "Free";
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

    static constexpr const char* name = "self_match_modifier";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<self_match_modifier::enum_type>;
    using storage_type = result_type;

    constexpr self_match_modifier()
     : value{ enum_type{} } {}

    constexpr self_match_modifier(enum_type v)
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
            set(enum_type::free);
    }

  protected:
    enum_type value;
};
}
