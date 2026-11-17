#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

// time_in_force
struct time_in_force {

    enum class enum_type : char {
        day = '0',
        ioc = '3',
        fok = '4',
        gtx = '5',
        gtt = '6',
        system_hours = 'M',
        at_the_opening = '2',
        at_the_close = '7'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 8> from_string_map = {{
        {"At The Close", enum_type::at_the_close},
        {"At The Opening", enum_type::at_the_opening},
        {"Day", enum_type::day},
        {"Fok", enum_type::fok},
        {"Gtt", enum_type::gtt},
        {"Gtx", enum_type::gtx},
        {"Ioc", enum_type::ioc},
        {"System Hours", enum_type::system_hours}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::day: return "Day";
            case enum_type::ioc: return "Ioc";
            case enum_type::fok: return "Fok";
            case enum_type::gtx: return "Gtx";
            case enum_type::gtt: return "Gtt";
            case enum_type::system_hours: return "System Hours";
            case enum_type::at_the_opening: return "At The Opening";
            case enum_type::at_the_close: return "At The Close";
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

    static constexpr const char* name = "time_in_force";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<time_in_force::enum_type>;
    using storage_type = result_type;

    constexpr time_in_force()
     : value{ enum_type{} } {}

    constexpr time_in_force(enum_type v)
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
            set(enum_type::at_the_close);
    }

  protected:
    enum_type value;
};
}
