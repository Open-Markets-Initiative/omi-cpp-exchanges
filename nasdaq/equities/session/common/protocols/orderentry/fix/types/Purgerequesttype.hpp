#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

// purgerequesttype
struct purgerequesttype {

    enum class enum_type : std::int32_t {
        cancel_orders = 1,
        disable_mpid = 2,
        enable_mpid = 3
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"Cancel orders", enum_type::cancel_orders},
        {"Disable MPID", enum_type::disable_mpid},
        {"Enable MPID", enum_type::enable_mpid}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::cancel_orders: return "Cancel orders";
            case enum_type::disable_mpid: return "Disable MPID";
            case enum_type::enable_mpid: return "Enable MPID";
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

    static constexpr const char* name = "purgerequesttype";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<purgerequesttype::enum_type>;
    using storage_type = result_type;

    constexpr purgerequesttype()
     : value{ enum_type{} } {}

    constexpr purgerequesttype(enum_type v)
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
            set(enum_type::enable_mpid);
    }

  protected:
    enum_type value;
};
}
