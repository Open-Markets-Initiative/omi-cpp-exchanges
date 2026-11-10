#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Cross.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/ExecutionId.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

// Cross Completed Message
struct cross_completed_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::symbol symbol;
        ipc_matchingengine::cross cross;
        ipc_matchingengine::price price;
        ipc_matchingengine::quantity quantity;
        ipc_matchingengine::execution_id execution_id;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

    // parse method
    static cross_completed_message* parse(std::byte* buffer) {
        return reinterpret_cast<cross_completed_message*>(buffer);
    }

    // parse method const
    static const cross_completed_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const cross_completed_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(cross_completed_message::fields_type, timestamp) == 0, "unexpected offset of cross_completed_message::fields_type::timestamp");
static_assert(offsetof(cross_completed_message::fields_type, symbol) == 8, "unexpected offset of cross_completed_message::fields_type::symbol");
static_assert(offsetof(cross_completed_message::fields_type, cross) == 16, "unexpected offset of cross_completed_message::fields_type::cross");
static_assert(offsetof(cross_completed_message::fields_type, price) == 17, "unexpected offset of cross_completed_message::fields_type::price");
static_assert(offsetof(cross_completed_message::fields_type, quantity) == 25, "unexpected offset of cross_completed_message::fields_type::quantity");
static_assert(offsetof(cross_completed_message::fields_type, execution_id) == 29, "unexpected offset of cross_completed_message::fields_type::execution_id");
static_assert(sizeof(cross_completed_message::fields_type) == 37, "unexpected sizeof cross_completed_message::fields_type");
static_assert(sizeof(cross_completed_message) == sizeof(ipc_matchingengine::message_header) + 37, "unexpected sizeof cross_completed_message");

#pragma pack(pop)
}
