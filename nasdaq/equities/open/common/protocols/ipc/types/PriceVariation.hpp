#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

// price_variation
struct price_variation {

    enum class enum_type : char {
        less_than_1_percent = 'L',
        _1_percent = '1',
        _2_percent = '2',
        _3_percent = '3',
        _4_percent = '4',
        _5_percent = '5',
        _6_percent = '6',
        _7_percent = '7',
        _8_percent = '8',
        _9_percent = '9',
        _10_percent = 'A',
        _20_percent = 'B',
        _30_percent = 'C',
        uncalculated = ' '
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 14> from_string_map = {{
        {"1 Percent", enum_type::_1_percent},
        {"10 Percent", enum_type::_10_percent},
        {"2 Percent", enum_type::_2_percent},
        {"20 Percent", enum_type::_20_percent},
        {"3 Percent", enum_type::_3_percent},
        {"30 Percent", enum_type::_30_percent},
        {"4 Percent", enum_type::_4_percent},
        {"5 Percent", enum_type::_5_percent},
        {"6 Percent", enum_type::_6_percent},
        {"7 Percent", enum_type::_7_percent},
        {"8 Percent", enum_type::_8_percent},
        {"9 Percent", enum_type::_9_percent},
        {"Less Than 1 Percent", enum_type::less_than_1_percent},
        {"Uncalculated", enum_type::uncalculated}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::less_than_1_percent: return "Less Than 1 Percent";
            case enum_type::_1_percent: return "1 Percent";
            case enum_type::_2_percent: return "2 Percent";
            case enum_type::_3_percent: return "3 Percent";
            case enum_type::_4_percent: return "4 Percent";
            case enum_type::_5_percent: return "5 Percent";
            case enum_type::_6_percent: return "6 Percent";
            case enum_type::_7_percent: return "7 Percent";
            case enum_type::_8_percent: return "8 Percent";
            case enum_type::_9_percent: return "9 Percent";
            case enum_type::_10_percent: return "10 Percent";
            case enum_type::_20_percent: return "20 Percent";
            case enum_type::_30_percent: return "30 Percent";
            case enum_type::uncalculated: return "Uncalculated";
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

    static constexpr const char* name = "price_variation";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<price_variation::enum_type>;
    using storage_type = result_type;

    constexpr price_variation()
     : value{ enum_type{} } {}

    constexpr price_variation(enum_type v)
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
            set(enum_type::uncalculated);
    }

  protected:
    enum_type value;
};
}
