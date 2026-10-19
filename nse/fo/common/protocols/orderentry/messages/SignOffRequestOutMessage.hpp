#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/UserId.hpp"
#include "../types/Reserved145.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Sign Off Request Out Message
struct sign_off_request_out_message {

    struct fields_type {
        nnf_orderentry::user_id user_id;
        nnf_orderentry::reserved_145 reserved_145;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::sign_off_request_out_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static sign_off_request_out_message* parse(std::byte* buffer) {
        return reinterpret_cast<sign_off_request_out_message*>(buffer);
    }

    // parse method const
    static const sign_off_request_out_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const sign_off_request_out_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(sign_off_request_out_message::fields_type, user_id) == 0, "unexpected offset of sign_off_request_out_message::fields_type::user_id");
static_assert(offsetof(sign_off_request_out_message::fields_type, reserved_145) == 4, "unexpected offset of sign_off_request_out_message::fields_type::reserved_145");
static_assert(sizeof(sign_off_request_out_message::fields_type) == 149, "unexpected sizeof sign_off_request_out_message::fields_type");
static_assert(sizeof(sign_off_request_out_message) == sizeof(nnf_orderentry::message_header) + 149, "unexpected sizeof sign_off_request_out_message");

#pragma pack(pop)
}
