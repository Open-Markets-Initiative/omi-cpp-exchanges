#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Sequence.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::iex::iexequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct book_request_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::sequence sequence;

    // parse method
    static book_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<book_request_message*>(buffer);
    }

    // parse method const
    static const book_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const book_request_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(book_request_message, timestamp) == 0, "unexpected offset of book_request_message::timestamp");
static_assert(offsetof(book_request_message, sequence) == 8, "unexpected offset of book_request_message::sequence");
static_assert(sizeof(book_request_message) == 16, "unexpected sizeof book_request_message");

#pragma pack(pop)
}
