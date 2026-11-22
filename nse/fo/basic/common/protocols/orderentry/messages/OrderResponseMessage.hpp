#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/OrderEntryBody.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Order Response Message
struct order_response_message {

    struct fields_type {
        nnf_orderentry::order_entry_body order_entry_body;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::order_response_message_2012, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static order_response_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_response_message*>(buffer);
    }

    // parse method const
    static const order_response_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_response_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_response_message::fields_type, order_entry_body) == 0, "unexpected offset of order_response_message::fields_type::order_entry_body");
static_assert(sizeof(order_response_message::fields_type) == 276, "unexpected sizeof order_response_message::fields_type");
static_assert(sizeof(order_response_message) == sizeof(nnf_orderentry::message_header) + 276, "unexpected sizeof order_response_message");

#pragma pack(pop)
}
