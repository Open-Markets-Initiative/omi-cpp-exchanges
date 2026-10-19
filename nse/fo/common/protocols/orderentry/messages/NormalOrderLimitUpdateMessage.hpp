#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Reserved1.hpp"
#include "../types/UserId.hpp"
#include "../types/OrderQtyLimit.hpp"
#include "../types/OrderValLimit.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Normal Order Limit Update Message
struct normal_order_limit_update_message {

    struct fields_type {
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::user_id user_id;
        nnf_orderentry::order_qty_limit order_qty_limit;
        nnf_orderentry::order_val_limit order_val_limit;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::normal_order_limit_update_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static normal_order_limit_update_message* parse(std::byte* buffer) {
        return reinterpret_cast<normal_order_limit_update_message*>(buffer);
    }

    // parse method const
    static const normal_order_limit_update_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const normal_order_limit_update_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(normal_order_limit_update_message::fields_type, broker_id) == 0, "unexpected offset of normal_order_limit_update_message::fields_type::broker_id");
static_assert(offsetof(normal_order_limit_update_message::fields_type, reserved_1) == 5, "unexpected offset of normal_order_limit_update_message::fields_type::reserved_1");
static_assert(offsetof(normal_order_limit_update_message::fields_type, user_id) == 6, "unexpected offset of normal_order_limit_update_message::fields_type::user_id");
static_assert(offsetof(normal_order_limit_update_message::fields_type, order_qty_limit) == 10, "unexpected offset of normal_order_limit_update_message::fields_type::order_qty_limit");
static_assert(offsetof(normal_order_limit_update_message::fields_type, order_val_limit) == 18, "unexpected offset of normal_order_limit_update_message::fields_type::order_val_limit");
static_assert(sizeof(normal_order_limit_update_message::fields_type) == 26, "unexpected sizeof normal_order_limit_update_message::fields_type");
static_assert(sizeof(normal_order_limit_update_message) == sizeof(nnf_orderentry::message_header) + 26, "unexpected sizeof normal_order_limit_update_message");

#pragma pack(pop)
}
