#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

// sessionrejectreason
struct sessionrejectreason {

    enum class enum_type : std::int32_t {
        invalid_tag_number = 0,
        required_tag_missing = 1,
        sendingtime52_accuracy_problem = 10,
        invalid_msgtype35 = 11,
        tag_appears_more_than_once = 13,
        tag_not_defined_for_this_message_type = 2,
        tag_specified_without_a_value = 4,
        value_is_incorrect_out_of_range_for_this_tag = 5,
        incorrect_data_format_for_value = 6,
        compid_problem = 9,
        other = 99
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 11> from_string_map = {{
        {"CompID problem", enum_type::compid_problem},
        {"Incorrect data format for value", enum_type::incorrect_data_format_for_value},
        {"Invalid MsgType(35)", enum_type::invalid_msgtype35},
        {"Invalid tag number", enum_type::invalid_tag_number},
        {"Other", enum_type::other},
        {"Required tag missing", enum_type::required_tag_missing},
        {"SendingTime(52) accuracy problem", enum_type::sendingtime52_accuracy_problem},
        {"Tag appears more than once", enum_type::tag_appears_more_than_once},
        {"Tag not defined for this message type", enum_type::tag_not_defined_for_this_message_type},
        {"Tag specified without a value", enum_type::tag_specified_without_a_value},
        {"Value is incorrect (out of range) for this tag", enum_type::value_is_incorrect_out_of_range_for_this_tag}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::invalid_tag_number: return "Invalid tag number";
            case enum_type::required_tag_missing: return "Required tag missing";
            case enum_type::sendingtime52_accuracy_problem: return "SendingTime(52) accuracy problem";
            case enum_type::invalid_msgtype35: return "Invalid MsgType(35)";
            case enum_type::tag_appears_more_than_once: return "Tag appears more than once";
            case enum_type::tag_not_defined_for_this_message_type: return "Tag not defined for this message type";
            case enum_type::tag_specified_without_a_value: return "Tag specified without a value";
            case enum_type::value_is_incorrect_out_of_range_for_this_tag: return "Value is incorrect (out of range) for this tag";
            case enum_type::incorrect_data_format_for_value: return "Incorrect data format for value";
            case enum_type::compid_problem: return "CompID problem";
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

    static constexpr const char* name = "sessionrejectreason";
    static constexpr std::size_t size = 4;
    static constexpr bool is_optional = false;

    using result_type = required<sessionrejectreason::enum_type>;
    using storage_type = result_type;

    constexpr sessionrejectreason()
     : value{ enum_type{} } {}

    constexpr sessionrejectreason(enum_type v)
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
