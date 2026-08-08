#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::orderentry::fix::v2_91 {

// sessionrejectreason
struct sessionrejectreason {

    enum class enum_type : std::int32_t {
        invalid_tag_number = 0,
        required_tag_missing = 1,
        tag_not_defined_for_this_message_type = 2,
        undefined_tag = 3,
        tag_specified_without_a_value = 4,
        value_is_incorrect_for_this_tag = 5,
        incorrect_data_format_for_value = 6,
        decryption_problem = 7,
        signature_problem = 8,
        compid_problem = 9,
        sendingtime_accuracy_problem = 10,
        invalid_msgtype = 11
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 12> from_string_map = {{
        {"Compid problem", enum_type::compid_problem},
        {"Decryption problem", enum_type::decryption_problem},
        {"Incorrect data format for value", enum_type::incorrect_data_format_for_value},
        {"Invalid msgtype", enum_type::invalid_msgtype},
        {"Invalid tag number", enum_type::invalid_tag_number},
        {"Required tag missing", enum_type::required_tag_missing},
        {"Sendingtime accuracy problem", enum_type::sendingtime_accuracy_problem},
        {"Signature problem", enum_type::signature_problem},
        {"Tag not defined for this message type", enum_type::tag_not_defined_for_this_message_type},
        {"Tag specified without a value", enum_type::tag_specified_without_a_value},
        {"Undefined tag", enum_type::undefined_tag},
        {"Value is incorrect for this tag", enum_type::value_is_incorrect_for_this_tag}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::invalid_tag_number: return "Invalid tag number";
            case enum_type::required_tag_missing: return "Required tag missing";
            case enum_type::tag_not_defined_for_this_message_type: return "Tag not defined for this message type";
            case enum_type::undefined_tag: return "Undefined tag";
            case enum_type::tag_specified_without_a_value: return "Tag specified without a value";
            case enum_type::value_is_incorrect_for_this_tag: return "Value is incorrect for this tag";
            case enum_type::incorrect_data_format_for_value: return "Incorrect data format for value";
            case enum_type::decryption_problem: return "Decryption problem";
            case enum_type::signature_problem: return "Signature problem";
            case enum_type::compid_problem: return "Compid problem";
            case enum_type::sendingtime_accuracy_problem: return "Sendingtime accuracy problem";
            case enum_type::invalid_msgtype: return "Invalid msgtype";
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
     : value{ enum_type::invalid_tag_number } {}

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
            set(enum_type::invalid_msgtype);
    }

  protected:
    enum_type value;
};
}
