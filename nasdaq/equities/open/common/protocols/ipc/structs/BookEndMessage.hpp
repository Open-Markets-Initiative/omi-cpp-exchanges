#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/BookSequence.hpp"
#include "../types/BookOrders.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct book_end_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::book_sequence book_sequence;
    ipc_matchingengine::book_orders book_orders;

    // parse method
    static book_end_message* parse(std::byte* buffer) {
        return reinterpret_cast<book_end_message*>(buffer);
    }

    // parse method const
    static const book_end_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const book_end_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(book_end_message, timestamp) == 0, "unexpected offset of book_end_message::timestamp");
static_assert(offsetof(book_end_message, book_sequence) == 8, "unexpected offset of book_end_message::book_sequence");
static_assert(offsetof(book_end_message, book_orders) == 16, "unexpected offset of book_end_message::book_orders");
static_assert(sizeof(book_end_message) == 20, "unexpected sizeof book_end_message");

#pragma pack(pop)
}
