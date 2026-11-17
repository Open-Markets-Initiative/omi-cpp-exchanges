#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

// minqtyinstruction
struct minqtyinstruction {

    enum class enum_type : char {
        minimum_execution_size_with_aon_remaining = 'A',
        composite = 'C',
        minimum_execution_size_with_cancel_remaining = 'M'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"COMPOSITE", enum_type::composite},
        {"Minimum execution size with aon remaining", enum_type::minimum_execution_size_with_aon_remaining},
        {"Minimum execution size with cancel remaining", enum_type::minimum_execution_size_with_cancel_remaining}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::minimum_execution_size_with_aon_remaining: return "Minimum execution size with aon remaining";
            case enum_type::composite: return "COMPOSITE";
            case enum_type::minimum_execution_size_with_cancel_remaining: return "Minimum execution size with cancel remaining";
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

    static constexpr const char* name = "minqtyinstruction";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<minqtyinstruction::enum_type>;
    using storage_type = result_type;

    constexpr minqtyinstruction()
     : value{ enum_type{} } {}

    constexpr minqtyinstruction(enum_type v)
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
            set(enum_type::minimum_execution_size_with_cancel_remaining);
    }

  protected:
    enum_type value;
};
}
