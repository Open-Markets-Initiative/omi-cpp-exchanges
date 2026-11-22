#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BoxId.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Reserved5.hpp"
#include "../types/SessionKey.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Box Sign On Request In Message
struct box_sign_on_request_in_message {

    struct fields_type {
        nnf_orderentry::box_id box_id;
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::reserved_5 reserved_5;
        nnf_orderentry::session_key session_key;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::box_sign_on_request_in_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static box_sign_on_request_in_message* parse(std::byte* buffer) {
        return reinterpret_cast<box_sign_on_request_in_message*>(buffer);
    }

    // parse method const
    static const box_sign_on_request_in_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const box_sign_on_request_in_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(box_sign_on_request_in_message::fields_type, box_id) == 0, "unexpected offset of box_sign_on_request_in_message::fields_type::box_id");
static_assert(offsetof(box_sign_on_request_in_message::fields_type, broker_id) == 2, "unexpected offset of box_sign_on_request_in_message::fields_type::broker_id");
static_assert(offsetof(box_sign_on_request_in_message::fields_type, reserved_5) == 7, "unexpected offset of box_sign_on_request_in_message::fields_type::reserved_5");
static_assert(offsetof(box_sign_on_request_in_message::fields_type, session_key) == 12, "unexpected offset of box_sign_on_request_in_message::fields_type::session_key");
static_assert(sizeof(box_sign_on_request_in_message::fields_type) == 20, "unexpected sizeof box_sign_on_request_in_message::fields_type");
static_assert(sizeof(box_sign_on_request_in_message) == sizeof(nnf_orderentry::message_header) + 20, "unexpected sizeof box_sign_on_request_in_message");

#pragma pack(pop)
}
