#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// execrestatementreason
struct execrestatementreason {

    enum class enum_type : std::int32_t {
        cancel_due_to_order_expiration = 103,
        cancel_on_connection_loss = 12,
        verbal_change = 2,
        other = 99
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Cancel due to order expiration", enum_type::cancel_due_to_order_expiration},
        {"Cancel on connection loss", enum_type::cancel_on_connection_loss},
        {"Other", enum_type::other},
        {"Verbal change", enum_type::verbal_change}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::cancel_due_to_order_expiration: return "Cancel due to order expiration";
            case enum_type::cancel_on_connection_loss: return "Cancel on connection loss";
            case enum_type::verbal_change: return "Verbal change";
            case enum_type::other: return "Other";
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

    static constexpr const char* name = "execrestatementreason";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<execrestatementreason::enum_type>;
    using storage_type = result_type;

    constexpr execrestatementreason()
     : value{ enum_type{} } {}

    constexpr execrestatementreason(enum_type v)
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
            set(enum_type::other);
    }

  protected:
    enum_type value;
};
}
