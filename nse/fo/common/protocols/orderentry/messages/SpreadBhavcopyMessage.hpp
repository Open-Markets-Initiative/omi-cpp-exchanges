#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/MessageType.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


#pragma pack(push, 1)

// Spread Bhavcopy Message
struct spread_bhavcopy_message {

    struct fields_type {
        nnf_orderentry::message_type message_type;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::spread_bhavcopy_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

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
    sbe_var_data spread_report_body() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static spread_bhavcopy_message* parse(std::byte* buffer) {
        return reinterpret_cast<spread_bhavcopy_message*>(buffer);
    }

    // parse method const
    static const spread_bhavcopy_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_bhavcopy_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(spread_bhavcopy_message::fields_type, message_type) == 0, "unexpected offset of spread_bhavcopy_message::fields_type::message_type");
static_assert(sizeof(spread_bhavcopy_message::fields_type) == 1, "unexpected sizeof spread_bhavcopy_message::fields_type");

#pragma pack(pop)
}

#include "details/SpreadBhavcopyMessageWriter.hpp"
#include "details/SpreadBhavcopyMessageReader.hpp"
