#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/OrderEntryBody.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Order Confirmation Message
struct order_confirmation_message {

    struct fields_type {
        nnf_orderentry::order_entry_body order_entry_body;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::order_confirmation_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static order_confirmation_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_confirmation_message*>(buffer);
    }

    // parse method const
    static const order_confirmation_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_confirmation_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_confirmation_message::fields_type, order_entry_body) == 0, "unexpected offset of order_confirmation_message::fields_type::order_entry_body");
static_assert(sizeof(order_confirmation_message::fields_type) == 276, "unexpected sizeof order_confirmation_message::fields_type");
static_assert(sizeof(order_confirmation_message) == sizeof(nnf_orderentry::message_header) + 276, "unexpected sizeof order_confirmation_message");

#pragma pack(pop)
}
