#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxequities::matchingengine::ipc::v2026 {

// phase_change
struct phase_change {

    enum class enum_type : char {
        start = 'S',
        pre_open = 'P',
        open = 'O',
        halt = 'H',
        resume = 'R',
        close = 'C',
        end = 'E'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 7> from_string_map = {{
        {"Close", enum_type::close},
        {"End", enum_type::end},
        {"Halt", enum_type::halt},
        {"Open", enum_type::open},
        {"Pre Open", enum_type::pre_open},
        {"Resume", enum_type::resume},
        {"Start", enum_type::start}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::start: return "Start";
            case enum_type::pre_open: return "Pre Open";
            case enum_type::open: return "Open";
            case enum_type::halt: return "Halt";
            case enum_type::resume: return "Resume";
            case enum_type::close: return "Close";
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
