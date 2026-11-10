#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// split_msg
struct split_msg {

    enum class enum_type : std::uint8_t {
        no_value = 255,
        split_message_delayed = 0,
        outof_order_message_delayed = 1,
        complete_message_delayed = 2
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Complete Message Delayed", enum_type::complete_message_delayed},
        {"No Value", enum_type::no_value},
        {"Outof Order Message Delayed", enum_type::outof_order_message_delayed},
        {"Split Message Delayed", enum_type::split_message_delayed}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::no_value: return "No Value";
            case enum_type::split_message_delayed: return "Split Message Delayed";
            case enum_type::outof_order_message_delayed: return "Outof Order Message Delayed";
            case enum_type::complete_message_delayed: return "Complete Message Delayed";
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

    static constexpr const char* name = "split_msg";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<split_msg::enum_type>;
    using storage_type = result_type;

    constexpr split_msg()
     : value{ static_cast<enum_type>(255) } {}

    constexpr split_msg(enum_type v)
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
