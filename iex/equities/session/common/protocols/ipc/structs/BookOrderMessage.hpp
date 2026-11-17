#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/OrderId.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Price.hpp"
#include "../types/Quantity.hpp"
#include "../types/Display.hpp"
#include "../types/BookSequence.hpp"
#include "../types/BookPlace.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct book_order_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::order_id order_id;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::side side;
    ipc_matchingengine::price price;
    ipc_matchingengine::quantity quantity;
    ipc_matchingengine::display display;
    ipc_matchingengine::book_sequence book_sequence;
    ipc_matchingengine::book_place book_place;

    // parse method
    static book_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<book_order_message*>(buffer);
    }

    // parse method const
    static const book_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const book_order_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(book_order_message, timestamp) == 0, "unexpected offset of book_order_message::timestamp");
static_assert(offsetof(book_order_message, order_id) == 8, "unexpected offset of book_order_message::order_id");
static_assert(offsetof(book_order_message, symbol) == 16, "unexpected offset of book_order_message::symbol");
static_assert(offsetof(book_order_message, side) == 24, "unexpected offset of book_order_message::side");
static_assert(offsetof(book_order_message, price) == 25, "unexpected offset of book_order_message::price");
static_assert(offsetof(book_order_message, quantity) == 33, "unexpected offset of book_order_message::quantity");
static_assert(offsetof(book_order_message, display) == 37, "unexpected offset of book_order_message::display");
static_assert(offsetof(book_order_message, book_sequence) == 38, "unexpected offset of book_order_message::book_sequence");
static_assert(offsetof(book_order_message, book_place) == 46, "unexpected offset of book_order_message::book_place");
static_assert(sizeof(book_order_message) == 47, "unexpected sizeof book_order_message");

#pragma pack(pop)
}
