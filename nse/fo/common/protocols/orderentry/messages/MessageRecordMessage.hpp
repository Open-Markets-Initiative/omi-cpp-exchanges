#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../structs/InnerHeader.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


#pragma pack(push, 1)

// Message Record Message
struct message_record_message {

    struct fields_type {
        nnf_orderentry::inner_header inner_header;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::message_record_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(nnf_orderentry::message_header) - sizeof(fields_type);

    fields_type fields;
    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.message_length.get().value();
        return sz == sizeof(nnf_orderentry::message_header) + sizeof(fields_type)
            ? tail + tail_capacity
            : tail + (sz - sizeof(nnf_orderentry::message_header) - sizeof(fields_type));
    }

    // sequential access to variable-length regions
    external_var_data message_download_data() const {
        return { tail_begin(), static_cast<std::size_t>(header.message_length.get().value() - 80), tail_end() };
    }


    // parse method
    static message_record_message* parse(std::byte* buffer) {
        return reinterpret_cast<message_record_message*>(buffer);
    }

    // parse method const
    static const message_record_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const message_record_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(message_record_message::fields_type, inner_header) == 0, "unexpected offset of message_record_message::fields_type::inner_header");
static_assert(sizeof(message_record_message::fields_type) == 40, "unexpected sizeof message_record_message::fields_type");

#pragma pack(pop)
}

#include "details/MessageRecordMessageWriter.hpp"
#include "details/MessageRecordMessageReader.hpp"
