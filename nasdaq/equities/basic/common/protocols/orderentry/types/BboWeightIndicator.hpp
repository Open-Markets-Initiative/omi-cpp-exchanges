#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// bbo_weight_indicator
struct bbo_weight_indicator {

    enum class enum_type : char {
        weight_0_to_02_percent = '0',
        weight_02_to_1_percent = '1',
        weight_1_to_2_percent = '2',
        greater_than_2 = '3',
        unspecified = ' ',
        sets_the_qbbo_while_joining_the_nbbo = 'S',
        improves_the_nbbo_upon_entry = 'N'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 7> from_string_map = {{
        {"Greater Than 2%", enum_type::greater_than_2},
        {"Improves The Nbbo Upon Entry", enum_type::improves_the_nbbo_upon_entry},
        {"Sets The Qbbo While Joining The Nbbo", enum_type::sets_the_qbbo_while_joining_the_nbbo},
        {"Unspecified", enum_type::unspecified},
        {"Weight 0 To 0.2 Percent", enum_type::weight_0_to_02_percent},
        {"Weight 0.2 To 1 Percent", enum_type::weight_02_to_1_percent},
        {"Weight 1 To 2 Percent", enum_type::weight_1_to_2_percent}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::weight_0_to_02_percent: return "Weight 0 To 0.2 Percent";
            case enum_type::weight_02_to_1_percent: return "Weight 0.2 To 1 Percent";
            case enum_type::weight_1_to_2_percent: return "Weight 1 To 2 Percent";
            case enum_type::greater_than_2: return "Greater Than 2%";
            case enum_type::unspecified: return "Unspecified";
            case enum_type::sets_the_qbbo_while_joining_the_nbbo: return "Sets The Qbbo While Joining The Nbbo";
            case enum_type::improves_the_nbbo_upon_entry: return "Improves The Nbbo Upon Entry";
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

    static constexpr const char* name = "bbo_weight_indicator";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<bbo_weight_indicator::enum_type>;
    using storage_type = result_type;

    constexpr bbo_weight_indicator()
     : value{ enum_type{} } {}

    constexpr bbo_weight_indicator(enum_type v)
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
            set(enum_type::improves_the_nbbo_upon_entry);
    }

  protected:
    enum_type value;
};
}
