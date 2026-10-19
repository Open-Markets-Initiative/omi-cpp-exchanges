#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BoxId.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Secure Box Registration Request In Message
struct secure_box_registration_request_in_message {

    struct fields_type {
        nnf_orderentry::box_id box_id;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::secure_box_registration_request_in_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static secure_box_registration_request_in_message* parse(std::byte* buffer) {
        return reinterpret_cast<secure_box_registration_request_in_message*>(buffer);
    }

    // parse method const
    static const secure_box_registration_request_in_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const secure_box_registration_request_in_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(secure_box_registration_request_in_message::fields_type, box_id) == 0, "unexpected offset of secure_box_registration_request_in_message::fields_type::box_id");
static_assert(sizeof(secure_box_registration_request_in_message::fields_type) == 2, "unexpected sizeof secure_box_registration_request_in_message::fields_type");
static_assert(sizeof(secure_box_registration_request_in_message) == sizeof(nnf_orderentry::message_header) + 2, "unexpected sizeof secure_box_registration_request_in_message");

#pragma pack(pop)
}
