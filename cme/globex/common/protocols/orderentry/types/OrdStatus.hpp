#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// ord_status
struct ord_status {

    enum class enum_type : char {
        new_value = '0',
        partially_filled = '1',
        filled = '2',
        cancelled = '4',
        replaced = '5',
        pending_cancel = '6',
        rejected = '8',
        expired = 'C',
        pending_replace = 'E',
        undefined = 'U'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 10> from_string_map = {{
        {"Cancelled", enum_type::cancelled},
        {"Expired", enum_type::expired},
        {"Filled", enum_type::filled},
        {"New", enum_type::new_value},
        {"Partially Filled", enum_type::partially_filled},
        {"Pending Cancel", enum_type::pending_cancel},
        {"Pending Replace", enum_type::pending_replace},
        {"Rejected", enum_type::rejected},
        {"Replaced", enum_type::replaced},
        {"Undefined", enum_type::undefined}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::new_value: return "New";
            case enum_type::partially_filled: return "Partially Filled";
            case enum_type::filled: return "Filled";
            case enum_type::cancelled: return "Cancelled";
            case enum_type::replaced: return "Replaced";
            case enum_type::pending_cancel: return "Pending Cancel";
            case enum_type::rejected: return "Rejected";
            case enum_type::expired: return "Expired";
            case enum_type::pending_replace: return "Pending Replace";
            case enum_type::undefined: return "Undefined";
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

    static constexpr const char* name = "ord_status";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<ord_status::enum_type>;
    using storage_type = result_type;

    constexpr ord_status()
     : value{ enum_type{} } {}

    constexpr ord_status(enum_type v)
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
            set(enum_type::undefined);
    }

  protected:
    enum_type value;
};
}
