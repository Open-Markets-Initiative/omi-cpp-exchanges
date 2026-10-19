#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/Timestamp.hpp"
#include "../types/OrigUserRefNum.hpp"
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

// Replaced Message
struct replaced_message {

    struct fields_type {
        ouch_orders::timestamp timestamp;
        ouch_orders::orig_user_ref_num orig_user_ref_num;
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
    ouch_orders::sequenced_message_type sequenced_message_type = ouch_orders::sequenced_message_type::enum_type::replaced_message;

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
    external_var_data replaced_message_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static replaced_message* parse(std::byte* buffer) {
        return reinterpret_cast<replaced_message*>(buffer);
    }

    // parse method const
    static const replaced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const replaced_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(replaced_message::fields_type, timestamp) == 0, "unexpected offset of replaced_message::fields_type::timestamp");
static_assert(offsetof(replaced_message::fields_type, orig_user_ref_num) == 8, "unexpected offset of replaced_message::fields_type::orig_user_ref_num");
static_assert(offsetof(replaced_message::fields_type, user_ref_num) == 12, "unexpected offset of replaced_message::fields_type::user_ref_num");
static_assert(offsetof(replaced_message::fields_type, side) == 16, "unexpected offset of replaced_message::fields_type::side");
static_assert(offsetof(replaced_message::fields_type, quantity) == 17, "unexpected offset of replaced_message::fields_type::quantity");
static_assert(offsetof(replaced_message::fields_type, symbol) == 21, "unexpected offset of replaced_message::fields_type::symbol");
static_assert(offsetof(replaced_message::fields_type, price) == 29, "unexpected offset of replaced_message::fields_type::price");
static_assert(offsetof(replaced_message::fields_type, time_in_force) == 37, "unexpected offset of replaced_message::fields_type::time_in_force");
static_assert(offsetof(replaced_message::fields_type, display) == 38, "unexpected offset of replaced_message::fields_type::display");
static_assert(offsetof(replaced_message::fields_type, order_reference_number) == 39, "unexpected offset of replaced_message::fields_type::order_reference_number");
static_assert(offsetof(replaced_message::fields_type, capacity) == 47, "unexpected offset of replaced_message::fields_type::capacity");
static_assert(offsetof(replaced_message::fields_type, inter_market_sweep_eligibility) == 48, "unexpected offset of replaced_message::fields_type::inter_market_sweep_eligibility");
static_assert(offsetof(replaced_message::fields_type, cross_type) == 49, "unexpected offset of replaced_message::fields_type::cross_type");
static_assert(offsetof(replaced_message::fields_type, order_state) == 50, "unexpected offset of replaced_message::fields_type::order_state");
static_assert(offsetof(replaced_message::fields_type, clordid) == 51, "unexpected offset of replaced_message::fields_type::clordid");
static_assert(offsetof(replaced_message::fields_type, appendage_length) == 65, "unexpected offset of replaced_message::fields_type::appendage_length");
static_assert(sizeof(replaced_message::fields_type) == 67, "unexpected sizeof replaced_message::fields_type");

#pragma pack(pop)
}

#include "details/ReplacedMessageWriter.hpp"
#include "details/ReplacedMessageReader.hpp"
