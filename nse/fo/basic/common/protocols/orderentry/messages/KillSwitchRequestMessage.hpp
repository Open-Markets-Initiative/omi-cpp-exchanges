#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/OrderEntryBody.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Kill Switch Request Message
struct kill_switch_request_message {

    struct fields_type {
        nnf_orderentry::order_entry_body order_entry_body;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::kill_switch_request_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static kill_switch_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<kill_switch_request_message*>(buffer);
    }

    // parse method const
    static const kill_switch_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const kill_switch_request_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(kill_switch_request_message::fields_type, order_entry_body) == 0, "unexpected offset of kill_switch_request_message::fields_type::order_entry_body");
static_assert(sizeof(kill_switch_request_message::fields_type) == 276, "unexpected sizeof kill_switch_request_message::fields_type");
static_assert(sizeof(kill_switch_request_message) == sizeof(nnf_orderentry::message_header) + 276, "unexpected sizeof kill_switch_request_message");

#pragma pack(pop)
}
