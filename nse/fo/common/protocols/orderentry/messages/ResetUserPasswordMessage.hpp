#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/UserId.hpp"
#include "../types/Reserved14.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Reset User Password Message
struct reset_user_password_message {

    struct fields_type {
        nnf_orderentry::user_id user_id;
        nnf_orderentry::reserved_14 reserved_14;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::reset_user_password_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static reset_user_password_message* parse(std::byte* buffer) {
        return reinterpret_cast<reset_user_password_message*>(buffer);
    }

    // parse method const
    static const reset_user_password_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const reset_user_password_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(reset_user_password_message::fields_type, user_id) == 0, "unexpected offset of reset_user_password_message::fields_type::user_id");
static_assert(offsetof(reset_user_password_message::fields_type, reserved_14) == 4, "unexpected offset of reset_user_password_message::fields_type::reserved_14");
static_assert(sizeof(reset_user_password_message::fields_type) == 18, "unexpected sizeof reset_user_password_message::fields_type");
static_assert(sizeof(reset_user_password_message) == sizeof(nnf_orderentry::message_header) + 18, "unexpected sizeof reset_user_password_message");

#pragma pack(pop)
}
