#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/SpreadOrderBody.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Spread Order Entry Message
struct spread_order_entry_message {

    struct fields_type {
        nnf_orderentry::spread_order_body spread_order_body;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::spread_order_entry_message_2100, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static spread_order_entry_message* parse(std::byte* buffer) {
        return reinterpret_cast<spread_order_entry_message*>(buffer);
    }

    // parse method const
    static const spread_order_entry_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_order_entry_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(spread_order_entry_message::fields_type, spread_order_body) == 0, "unexpected offset of spread_order_entry_message::fields_type::spread_order_body");
static_assert(sizeof(spread_order_entry_message::fields_type) == 440, "unexpected sizeof spread_order_entry_message::fields_type");
static_assert(sizeof(spread_order_entry_message) == sizeof(nnf_orderentry::message_header) + 440, "unexpected sizeof spread_order_entry_message");

#pragma pack(pop)
}
