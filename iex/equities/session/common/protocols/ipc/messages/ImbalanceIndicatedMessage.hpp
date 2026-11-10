#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Cross.hpp"
#include "../types/Price.hpp"
#include "../types/PairedQuantity.hpp"
#include "../types/ImbalanceQuantity.hpp"
#include "../types/ImbalanceDirection.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

// Imbalance Indicated Message
struct imbalance_indicated_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::symbol symbol;
        ipc_matchingengine::cross cross;
        ipc_matchingengine::price price;
        ipc_matchingengine::paired_quantity paired_quantity;
        ipc_matchingengine::imbalance_quantity imbalance_quantity;
        ipc_matchingengine::imbalance_direction imbalance_direction;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

    // parse method
    static imbalance_indicated_message* parse(std::byte* buffer) {
        return reinterpret_cast<imbalance_indicated_message*>(buffer);
    }

    // parse method const
    static const imbalance_indicated_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const imbalance_indicated_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(imbalance_indicated_message::fields_type, timestamp) == 0, "unexpected offset of imbalance_indicated_message::fields_type::timestamp");
static_assert(offsetof(imbalance_indicated_message::fields_type, symbol) == 8, "unexpected offset of imbalance_indicated_message::fields_type::symbol");
static_assert(offsetof(imbalance_indicated_message::fields_type, cross) == 16, "unexpected offset of imbalance_indicated_message::fields_type::cross");
static_assert(offsetof(imbalance_indicated_message::fields_type, price) == 17, "unexpected offset of imbalance_indicated_message::fields_type::price");
static_assert(offsetof(imbalance_indicated_message::fields_type, paired_quantity) == 25, "unexpected offset of imbalance_indicated_message::fields_type::paired_quantity");
static_assert(offsetof(imbalance_indicated_message::fields_type, imbalance_quantity) == 33, "unexpected offset of imbalance_indicated_message::fields_type::imbalance_quantity");
static_assert(offsetof(imbalance_indicated_message::fields_type, imbalance_direction) == 41, "unexpected offset of imbalance_indicated_message::fields_type::imbalance_direction");
static_assert(sizeof(imbalance_indicated_message::fields_type) == 42, "unexpected sizeof imbalance_indicated_message::fields_type");
static_assert(sizeof(imbalance_indicated_message) == sizeof(ipc_matchingengine::message_header) + 42, "unexpected sizeof imbalance_indicated_message");

#pragma pack(pop)
}
