#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Reserved25.hpp"
#include "../types/BranchId.hpp"
#include "../structs/BranchLimits.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Branch Order Value Limit Update Message
struct branch_order_value_limit_update_message {

    struct fields_type {
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::reserved_25 reserved_25;
        nnf_orderentry::branch_id branch_id;
        nnf_orderentry::branch_limits branch_limits[2];
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::branch_order_value_limit_update_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static branch_order_value_limit_update_message* parse(std::byte* buffer) {
        return reinterpret_cast<branch_order_value_limit_update_message*>(buffer);
    }

    // parse method const
    static const branch_order_value_limit_update_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const branch_order_value_limit_update_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(branch_order_value_limit_update_message::fields_type, broker_id) == 0, "unexpected offset of branch_order_value_limit_update_message::fields_type::broker_id");
static_assert(offsetof(branch_order_value_limit_update_message::fields_type, reserved_25) == 5, "unexpected offset of branch_order_value_limit_update_message::fields_type::reserved_25");
static_assert(offsetof(branch_order_value_limit_update_message::fields_type, branch_id) == 30, "unexpected offset of branch_order_value_limit_update_message::fields_type::branch_id");
static_assert(offsetof(branch_order_value_limit_update_message::fields_type, branch_limits) == 32, "unexpected offset of branch_order_value_limit_update_message::fields_type::branch_limits");
static_assert(sizeof(branch_order_value_limit_update_message::fields_type) == 96, "unexpected sizeof branch_order_value_limit_update_message::fields_type");
static_assert(sizeof(branch_order_value_limit_update_message) == sizeof(nnf_orderentry::message_header) + 96, "unexpected sizeof branch_order_value_limit_update_message");

#pragma pack(pop)
}
