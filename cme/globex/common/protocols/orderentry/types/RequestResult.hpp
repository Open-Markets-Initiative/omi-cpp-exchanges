#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// request_result
struct request_result {

    enum class enum_type : std::uint8_t {
        valid_request = 0,
        no_data_found_that_matched_selection_criteria = 2,
        not_authorizedto_retrieve_data = 3,
        data_temporarily_unavailable = 4
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 4> from_string_map = {{
        {"Data Temporarily Unavailable", enum_type::data_temporarily_unavailable},
        {"No Data Found That Matched Selection Criteria", enum_type::no_data_found_that_matched_selection_criteria},
        {"Not Authorizedto Retrieve Data", enum_type::not_authorizedto_retrieve_data},
        {"Valid Request", enum_type::valid_request}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::valid_request: return "Valid Request";
            case enum_type::no_data_found_that_matched_selection_criteria: return "No Data Found That Matched Selection Criteria";
            case enum_type::not_authorizedto_retrieve_data: return "Not Authorizedto Retrieve Data";
            case enum_type::data_temporarily_unavailable: return "Data Temporarily Unavailable";
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

    static constexpr const char* name = "request_result";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<request_result::enum_type>;
    using storage_type = result_type;

    constexpr request_result()
     : value{ enum_type{} } {}

    constexpr request_result(enum_type v)
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
            set(enum_type::data_temporarily_unavailable);
    }

  protected:
    enum_type value;
};
}
