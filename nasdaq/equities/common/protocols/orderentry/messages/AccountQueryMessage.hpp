#pragma once

#include <cstddef>
#include "../structs/ClientPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/UnsequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Account Query Message
struct account_query_message {

    struct fields_type {
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::client_packet_header header = {std::uint16_t(sizeof(ouch_orders::client_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::unsequenced_message_type) - 2), client_packet_type::enum_type::unsequenced_data_packet};
    ouch_orders::unsequenced_message_type unsequenced_message_type = ouch_orders::unsequenced_message_type::enum_type::account_query_message;

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(ouch_orders::client_packet_header) - sizeof(fields_type) - sizeof(ouch_orders::unsequenced_message_type);

    fields_type fields;
    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.packet_length.get().value();
        return (sz + 2) == sizeof(ouch_orders::client_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::unsequenced_message_type)
            ? tail + tail_capacity
            : tail + ((sz + 2) - sizeof(ouch_orders::client_packet_header) - sizeof(fields_type) - sizeof(ouch_orders::unsequenced_message_type));
    }

    // sequential access to variable-length regions
    external_var_data account_query_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static account_query_message* parse(std::byte* buffer) {
        return reinterpret_cast<account_query_message*>(buffer);
    }

    // parse method const
    static const account_query_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const account_query_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(account_query_message::fields_type, appendage_length) == 0, "unexpected offset of account_query_message::fields_type::appendage_length");
static_assert(sizeof(account_query_message::fields_type) == 2, "unexpected sizeof account_query_message::fields_type");

#pragma pack(pop)
}

#include "details/AccountQueryMessageWriter.hpp"
#include "details/AccountQueryMessageReader.hpp"
