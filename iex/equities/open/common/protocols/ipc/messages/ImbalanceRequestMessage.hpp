#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

// Imbalance Request Message
struct imbalance_request_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::sequence sequence;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

    // parse method
    static imbalance_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<imbalance_request_message*>(buffer);
    }

    // parse method const
    static const imbalance_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const imbalance_request_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(imbalance_request_message::fields_type, timestamp) == 0, "unexpected offset of imbalance_request_message::fields_type::timestamp");
static_assert(offsetof(imbalance_request_message::fields_type, sequence) == 8, "unexpected offset of imbalance_request_message::fields_type::sequence");
static_assert(sizeof(imbalance_request_message::fields_type) == 16, "unexpected sizeof imbalance_request_message::fields_type");
static_assert(sizeof(imbalance_request_message) == sizeof(ipc_matchingengine::message_header) + 16, "unexpected sizeof imbalance_request_message");

#pragma pack(pop)
}
