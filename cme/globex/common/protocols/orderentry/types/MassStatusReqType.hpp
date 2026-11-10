#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// mass_status_req_type
struct mass_status_req_type {

    enum class enum_type : std::uint8_t {
        instrument = 1,
        instrument_group = 3,
        all_orders = 7,
        market_segment = 100
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"All Orders", enum_type::all_orders},
        {"Instrument", enum_type::instrument},
        {"Instrument Group", enum_type::instrument_group},
        {"Market Segment", enum_type::market_segment}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::instrument: return "Instrument";
            case enum_type::instrument_group: return "Instrument Group";
            case enum_type::all_orders: return "All Orders";
            case enum_type::market_segment: return "Market Segment";
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

    static constexpr const char* name = "mass_status_req_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<mass_status_req_type::enum_type>;
    using storage_type = result_type;

    constexpr mass_status_req_type()
     : value{ enum_type{} } {}

    constexpr mass_status_req_type(enum_type v)
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
            set(enum_type::market_segment);
    }

  protected:
    enum_type value;
};
}
