#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// auction
struct auction {

    enum class enum_type : char {
        none = 'N',
        opening = 'O',
        intraday = 'I',
        closing = 'C',
        extended = 'A',
        extended_life = 'E',
        opening_imbalance_only = 'M',
        closing_imbalance_only = 'L'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 8> from_string_map = {{
        {"Closing", enum_type::closing},
        {"Closing Imbalance Only", enum_type::closing_imbalance_only},
        {"Extended", enum_type::extended},
        {"Extended Life", enum_type::extended_life},
        {"Intraday", enum_type::intraday},
        {"None", enum_type::none},
        {"Opening", enum_type::opening},
        {"Opening Imbalance Only", enum_type::opening_imbalance_only}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::none: return "None";
            case enum_type::opening: return "Opening";
            case enum_type::intraday: return "Intraday";
            case enum_type::closing: return "Closing";
            case enum_type::extended: return "Extended";
            case enum_type::extended_life: return "Extended Life";
            case enum_type::opening_imbalance_only: return "Opening Imbalance Only";
            case enum_type::closing_imbalance_only: return "Closing Imbalance Only";
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

    static constexpr const char* name = "auction";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<auction::enum_type>;
    using storage_type = result_type;

    constexpr auction()
     : value{ enum_type{} } {}

    constexpr auction(enum_type v)
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
            set(enum_type::closing_imbalance_only);
    }

  protected:
    enum_type value;
};
}
