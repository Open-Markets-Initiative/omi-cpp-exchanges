#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/SequenceNumber.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Download Request Message
struct download_request_message {

    struct fields_type {
        nnf_orderentry::sequence_number sequence_number;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::download_request_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static download_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<download_request_message*>(buffer);
    }

    // parse method const
    static const download_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const download_request_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(download_request_message::fields_type, sequence_number) == 0, "unexpected offset of download_request_message::fields_type::sequence_number");
static_assert(sizeof(download_request_message::fields_type) == 8, "unexpected sizeof download_request_message::fields_type");
static_assert(sizeof(download_request_message) == sizeof(nnf_orderentry::message_header) + 8, "unexpected sizeof download_request_message");

#pragma pack(pop)
}
