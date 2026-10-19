#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Reserved2.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Update Local Database Trailer Message
struct update_local_database_trailer_message {

    struct fields_type {
        nnf_orderentry::reserved_2 reserved_2;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::update_local_database_trailer_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static update_local_database_trailer_message* parse(std::byte* buffer) {
        return reinterpret_cast<update_local_database_trailer_message*>(buffer);
    }

    // parse method const
    static const update_local_database_trailer_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const update_local_database_trailer_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(update_local_database_trailer_message::fields_type, reserved_2) == 0, "unexpected offset of update_local_database_trailer_message::fields_type::reserved_2");
static_assert(sizeof(update_local_database_trailer_message::fields_type) == 2, "unexpected sizeof update_local_database_trailer_message::fields_type");
static_assert(sizeof(update_local_database_trailer_message) == sizeof(nnf_orderentry::message_header) + 2, "unexpected sizeof update_local_database_trailer_message");

#pragma pack(pop)
}
