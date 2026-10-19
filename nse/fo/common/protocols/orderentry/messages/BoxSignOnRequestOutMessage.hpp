#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BoxId.hpp"
#include "../types/Reserved10.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Box Sign On Request Out Message
struct box_sign_on_request_out_message {

    struct fields_type {
        nnf_orderentry::box_id box_id;
        nnf_orderentry::reserved_10 reserved_10;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::box_sign_on_request_out_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static box_sign_on_request_out_message* parse(std::byte* buffer) {
        return reinterpret_cast<box_sign_on_request_out_message*>(buffer);
    }

    // parse method const
    static const box_sign_on_request_out_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const box_sign_on_request_out_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(box_sign_on_request_out_message::fields_type, box_id) == 0, "unexpected offset of box_sign_on_request_out_message::fields_type::box_id");
static_assert(offsetof(box_sign_on_request_out_message::fields_type, reserved_10) == 2, "unexpected offset of box_sign_on_request_out_message::fields_type::reserved_10");
static_assert(sizeof(box_sign_on_request_out_message::fields_type) == 12, "unexpected sizeof box_sign_on_request_out_message::fields_type");
static_assert(sizeof(box_sign_on_request_out_message) == sizeof(nnf_orderentry::message_header) + 12, "unexpected sizeof box_sign_on_request_out_message");

#pragma pack(pop)
}
