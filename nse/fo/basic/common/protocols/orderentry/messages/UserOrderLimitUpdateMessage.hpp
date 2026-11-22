#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BrokerId.hpp"
#include "../types/BranchId.hpp"
#include "../types/UserName.hpp"
#include "../types/UserId.hpp"
#include "../types/UserType.hpp"
#include "../structs/InstrumentUser.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// User Order Limit Update Message
struct user_order_limit_update_message {

    struct fields_type {
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::branch_id branch_id;
        nnf_orderentry::user_name user_name;
        nnf_orderentry::user_id user_id;
        nnf_orderentry::user_type user_type;
        nnf_orderentry::instrument_user instrument_user[2];
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::user_order_limit_update_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static user_order_limit_update_message* parse(std::byte* buffer) {
        return reinterpret_cast<user_order_limit_update_message*>(buffer);
    }

    // parse method const
    static const user_order_limit_update_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const user_order_limit_update_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(user_order_limit_update_message::fields_type, broker_id) == 0, "unexpected offset of user_order_limit_update_message::fields_type::broker_id");
static_assert(offsetof(user_order_limit_update_message::fields_type, branch_id) == 5, "unexpected offset of user_order_limit_update_message::fields_type::branch_id");
static_assert(offsetof(user_order_limit_update_message::fields_type, user_name) == 7, "unexpected offset of user_order_limit_update_message::fields_type::user_name");
static_assert(offsetof(user_order_limit_update_message::fields_type, user_id) == 32, "unexpected offset of user_order_limit_update_message::fields_type::user_id");
static_assert(offsetof(user_order_limit_update_message::fields_type, user_type) == 36, "unexpected offset of user_order_limit_update_message::fields_type::user_type");
static_assert(offsetof(user_order_limit_update_message::fields_type, instrument_user) == 38, "unexpected offset of user_order_limit_update_message::fields_type::instrument_user");
static_assert(sizeof(user_order_limit_update_message::fields_type) == 166, "unexpected sizeof user_order_limit_update_message::fields_type");
static_assert(sizeof(user_order_limit_update_message) == sizeof(nnf_orderentry::message_header) + 166, "unexpected sizeof user_order_limit_update_message");

#pragma pack(pop)
}
