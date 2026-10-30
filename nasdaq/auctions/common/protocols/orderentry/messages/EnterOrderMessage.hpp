#pragma once

#include <cstddef>
#include "../structs/ClientPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/Side.hpp"
#include "../types/Quantity.hpp"
#include "../types/Symbol.hpp"
#include "../types/Price.hpp"
#include "../types/TimeInForce.hpp"
#include "../types/Display.hpp"
#include "../types/Capacity.hpp"
#include "../types/InterMarketSweepEligibility.hpp"
#include "../types/CrossType.hpp"
#include "../types/Clordid.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/UnsequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Enter Order Message
struct enter_order_message {

    struct fields_type {
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::side side;
        ouch_orders::quantity quantity;
        ouch_orders::symbol symbol;
        ouch_orders::price price;
        ouch_orders::time_in_force time_in_force;
        ouch_orders::display display;
        ouch_orders::capacity capacity;
        ouch_orders::inter_market_sweep_eligibility inter_market_sweep_eligibility;
        ouch_orders::cross_type cross_type;
        ouch_orders::clordid clordid;
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::client_packet_header header = {std::uint16_t(sizeof(ouch_orders::client_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::unsequenced_message_type) - 2), client_packet_type::enum_type::unsequenced_data_packet};
    ouch_orders::unsequenced_message_type unsequenced_message_type = ouch_orders::unsequenced_message_type::enum_type::enter_order_message;

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
    external_var_data enter_order_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static enter_order_message* parse(std::byte* buffer) {
        return reinterpret_cast<enter_order_message*>(buffer);
    }

    // parse method const
    static const enter_order_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const enter_order_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(enter_order_message::fields_type, user_ref_num) == 0, "unexpected offset of enter_order_message::fields_type::user_ref_num");
static_assert(offsetof(enter_order_message::fields_type, side) == 4, "unexpected offset of enter_order_message::fields_type::side");
static_assert(offsetof(enter_order_message::fields_type, quantity) == 5, "unexpected offset of enter_order_message::fields_type::quantity");
static_assert(offsetof(enter_order_message::fields_type, symbol) == 9, "unexpected offset of enter_order_message::fields_type::symbol");
static_assert(offsetof(enter_order_message::fields_type, price) == 17, "unexpected offset of enter_order_message::fields_type::price");
static_assert(offsetof(enter_order_message::fields_type, time_in_force) == 25, "unexpected offset of enter_order_message::fields_type::time_in_force");
static_assert(offsetof(enter_order_message::fields_type, display) == 26, "unexpected offset of enter_order_message::fields_type::display");
static_assert(offsetof(enter_order_message::fields_type, capacity) == 27, "unexpected offset of enter_order_message::fields_type::capacity");
static_assert(offsetof(enter_order_message::fields_type, inter_market_sweep_eligibility) == 28, "unexpected offset of enter_order_message::fields_type::inter_market_sweep_eligibility");
static_assert(offsetof(enter_order_message::fields_type, cross_type) == 29, "unexpected offset of enter_order_message::fields_type::cross_type");
static_assert(offsetof(enter_order_message::fields_type, clordid) == 30, "unexpected offset of enter_order_message::fields_type::clordid");
static_assert(offsetof(enter_order_message::fields_type, appendage_length) == 44, "unexpected offset of enter_order_message::fields_type::appendage_length");
static_assert(sizeof(enter_order_message::fields_type) == 46, "unexpected sizeof enter_order_message::fields_type");

#pragma pack(pop)
}

#include "details/EnterOrderMessageWriter.hpp"
#include "details/EnterOrderMessageReader.hpp"
