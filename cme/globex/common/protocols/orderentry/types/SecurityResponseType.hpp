#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// security_response_type
struct security_response_type {

    enum class enum_type : std::uint8_t {
        accept_security_proposalasis = 1,
        accept_securityproposalwithrevisionsasindicatedinthemessage = 2,
        reject_security_proposal = 5
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 3> from_string_map = {{
        {"Accept Security Proposalasis", enum_type::accept_security_proposalasis},
        {"Accept Securityproposalwithrevisionsasindicatedinthemessage", enum_type::accept_securityproposalwithrevisionsasindicatedinthemessage},
        {"Reject Security Proposal", enum_type::reject_security_proposal}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::accept_security_proposalasis: return "Accept Security Proposalasis";
            case enum_type::accept_securityproposalwithrevisionsasindicatedinthemessage: return "Accept Securityproposalwithrevisionsasindicatedinthemessage";
            case enum_type::reject_security_proposal: return "Reject Security Proposal";
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

    static constexpr const char* name = "security_response_type";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<security_response_type::enum_type>;
    using storage_type = result_type;

    constexpr security_response_type()
     : value{ enum_type{} } {}

    constexpr security_response_type(enum_type v)
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
            set(enum_type::reject_security_proposal);
    }

  protected:
    enum_type value;
};
}
