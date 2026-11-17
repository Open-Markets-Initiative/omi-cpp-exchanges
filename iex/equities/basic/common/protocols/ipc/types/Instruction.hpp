#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

// instruction
struct instruction {

    enum class enum_type : char {
        iex_only = 'i',
        router = 'u',
        iso = 'f',
        trade_at_iso = 'y',
        discretionary_limit = 'j'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Discretionary Limit", enum_type::discretionary_limit},
        {"Iex Only", enum_type::iex_only},
        {"Iso", enum_type::iso},
        {"Router", enum_type::router},
        {"Trade At Iso", enum_type::trade_at_iso}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::iex_only: return "Iex Only";
            case enum_type::router: return "Router";
            case enum_type::iso: return "Iso";
            case enum_type::trade_at_iso: return "Trade At Iso";
            case enum_type::discretionary_limit: return "Discretionary Limit";
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

    static constexpr const char* name = "instruction";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<instruction::enum_type>;
    using storage_type = result_type;

    constexpr instruction()
     : value{ enum_type{} } {}

    constexpr instruction(enum_type v)
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
            set(enum_type::discretionary_limit);
    }

  protected:
    enum_type value;
};
}
