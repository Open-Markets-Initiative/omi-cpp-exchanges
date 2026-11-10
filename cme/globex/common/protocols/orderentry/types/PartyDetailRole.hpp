#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// party_detail_role
struct party_detail_role {

    enum class enum_type : std::uint16_t {
        executing_firm = 1,
        customer_account = 24,
        take_up_firm = 96,
        operator_value = 118,
        take_up_account = 1000
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Customer Account", enum_type::customer_account},
        {"Executing Firm", enum_type::executing_firm},
        {"Operator", enum_type::operator_value},
        {"Take Up Account", enum_type::take_up_account},
        {"Take Up Firm", enum_type::take_up_firm}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::executing_firm: return "Executing Firm";
            case enum_type::customer_account: return "Customer Account";
            case enum_type::take_up_firm: return "Take Up Firm";
            case enum_type::operator_value: return "Operator";
            case enum_type::take_up_account: return "Take Up Account";
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

    static constexpr const char* name = "party_detail_role";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<party_detail_role::enum_type>;
    using storage_type = result_type;

    constexpr party_detail_role()
     : value{ enum_type{} } {}

    constexpr party_detail_role(enum_type v)
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
            set(enum_type::take_up_account);
    }

  protected:
    enum_type value;
};
}
