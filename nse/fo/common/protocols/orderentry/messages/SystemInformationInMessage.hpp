#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/LastUpdatePortfolioTIme.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// System Information In Message
struct system_information_in_message {

    struct fields_type {
        nnf_orderentry::last_update_portfolio_t_ime last_update_portfolio_t_ime;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::system_information_in_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static system_information_in_message* parse(std::byte* buffer) {
        return reinterpret_cast<system_information_in_message*>(buffer);
    }

    // parse method const
    static const system_information_in_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const system_information_in_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(system_information_in_message::fields_type, last_update_portfolio_t_ime) == 0, "unexpected offset of system_information_in_message::fields_type::last_update_portfolio_t_ime");
static_assert(sizeof(system_information_in_message::fields_type) == 4, "unexpected sizeof system_information_in_message::fields_type");
static_assert(sizeof(system_information_in_message) == sizeof(nnf_orderentry::message_header) + 4, "unexpected sizeof system_information_in_message");

#pragma pack(pop)
}
