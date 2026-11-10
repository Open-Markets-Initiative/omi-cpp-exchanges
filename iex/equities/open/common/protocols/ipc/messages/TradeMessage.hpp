#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/AggressorOrderId.hpp"
#include "../types/RestingOrderId.hpp"
#include "../types/ExecutionId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/Auction.hpp"

namespace iex::iexequities::matchingengine::ipc::v1_0 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v1_0;

#pragma pack(push, 1)

// Trade Message
struct trade_message {

    struct fields_type {
        ipc_matchingengine::timestamp timestamp;
        ipc_matchingengine::aggressor_order_id aggressor_order_id;
        ipc_matchingengine::resting_order_id resting_order_id;
        ipc_matchingengine::execution_id execution_id;
        ipc_matchingengine::symbol symbol;
        ipc_matchingengine::price price;
        ipc_matchingengine::quantity quantity;
        ipc_matchingengine::auction auction;
    };

    ipc_matchingengine::message_header header = {std::uint16_t(sizeof(ipc_matchingengine::message_header) + sizeof(fields_type) - 2), {}};

    fields_type fields;

    // parse method
    static trade_message* parse(std::byte* buffer) {
        return reinterpret_cast<trade_message*>(buffer);
    }

    // parse method const
    static const trade_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const trade_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(trade_message::fields_type, timestamp) == 0, "unexpected offset of trade_message::fields_type::timestamp");
static_assert(offsetof(trade_message::fields_type, aggressor_order_id) == 8, "unexpected offset of trade_message::fields_type::aggressor_order_id");
static_assert(offsetof(trade_message::fields_type, resting_order_id) == 16, "unexpected offset of trade_message::fields_type::resting_order_id");
static_assert(offsetof(trade_message::fields_type, execution_id) == 24, "unexpected offset of trade_message::fields_type::execution_id");
static_assert(offsetof(trade_message::fields_type, symbol) == 32, "unexpected offset of trade_message::fields_type::symbol");
static_assert(offsetof(trade_message::fields_type, price) == 40, "unexpected offset of trade_message::fields_type::price");
static_assert(offsetof(trade_message::fields_type, quantity) == 48, "unexpected offset of trade_message::fields_type::quantity");
static_assert(offsetof(trade_message::fields_type, auction) == 52, "unexpected offset of trade_message::fields_type::auction");
static_assert(sizeof(trade_message::fields_type) == 53, "unexpected sizeof trade_message::fields_type");
static_assert(sizeof(trade_message) == sizeof(ipc_matchingengine::message_header) + 53, "unexpected sizeof trade_message");

#pragma pack(pop)
}
