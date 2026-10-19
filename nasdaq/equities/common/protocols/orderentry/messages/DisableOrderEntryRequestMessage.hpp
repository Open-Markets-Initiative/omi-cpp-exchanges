#pragma once

#include <cstddef>
#include "../structs/ClientPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/Firm.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/UnsequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Disable Order Entry Request Message
struct disable_order_entry_request_message {

    struct fields_type {
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::firm firm;
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::client_packet_header header = {std::uint16_t(sizeof(ouch_orders::client_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::unsequenced_message_type) - 2), client_packet_type::enum_type::unsequenced_data_packet};
    ouch_orders::unsequenced_message_type unsequenced_message_type = ouch_orders::unsequenced_message_type::enum_type::disable_order_entry_request_message;

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
    external_var_data disable_order_entry_request_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static disable_order_entry_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<disable_order_entry_request_message*>(buffer);
    }

    // parse method const
    static const disable_order_entry_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const disable_order_entry_request_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(disable_order_entry_request_message::fields_type, user_ref_num) == 0, "unexpected offset of disable_order_entry_request_message::fields_type::user_ref_num");
static_assert(offsetof(disable_order_entry_request_message::fields_type, firm) == 4, "unexpected offset of disable_order_entry_request_message::fields_type::firm");
static_assert(offsetof(disable_order_entry_request_message::fields_type, appendage_length) == 8, "unexpected offset of disable_order_entry_request_message::fields_type::appendage_length");
static_assert(sizeof(disable_order_entry_request_message::fields_type) == 10, "unexpected sizeof disable_order_entry_request_message::fields_type");

#pragma pack(pop)
}

#include "details/DisableOrderEntryRequestMessageWriter.hpp"
#include "details/DisableOrderEntryRequestMessageReader.hpp"
