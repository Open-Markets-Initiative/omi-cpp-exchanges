#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

// trading_state
struct trading_state {

    enum class enum_type : char {
        halted_across_all_us_equity_markets_sr_os = 'H',
        paused_across_all_us_equity_markets_sr_os = 'P',
        quotation_only_period_for_cross_sro_halt_or_pause = 'Q',
        trading_on_nasdaq = 'T'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Halted Across All Us Equity Markets Sr Os", enum_type::halted_across_all_us_equity_markets_sr_os},
        {"Paused Across All Us Equity Markets Sr Os", enum_type::paused_across_all_us_equity_markets_sr_os},
        {"Quotation Only Period For Cross Sro Halt Or Pause", enum_type::quotation_only_period_for_cross_sro_halt_or_pause},
        {"Trading On Nasdaq", enum_type::trading_on_nasdaq}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::halted_across_all_us_equity_markets_sr_os: return "Halted Across All Us Equity Markets Sr Os";
            case enum_type::paused_across_all_us_equity_markets_sr_os: return "Paused Across All Us Equity Markets Sr Os";
            case enum_type::quotation_only_period_for_cross_sro_halt_or_pause: return "Quotation Only Period For Cross Sro Halt Or Pause";
            case enum_type::trading_on_nasdaq: return "Trading On Nasdaq";
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

    static constexpr const char* name = "trading_state";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<trading_state::enum_type>;
    using storage_type = result_type;

    constexpr trading_state()
     : value{ enum_type{} } {}

    constexpr trading_state(enum_type v)
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
            set(enum_type::trading_on_nasdaq);
    }

  protected:
    enum_type value;
};
}
