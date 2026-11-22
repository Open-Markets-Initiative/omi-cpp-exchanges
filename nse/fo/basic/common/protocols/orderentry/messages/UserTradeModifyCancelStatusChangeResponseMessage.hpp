#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/UserId.hpp"
#include "../types/TrdModCxlBit.hpp"
#include "../types/Reserved1.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// User Trade Modify Cancel Status Change Response Message
struct user_trade_modify_cancel_status_change_response_message {

    struct fields_type {
        nnf_orderentry::user_id user_id;
        nnf_orderentry::trd_mod_cxl_bit trd_mod_cxl_bit;
        nnf_orderentry::reserved_1 reserved_1;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::user_trade_modify_cancel_status_change_response_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static user_trade_modify_cancel_status_change_response_message* parse(std::byte* buffer) {
        return reinterpret_cast<user_trade_modify_cancel_status_change_response_message*>(buffer);
    }

    // parse method const
    static const user_trade_modify_cancel_status_change_response_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const user_trade_modify_cancel_status_change_response_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(user_trade_modify_cancel_status_change_response_message::fields_type, user_id) == 0, "unexpected offset of user_trade_modify_cancel_status_change_response_message::fields_type::user_id");
static_assert(offsetof(user_trade_modify_cancel_status_change_response_message::fields_type, trd_mod_cxl_bit) == 4, "unexpected offset of user_trade_modify_cancel_status_change_response_message::fields_type::trd_mod_cxl_bit");
static_assert(offsetof(user_trade_modify_cancel_status_change_response_message::fields_type, reserved_1) == 5, "unexpected offset of user_trade_modify_cancel_status_change_response_message::fields_type::reserved_1");
static_assert(sizeof(user_trade_modify_cancel_status_change_response_message::fields_type) == 6, "unexpected sizeof user_trade_modify_cancel_status_change_response_message::fields_type");
static_assert(sizeof(user_trade_modify_cancel_status_change_response_message) == sizeof(nnf_orderentry::message_header) + 6, "unexpected sizeof user_trade_modify_cancel_status_change_response_message");

#pragma pack(pop)
}
