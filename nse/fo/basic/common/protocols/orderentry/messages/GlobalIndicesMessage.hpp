#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/IndexDetails.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Global Indices Message
struct global_indices_message {

    struct fields_type {
        nnf_orderentry::index_details index_details;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::global_indices_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static global_indices_message* parse(std::byte* buffer) {
        return reinterpret_cast<global_indices_message*>(buffer);
    }

    // parse method const
    static const global_indices_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const global_indices_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(global_indices_message::fields_type, index_details) == 0, "unexpected offset of global_indices_message::fields_type::index_details");
static_assert(sizeof(global_indices_message::fields_type) == 98, "unexpected sizeof global_indices_message::fields_type");
static_assert(sizeof(global_indices_message) == sizeof(nnf_orderentry::message_header) + 98, "unexpected sizeof global_indices_message");

#pragma pack(pop)
}
