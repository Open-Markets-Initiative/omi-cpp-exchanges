#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/ReasonCode.hpp"
#include "../structs/Giveup.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Giveup Confirmation Message
struct giveup_confirmation_message {

    struct fields_type {
        nnf_orderentry::reason_code reason_code;
        nnf_orderentry::giveup giveup;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::giveup_confirmation_message_4506, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static giveup_confirmation_message* parse(std::byte* buffer) {
        return reinterpret_cast<giveup_confirmation_message*>(buffer);
    }

    // parse method const
    static const giveup_confirmation_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const giveup_confirmation_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(giveup_confirmation_message::fields_type, reason_code) == 0, "unexpected offset of giveup_confirmation_message::fields_type::reason_code");
static_assert(offsetof(giveup_confirmation_message::fields_type, giveup) == 2, "unexpected offset of giveup_confirmation_message::fields_type::giveup");
static_assert(sizeof(giveup_confirmation_message::fields_type) == 81, "unexpected sizeof giveup_confirmation_message::fields_type");
static_assert(sizeof(giveup_confirmation_message) == sizeof(nnf_orderentry::message_header) + 81, "unexpected sizeof giveup_confirmation_message");

#pragma pack(pop)
}
