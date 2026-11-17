#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// self_match_strategy
struct self_match_strategy {

    enum class enum_type : char {
        disabled = 'N',
        decrement = 'D',
        cancel_oldest = 'O',
        cancel_newest = 'W',
        use_remover = 'R'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Cancel Newest", enum_type::cancel_newest},
        {"Cancel Oldest", enum_type::cancel_oldest},
        {"Decrement", enum_type::decrement},
        {"Disabled", enum_type::disabled},
        {"Use Remover", enum_type::use_remover}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::disabled: return "Disabled";
            case enum_type::decrement: return "Decrement";
            case enum_type::cancel_oldest: return "Cancel Oldest";
            case enum_type::cancel_newest: return "Cancel Newest";
            case enum_type::use_remover: return "Use Remover";
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

    static constexpr const char* name = "self_match_strategy";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<self_match_strategy::enum_type>;
    using storage_type = result_type;

    constexpr self_match_strategy()
     : value{ enum_type{} } {}

    constexpr self_match_strategy(enum_type v)
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
            set(enum_type::use_remover);
    }

  protected:
    enum_type value;
};
}
