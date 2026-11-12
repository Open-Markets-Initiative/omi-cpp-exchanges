#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// gapfillflag
struct gapfillflag {

    enum class enum_type : char {
        sequence_reset_ignore_msgseqnum_34 = 'N',
        gap_fill_message_msgseqnum_34_field_valid = 'Y'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 2> from_string_map = {{
        {"Gap Fill message, MsgSeqNum (34) field valid", enum_type::gap_fill_message_msgseqnum_34_field_valid},
        {"Sequence Reset, ignore MsgSeqNum (34)", enum_type::sequence_reset_ignore_msgseqnum_34}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::sequence_reset_ignore_msgseqnum_34: return "Sequence Reset, ignore MsgSeqNum (34)";
            case enum_type::gap_fill_message_msgseqnum_34_field_valid: return "Gap Fill message, MsgSeqNum (34) field valid";
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

    static constexpr const char* name = "gapfillflag";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<gapfillflag::enum_type>;
    using storage_type = result_type;

    constexpr gapfillflag()
     : value{ enum_type{} } {}

    constexpr gapfillflag(enum_type v)
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
            set(enum_type::gap_fill_message_msgseqnum_34_field_valid);
    }

  protected:
    enum_type value;
};
}
