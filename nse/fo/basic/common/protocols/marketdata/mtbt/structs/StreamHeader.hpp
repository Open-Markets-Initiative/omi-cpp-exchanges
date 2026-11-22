#pragma once

#include <cstddef>
#include "../types/MessageLength.hpp"
#include "../types/StreamId.hpp"
#include "../types/SequenceNumber.hpp"

namespace nse::nsefo::mtbt::tbt::v7_1 {

namespace tbt_mtbt = ::nse::nsefo::mtbt::tbt::v7_1;

#pragma pack(push, 1)

struct stream_header {

    tbt_mtbt::message_length message_length;
    tbt_mtbt::stream_id stream_id;
    tbt_mtbt::sequence_number sequence_number;

    // parse method
    static stream_header* parse(std::byte* buffer) {
        return reinterpret_cast<stream_header*>(buffer);
    }

    // parse method const
    static const stream_header* parse(const std::byte* buffer) {
        return reinterpret_cast<const stream_header*>(buffer);
    }
};

// layout verification
static_assert(offsetof(stream_header, message_length) == 0, "unexpected offset of stream_header::message_length");
static_assert(offsetof(stream_header, stream_id) == 2, "unexpected offset of stream_header::stream_id");
static_assert(offsetof(stream_header, sequence_number) == 4, "unexpected offset of stream_header::sequence_number");
static_assert(sizeof(stream_header) == 8, "unexpected sizeof stream_header");

#pragma pack(pop)
}
