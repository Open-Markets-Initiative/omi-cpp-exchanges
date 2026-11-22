#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// cxlrejresponseto
struct cxlrejresponseto {

    enum class enum_type : std::int32_t {
        order_cancel_request = 1,
        order_cancelreplace_request = 2,
        quote = 3
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"Order Cancel Request", enum_type::order_cancel_request},
        {"Order Cancel/Replace Request", enum_type::order_cancelreplace_request},
        {"Quote", enum_type::quote}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::order_cancel_request: return "Order Cancel Request";
            case enum_type::order_cancelreplace_request: return "Order Cancel/Replace Request";
            case enum_type::quote: return "Quote";
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

    static constexpr const char* name = "cxlrejresponseto";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<cxlrejresponseto::enum_type>;
    using storage_type = result_type;

    constexpr cxlrejresponseto()
     : value{ enum_type{} } {}

    constexpr cxlrejresponseto(enum_type v)
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
            set(enum_type::quote);
    }

  protected:
    enum_type value;
};
}
