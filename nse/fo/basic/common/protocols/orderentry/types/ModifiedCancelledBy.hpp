#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// modified_cancelled_by
struct modified_cancelled_by {

    enum class enum_type : char {
        trader = 'T',
        branch_manager = 'B',
        corporate_manager = 'M',
        exchange = 'C'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Branch Manager", enum_type::branch_manager},
        {"Corporate Manager", enum_type::corporate_manager},
        {"Exchange", enum_type::exchange},
        {"Trader", enum_type::trader}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::trader: return "Trader";
            case enum_type::branch_manager: return "Branch Manager";
            case enum_type::corporate_manager: return "Corporate Manager";
            case enum_type::exchange: return "Exchange";
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

    static constexpr const char* name = "modified_cancelled_by";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<modified_cancelled_by::enum_type>;
    using storage_type = result_type;

    constexpr modified_cancelled_by()
     : value{ enum_type{} } {}

    constexpr modified_cancelled_by(enum_type v)
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
            set(enum_type::exchange);
    }

  protected:
    enum_type value;
};
}
