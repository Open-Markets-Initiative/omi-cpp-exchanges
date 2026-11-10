#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// cust_order_handling_inst
struct cust_order_handling_inst {

    enum class enum_type : char {
        fc_mprovidedscreen = 'C',
        otherprovidedscreen = 'D',
        fcmap_ior_fix = 'G',
        algo_engine = 'H',
        desk_electronic = 'W',
        client_electronic = 'Y'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 6> from_string_map = {{
        {"Algo Engine", enum_type::algo_engine},
        {"Client Electronic", enum_type::client_electronic},
        {"Desk Electronic", enum_type::desk_electronic},
        {"Fc Mprovidedscreen", enum_type::fc_mprovidedscreen},
        {"Fcmap Ior Fix", enum_type::fcmap_ior_fix},
        {"Otherprovidedscreen", enum_type::otherprovidedscreen}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::fc_mprovidedscreen: return "Fc Mprovidedscreen";
            case enum_type::otherprovidedscreen: return "Otherprovidedscreen";
            case enum_type::fcmap_ior_fix: return "Fcmap Ior Fix";
            case enum_type::algo_engine: return "Algo Engine";
            case enum_type::desk_electronic: return "Desk Electronic";
            case enum_type::client_electronic: return "Client Electronic";
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

    static constexpr const char* name = "cust_order_handling_inst";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<cust_order_handling_inst::enum_type>;
    using storage_type = result_type;

    constexpr cust_order_handling_inst()
     : value{ static_cast<enum_type>(0) } {}

    constexpr cust_order_handling_inst(enum_type v)
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
            set(static_cast<enum_type>(0));
    }

  protected:
    enum_type value;
};
}
