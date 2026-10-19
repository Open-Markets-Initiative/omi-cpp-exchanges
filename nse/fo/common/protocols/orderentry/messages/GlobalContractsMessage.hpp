#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/ContractsDetails.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Global Contracts Message
struct global_contracts_message {

    struct fields_type {
        nnf_orderentry::contracts_details contracts_details;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::global_contracts_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static global_contracts_message* parse(std::byte* buffer) {
        return reinterpret_cast<global_contracts_message*>(buffer);
    }

    // parse method const
    static const global_contracts_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const global_contracts_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(global_contracts_message::fields_type, contracts_details) == 0, "unexpected offset of global_contracts_message::fields_type::contracts_details");
static_assert(sizeof(global_contracts_message::fields_type) == 122, "unexpected sizeof global_contracts_message::fields_type");
static_assert(sizeof(global_contracts_message) == sizeof(nnf_orderentry::message_header) + 122, "unexpected sizeof global_contracts_message");

#pragma pack(pop)
}
