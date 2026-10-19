#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/Timestamp.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/Side.hpp"
#include "../types/Quantity.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Order Modified Message
struct order_modified_message {

    struct fields_type {
        ouch_orders::timestamp timestamp;
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::side side;
        ouch_orders::quantity quantity;
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::server_packet_header header = {std::uint16_t(sizeof(ouch_orders::server_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    ouch_orders::sequenced_message_type sequenced_message_type = ouch_orders::sequenced_message_type::enum_type::order_modified_message;

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(ouch_orders::server_packet_header) - sizeof(fields_type) - sizeof(ouch_orders::sequenced_message_type);

    fields_type fields;
    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.packet_length.get().value();
        return (sz + 2) == sizeof(ouch_orders::server_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::sequenced_message_type)
            ? tail + tail_capacity
            : tail + ((sz + 2) - sizeof(ouch_orders::server_packet_header) - sizeof(fields_type) - sizeof(ouch_orders::sequenced_message_type));
    }

    // sequential access to variable-length regions
    external_var_data order_modified_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static order_modified_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_modified_message*>(buffer);
    }

    // parse method const
    static const order_modified_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_modified_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_modified_message::fields_type, timestamp) == 0, "unexpected offset of order_modified_message::fields_type::timestamp");
static_assert(offsetof(order_modified_message::fields_type, user_ref_num) == 8, "unexpected offset of order_modified_message::fields_type::user_ref_num");
static_assert(offsetof(order_modified_message::fields_type, side) == 12, "unexpected offset of order_modified_message::fields_type::side");
static_assert(offsetof(order_modified_message::fields_type, quantity) == 13, "unexpected offset of order_modified_message::fields_type::quantity");
static_assert(offsetof(order_modified_message::fields_type, appendage_length) == 17, "unexpected offset of order_modified_message::fields_type::appendage_length");
static_assert(sizeof(order_modified_message::fields_type) == 19, "unexpected sizeof order_modified_message::fields_type");

#pragma pack(pop)
}

#include "details/OrderModifiedMessageWriter.hpp"
#include "details/OrderModifiedMessageReader.hpp"
