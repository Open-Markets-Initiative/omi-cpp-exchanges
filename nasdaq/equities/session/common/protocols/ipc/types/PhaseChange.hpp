#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// phase_change
struct phase_change {

    enum class enum_type : char {
        pre_open = 'P',
        open = 'O',
        halt = 'H',
        resume = 'R',
        quote = 'Q',
        postpone = 'W',
        opening_lock_in = 'I',
        opening_lock_out = 'K',
        opening_late = 'J',
        closing_lock_out = 'X',
        closing_freeze = 'F',
        pre_close = 'L',
        close = 'C',
        extended_trading_close_end = 'A',
        end = 'E'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 15> from_string_map = {{
        {"Close", enum_type::close},
        {"Closing Freeze", enum_type::closing_freeze},
        {"Closing Lock Out", enum_type::closing_lock_out},
        {"End", enum_type::end},
        {"Extended Trading Close End", enum_type::extended_trading_close_end},
        {"Halt", enum_type::halt},
        {"Open", enum_type::open},
        {"Opening Late", enum_type::opening_late},
        {"Opening Lock In", enum_type::opening_lock_in},
        {"Opening Lock Out", enum_type::opening_lock_out},
        {"Postpone", enum_type::postpone},
        {"Pre Close", enum_type::pre_close},
        {"Pre Open", enum_type::pre_open},
        {"Quote", enum_type::quote},
        {"Resume", enum_type::resume}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::pre_open: return "Pre Open";
            case enum_type::open: return "Open";
            case enum_type::halt: return "Halt";
            case enum_type::resume: return "Resume";
            case enum_type::quote: return "Quote";
            case enum_type::postpone: return "Postpone";
            case enum_type::opening_lock_in: return "Opening Lock In";
            case enum_type::opening_lock_out: return "Opening Lock Out";
            case enum_type::opening_late: return "Opening Late";
            case enum_type::closing_lock_out: return "Closing Lock Out";
            case enum_type::closing_freeze: return "Closing Freeze";
            case enum_type::pre_close: return "Pre Close";
            case enum_type::close: return "Close";
            case enum_type::extended_trading_close_end: return "Extended Trading Close End";
            case enum_type::end: return "End";
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

    static constexpr const char* name = "phase_change";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<phase_change::enum_type>;
    using storage_type = result_type;

    constexpr phase_change()
     : value{ enum_type{} } {}

    constexpr phase_change(enum_type v)
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
            set(enum_type::end);
    }

  protected:
    enum_type value;
};
}
