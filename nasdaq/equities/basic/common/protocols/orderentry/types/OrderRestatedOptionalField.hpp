#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// order_restated_optional_field
struct order_restated_optional_field {

    enum class enum_type : std::uint8_t {
        display_quantity = 22,
        display_price = 23,
        secondaryordrefnum = 1,
        userrefidx = 28
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Display Price", enum_type::display_price},
        {"Display Quantity", enum_type::display_quantity},
        {"SecondaryOrdRefNum", enum_type::secondaryordrefnum},
        {"UserRefIdx", enum_type::userrefidx}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::display_quantity: return "Display Quantity";
            case enum_type::display_price: return "Display Price";
            case enum_type::secondaryordrefnum: return "SecondaryOrdRefNum";
            case enum_type::userrefidx: return "UserRefIdx";
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

    static constexpr const char* name = "order_restated_optional_field";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<order_restated_optional_field::enum_type>;
    using storage_type = result_type;

    constexpr order_restated_optional_field()
     : value{ enum_type{} } {}

    constexpr order_restated_optional_field(enum_type v)
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
            set(enum_type::userrefidx);
    }

  protected:
    enum_type value;
};
}
