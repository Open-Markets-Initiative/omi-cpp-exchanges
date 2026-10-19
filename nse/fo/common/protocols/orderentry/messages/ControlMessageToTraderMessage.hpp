#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/TraderId.hpp"
#include "../types/ActionCode.hpp"
#include "../types/Reserved1.hpp"
#include "../types/BroadCastMessageLength.hpp"
#include "../types/BroadCastMessage.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Control Message To Trader Message
struct control_message_to_trader_message {

    struct fields_type {
        nnf_orderentry::trader_id trader_id;
        nnf_orderentry::action_code action_code;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::broad_cast_message_length broad_cast_message_length;
        nnf_orderentry::broad_cast_message broad_cast_message;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::control_message_to_trader_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static control_message_to_trader_message* parse(std::byte* buffer) {
        return reinterpret_cast<control_message_to_trader_message*>(buffer);
    }

    // parse method const
    static const control_message_to_trader_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const control_message_to_trader_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(control_message_to_trader_message::fields_type, trader_id) == 0, "unexpected offset of control_message_to_trader_message::fields_type::trader_id");
static_assert(offsetof(control_message_to_trader_message::fields_type, action_code) == 4, "unexpected offset of control_message_to_trader_message::fields_type::action_code");
static_assert(offsetof(control_message_to_trader_message::fields_type, reserved_1) == 7, "unexpected offset of control_message_to_trader_message::fields_type::reserved_1");
static_assert(offsetof(control_message_to_trader_message::fields_type, broad_cast_message_length) == 8, "unexpected offset of control_message_to_trader_message::fields_type::broad_cast_message_length");
static_assert(offsetof(control_message_to_trader_message::fields_type, broad_cast_message) == 10, "unexpected offset of control_message_to_trader_message::fields_type::broad_cast_message");
static_assert(sizeof(control_message_to_trader_message::fields_type) == 249, "unexpected sizeof control_message_to_trader_message::fields_type");
static_assert(sizeof(control_message_to_trader_message) == sizeof(nnf_orderentry::message_header) + 249, "unexpected sizeof control_message_to_trader_message");

#pragma pack(pop)
}
