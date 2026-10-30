#pragma once

#include <cstddef>
#include "../structs/ClientPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/Side.hpp"
#include "../types/Quantity.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/UnsequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Modify Order Message
struct modify_order_message {

    struct fields_type {
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::side side;
        ouch_orders::quantity quantity;
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::client_packet_header header = {std::uint16_t(sizeof(ouch_orders::client_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::unsequenced_message_type) - 2), client_packet_type::enum_type::unsequenced_data_packet};
    ouch_orders::unsequenced_message_type unsequenced_message_type = ouch_orders::unsequenced_message_type::enum_type::modify_order_message;

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
    external_var_data modify_order_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static modify_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<modify_order_message*>(buffer);
    }

    // parse method const
    static const modify_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const modify_order_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(modify_order_message::fields_type, user_ref_num) == 0, "unexpected offset of modify_order_message::fields_type::user_ref_num");
static_assert(offsetof(modify_order_message::fields_type, side) == 4, "unexpected offset of modify_order_message::fields_type::side");
static_assert(offsetof(modify_order_message::fields_type, quantity) == 5, "unexpected offset of modify_order_message::fields_type::quantity");
static_assert(offsetof(modify_order_message::fields_type, appendage_length) == 9, "unexpected offset of modify_order_message::fields_type::appendage_length");
static_assert(sizeof(modify_order_message::fields_type) == 11, "unexpected sizeof modify_order_message::fields_type");

#pragma pack(pop)
}

#include "details/ModifyOrderMessageWriter.hpp"
#include "details/ModifyOrderMessageReader.hpp"
