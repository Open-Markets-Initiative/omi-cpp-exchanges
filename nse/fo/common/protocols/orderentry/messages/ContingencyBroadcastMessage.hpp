#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/StreamNumber.hpp"
#include "../types/Status.hpp"
#include "../types/Reserved200.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Contingency Broadcast Message
struct contingency_broadcast_message {

    struct fields_type {
        nnf_orderentry::stream_number stream_number;
        nnf_orderentry::status status;
        nnf_orderentry::reserved_200 reserved_200;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::contingency_broadcast_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static contingency_broadcast_message* parse(std::byte* buffer) {
        return reinterpret_cast<contingency_broadcast_message*>(buffer);
    }

    // parse method const
    static const contingency_broadcast_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const contingency_broadcast_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(contingency_broadcast_message::fields_type, stream_number) == 0, "unexpected offset of contingency_broadcast_message::fields_type::stream_number");
static_assert(offsetof(contingency_broadcast_message::fields_type, status) == 2, "unexpected offset of contingency_broadcast_message::fields_type::status");
static_assert(offsetof(contingency_broadcast_message::fields_type, reserved_200) == 4, "unexpected offset of contingency_broadcast_message::fields_type::reserved_200");
static_assert(sizeof(contingency_broadcast_message::fields_type) == 204, "unexpected sizeof contingency_broadcast_message::fields_type");
static_assert(sizeof(contingency_broadcast_message) == sizeof(nnf_orderentry::message_header) + 204, "unexpected sizeof contingency_broadcast_message");

#pragma pack(pop)
}
