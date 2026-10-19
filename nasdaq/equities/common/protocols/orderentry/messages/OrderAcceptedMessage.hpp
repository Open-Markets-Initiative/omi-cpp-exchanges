#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/Timestamp.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/Side.hpp"
#include "../types/Quantity.hpp"
#include "../types/Symbol.hpp"
#include "../types/Price.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/Display.hpp"
#include "../types/OrderReferenceNumber.hpp"
#include "../types/Capacity.hpp"
#include "../types/InterMarketSweepEligibility.hpp"
#include "../types/CrossType.hpp"
#include "../types/OrderState.hpp"
#include "../types/Clordid.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Order Accepted Message
struct order_accepted_message {

    struct fields_type {
        ouch_orders::timestamp timestamp;
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::side side;
        ouch_orders::quantity quantity;
        ouch_orders::symbol symbol;
        ouch_orders::price price;
        ouch_orders::time_in_force time_in_force;
        ouch_orders::display display;
        ouch_orders::order_reference_number order_reference_number;
        ouch_orders::capacity capacity;
        ouch_orders::inter_market_sweep_eligibility inter_market_sweep_eligibility;
        ouch_orders::cross_type cross_type;
        ouch_orders::order_state order_state;
        ouch_orders::clordid clordid;
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::server_packet_header header = {std::uint16_t(sizeof(ouch_orders::server_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    ouch_orders::sequenced_message_type sequenced_message_type = ouch_orders::sequenced_message_type::enum_type::order_accepted_message;

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
    external_var_data order_accepted_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static order_accepted_message* parse(std::byte* buffer) {
        return reinterpret_cast<order_accepted_message*>(buffer);
    }

    // parse method const
    static const order_accepted_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_accepted_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_accepted_message::fields_type, timestamp) == 0, "unexpected offset of order_accepted_message::fields_type::timestamp");
static_assert(offsetof(order_accepted_message::fields_type, user_ref_num) == 8, "unexpected offset of order_accepted_message::fields_type::user_ref_num");
static_assert(offsetof(order_accepted_message::fields_type, side) == 12, "unexpected offset of order_accepted_message::fields_type::side");
static_assert(offsetof(order_accepted_message::fields_type, quantity) == 13, "unexpected offset of order_accepted_message::fields_type::quantity");
static_assert(offsetof(order_accepted_message::fields_type, symbol) == 17, "unexpected offset of order_accepted_message::fields_type::symbol");
static_assert(offsetof(order_accepted_message::fields_type, price) == 25, "unexpected offset of order_accepted_message::fields_type::price");
static_assert(offsetof(order_accepted_message::fields_type, time_in_force) == 33, "unexpected offset of order_accepted_message::fields_type::time_in_force");
static_assert(offsetof(order_accepted_message::fields_type, display) == 34, "unexpected offset of order_accepted_message::fields_type::display");
static_assert(offsetof(order_accepted_message::fields_type, order_reference_number) == 35, "unexpected offset of order_accepted_message::fields_type::order_reference_number");
static_assert(offsetof(order_accepted_message::fields_type, capacity) == 43, "unexpected offset of order_accepted_message::fields_type::capacity");
static_assert(offsetof(order_accepted_message::fields_type, inter_market_sweep_eligibility) == 44, "unexpected offset of order_accepted_message::fields_type::inter_market_sweep_eligibility");
static_assert(offsetof(order_accepted_message::fields_type, cross_type) == 45, "unexpected offset of order_accepted_message::fields_type::cross_type");
static_assert(offsetof(order_accepted_message::fields_type, order_state) == 46, "unexpected offset of order_accepted_message::fields_type::order_state");
static_assert(offsetof(order_accepted_message::fields_type, clordid) == 47, "unexpected offset of order_accepted_message::fields_type::clordid");
static_assert(offsetof(order_accepted_message::fields_type, appendage_length) == 61, "unexpected offset of order_accepted_message::fields_type::appendage_length");
static_assert(sizeof(order_accepted_message::fields_type) == 63, "unexpected sizeof order_accepted_message::fields_type");

#pragma pack(pop)
}

#include "details/OrderAcceptedMessageWriter.hpp"
#include "details/OrderAcceptedMessageReader.hpp"
