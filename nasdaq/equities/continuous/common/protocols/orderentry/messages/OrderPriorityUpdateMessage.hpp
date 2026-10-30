#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/Timestamp.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/Price.hpp"
#include "../types/Display.hpp"
#include "../types/OrderReferenceNumber.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Order Priority Update Message
struct order_priority_update_message {

    struct fields_type {
        ouch_orders::timestamp timestamp;
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::price price;
        ouch_orders::display display;
        ouch_orders::order_reference_number order_reference_number;
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::server_packet_header header = {std::uint16_t(sizeof(ouch_orders::server_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    ouch_orders::sequenced_message_type sequenced_message_type = ouch_orders::sequenced_message_type::enum_type::order_priority_update_message;

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
    external_var_data order_priority_update_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static order_priority_update_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_priority_update_message*>(buffer);
    }

    // parse method const
    static const order_priority_update_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_priority_update_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_priority_update_message::fields_type, timestamp) == 0, "unexpected offset of order_priority_update_message::fields_type::timestamp");
static_assert(offsetof(order_priority_update_message::fields_type, user_ref_num) == 8, "unexpected offset of order_priority_update_message::fields_type::user_ref_num");
static_assert(offsetof(order_priority_update_message::fields_type, price) == 12, "unexpected offset of order_priority_update_message::fields_type::price");
static_assert(offsetof(order_priority_update_message::fields_type, display) == 20, "unexpected offset of order_priority_update_message::fields_type::display");
static_assert(offsetof(order_priority_update_message::fields_type, order_reference_number) == 21, "unexpected offset of order_priority_update_message::fields_type::order_reference_number");
static_assert(offsetof(order_priority_update_message::fields_type, appendage_length) == 29, "unexpected offset of order_priority_update_message::fields_type::appendage_length");
static_assert(sizeof(order_priority_update_message::fields_type) == 31, "unexpected sizeof order_priority_update_message::fields_type");

#pragma pack(pop)
}

#include "details/OrderPriorityUpdateMessageWriter.hpp"
#include "details/OrderPriorityUpdateMessageReader.hpp"
