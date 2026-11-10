#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// expiration_cycle
struct expiration_cycle {

    enum class enum_type : std::uint8_t {
        no_value = 255,
        expire_on_trading_session_close = 0,
        expirationatgivendate = 2
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"Expirationatgivendate", enum_type::expirationatgivendate},
        {"Expire On Trading Session Close", enum_type::expire_on_trading_session_close},
        {"No Value", enum_type::no_value}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::no_value: return "No Value";
            case enum_type::expire_on_trading_session_close: return "Expire On Trading Session Close";
            case enum_type::expirationatgivendate: return "Expirationatgivendate";
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

    static constexpr const char* name = "expiration_cycle";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<expiration_cycle::enum_type>;
    using storage_type = result_type;

    constexpr expiration_cycle()
     : value{ static_cast<enum_type>(255) } {}

    constexpr expiration_cycle(enum_type v)
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
            set(static_cast<enum_type>(255));
    }

  protected:
    enum_type value;
};
}
