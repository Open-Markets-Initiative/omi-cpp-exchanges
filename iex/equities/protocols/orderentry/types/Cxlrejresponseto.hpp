#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// cxlrejresponseto
struct cxlrejresponseto {

    enum class enum_type : char {
        order_cancel_request = '1',
        order_cancel_replace_request = '2'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Order cancel replace request", enum_type::order_cancel_replace_request},
        {"Order cancel request", enum_type::order_cancel_request}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::order_cancel_request: return "Order cancel request";
            case enum_type::order_cancel_replace_request: return "Order cancel replace request";
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
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<cxlrejresponseto::enum_type>;
    using storage_type = result_type;

    constexpr cxlrejresponseto()
     : value{ enum_type::order_cancel_request } {}

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
            set(enum_type::order_cancel_replace_request);
    }

  protected:
    enum_type value;
};
}
