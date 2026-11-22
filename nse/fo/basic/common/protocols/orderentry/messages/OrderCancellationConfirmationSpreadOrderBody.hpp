#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/SpreadOrderBody.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Order Cancellation Confirmation Message
struct order_cancellation_confirmation_spread_order_body {

    struct fields_type {
        nnf_orderentry::spread_order_body spread_order_body;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::order_cancellation_confirmation_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static order_cancellation_confirmation_spread_order_body* parse(std::byte* buffer) {
        return reinterpret_cast<order_cancellation_confirmation_spread_order_body*>(buffer);
    }

    // parse method const
    static const order_cancellation_confirmation_spread_order_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_cancellation_confirmation_spread_order_body*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_cancellation_confirmation_spread_order_body::fields_type, spread_order_body) == 0, "unexpected offset of order_cancellation_confirmation_spread_order_body::fields_type::spread_order_body");
static_assert(sizeof(order_cancellation_confirmation_spread_order_body::fields_type) == 440, "unexpected sizeof order_cancellation_confirmation_spread_order_body::fields_type");
static_assert(sizeof(order_cancellation_confirmation_spread_order_body) == sizeof(nnf_orderentry::message_header) + 440, "unexpected sizeof order_cancellation_confirmation_spread_order_body");

#pragma pack(pop)
}
