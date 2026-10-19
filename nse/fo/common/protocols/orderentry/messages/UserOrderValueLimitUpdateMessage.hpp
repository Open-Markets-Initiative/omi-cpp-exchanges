#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Reserved1.hpp"
#include "../types/BranchId.hpp"
#include "../types/Reserved26.hpp"
#include "../types/UserId.hpp"
#include "../types/Reserved2.hpp"
#include "../structs/UserLimits.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// User Order Value Limit Update Message
struct user_order_value_limit_update_message {

    struct fields_type {
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::branch_id branch_id;
        nnf_orderentry::reserved_26 reserved_26;
        nnf_orderentry::user_id user_id;
        nnf_orderentry::reserved_2 reserved_2;
        nnf_orderentry::user_limits user_limits[2];
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::user_order_value_limit_update_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static user_order_value_limit_update_message* parse(std::byte* buffer) {
        return reinterpret_cast<user_order_value_limit_update_message*>(buffer);
    }

    // parse method const
    static const user_order_value_limit_update_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const user_order_value_limit_update_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(user_order_value_limit_update_message::fields_type, broker_id) == 0, "unexpected offset of user_order_value_limit_update_message::fields_type::broker_id");
static_assert(offsetof(user_order_value_limit_update_message::fields_type, reserved_1) == 5, "unexpected offset of user_order_value_limit_update_message::fields_type::reserved_1");
static_assert(offsetof(user_order_value_limit_update_message::fields_type, branch_id) == 6, "unexpected offset of user_order_value_limit_update_message::fields_type::branch_id");
static_assert(offsetof(user_order_value_limit_update_message::fields_type, reserved_26) == 8, "unexpected offset of user_order_value_limit_update_message::fields_type::reserved_26");
static_assert(offsetof(user_order_value_limit_update_message::fields_type, user_id) == 34, "unexpected offset of user_order_value_limit_update_message::fields_type::user_id");
static_assert(offsetof(user_order_value_limit_update_message::fields_type, reserved_2) == 38, "unexpected offset of user_order_value_limit_update_message::fields_type::reserved_2");
static_assert(offsetof(user_order_value_limit_update_message::fields_type, user_limits) == 40, "unexpected offset of user_order_value_limit_update_message::fields_type::user_limits");
static_assert(sizeof(user_order_value_limit_update_message::fields_type) == 168, "unexpected sizeof user_order_value_limit_update_message::fields_type");
static_assert(sizeof(user_order_value_limit_update_message) == sizeof(nnf_orderentry::message_header) + 168, "unexpected sizeof user_order_value_limit_update_message");

#pragma pack(pop)
}
