#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/UserId.hpp"
#include "../types/Reserved36.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// User Address Unlock Confirm Message
struct user_address_unlock_confirm_message {

    struct fields_type {
        nnf_orderentry::user_id user_id;
        nnf_orderentry::reserved_36 reserved_36;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::user_address_unlock_confirm_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static user_address_unlock_confirm_message* parse(std::byte* buffer) {
        return reinterpret_cast<user_address_unlock_confirm_message*>(buffer);
    }

    // parse method const
    static const user_address_unlock_confirm_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const user_address_unlock_confirm_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(user_address_unlock_confirm_message::fields_type, user_id) == 0, "unexpected offset of user_address_unlock_confirm_message::fields_type::user_id");
static_assert(offsetof(user_address_unlock_confirm_message::fields_type, reserved_36) == 4, "unexpected offset of user_address_unlock_confirm_message::fields_type::reserved_36");
static_assert(sizeof(user_address_unlock_confirm_message::fields_type) == 40, "unexpected sizeof user_address_unlock_confirm_message::fields_type");
static_assert(sizeof(user_address_unlock_confirm_message) == sizeof(nnf_orderentry::message_header) + 40, "unexpected sizeof user_address_unlock_confirm_message");

#pragma pack(pop)
}
