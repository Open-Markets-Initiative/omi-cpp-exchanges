#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


#pragma pack(push, 1)

// Order Cancellation Confirmation Message
struct order_cancellation_confirmation_message {

    struct fields_type {
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::order_cancellation_confirmation_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header))};

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(nnf_orderentry::message_header);

    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.message_length.get().value();
        return sz == sizeof(nnf_orderentry::message_header)
            ? tail + tail_capacity
            : tail + (sz - sizeof(nnf_orderentry::message_header));
    }

    // sequential access to variable-length regions
    sbe_var_data cancellation_confirmation_body() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static order_cancellation_confirmation_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_cancellation_confirmation_message*>(buffer);
    }

    // parse method const
    static const order_cancellation_confirmation_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_cancellation_confirmation_message*>(buffer);
    }

};

// layout verification
static_assert(sizeof(order_cancellation_confirmation_message::fields_type) == 1, "unexpected sizeof order_cancellation_confirmation_message::fields_type");

#pragma pack(pop)
}

#include "details/OrderCancellationConfirmationMessageWriter.hpp"
#include "details/OrderCancellationConfirmationMessageReader.hpp"
