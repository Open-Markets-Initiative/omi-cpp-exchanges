#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// participant_type
struct participant_type {

    enum class enum_type : char {
        initiator = 'I',
        competitor = 'C',
        solicitor = 'S',
        no_value = ' '
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Competitor", enum_type::competitor},
        {"Initiator", enum_type::initiator},
        {"No Value", enum_type::no_value},
        {"Solicitor", enum_type::solicitor}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::initiator: return "Initiator";
            case enum_type::competitor: return "Competitor";
            case enum_type::solicitor: return "Solicitor";
            case enum_type::no_value: return "No Value";
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

    static constexpr const char* name = "participant_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = true;
    static constexpr enum_type null_value = static_cast<enum_type>(' ');

    using result_type = std::optional<participant_type::enum_type>;
    using storage_type = result_type;

    constexpr participant_type()
     : value{ static_cast<enum_type>(' ') } {}

    constexpr participant_type(enum_type v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        const enum_type held = value;
        return held == null_value ? std::nullopt : result_type{held};
    }

    constexpr void set(enum_type v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(null_value);
    }

    constexpr void set_null() {
        set(null_value);
    }

  protected:
    enum_type value;
};
}
