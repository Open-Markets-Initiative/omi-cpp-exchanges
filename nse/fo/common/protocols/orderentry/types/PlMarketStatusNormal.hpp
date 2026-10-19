#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// pl_market_status_normal
struct pl_market_status_normal {

    enum class enum_type : std::int16_t {
        pre_open = 0,
        open = 1,
        closed = 2,
        pre_open_ended = 3,
        postclose = 4
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Closed", enum_type::closed},
        {"Open", enum_type::open},
        {"Postclose", enum_type::postclose},
        {"Pre Open", enum_type::pre_open},
        {"Pre Open Ended", enum_type::pre_open_ended}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::pre_open: return "Pre Open";
            case enum_type::open: return "Open";
            case enum_type::closed: return "Closed";
            case enum_type::pre_open_ended: return "Pre Open Ended";
            case enum_type::postclose: return "Postclose";
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

    static constexpr const char* name = "pl_market_status_normal";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<pl_market_status_normal::enum_type>;
    using storage_type = result_type;

    constexpr pl_market_status_normal()
     : value{ enum_type{} } {}

    constexpr pl_market_status_normal(enum_type v)
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
            set(enum_type::postclose);
    }

  protected:
    enum_type value;
};
}
