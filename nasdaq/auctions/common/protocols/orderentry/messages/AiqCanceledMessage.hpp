#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/Timestamp.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/DecrementShares.hpp"
#include "../types/OrderCancelReason.hpp"
#include "../types/QuantityPreventedFromTrading.hpp"
#include "../types/ExecutionPrice.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/AiqStrategy.hpp"
#include "../types/AppendageLength.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


#pragma pack(push, 1)

// Aiq Canceled Message
struct aiq_canceled_message {

    struct fields_type {
        ouch_orders::timestamp timestamp;
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::decrement_shares decrement_shares;
        ouch_orders::order_cancel_reason order_cancel_reason;
        ouch_orders::quantity_prevented_from_trading quantity_prevented_from_trading;
        ouch_orders::execution_price execution_price;
        ouch_orders::liquidity_flag liquidity_flag;
        ouch_orders::aiq_strategy aiq_strategy;
        ouch_orders::appendage_length appendage_length;
    };

    ouch_orders::server_packet_header header = {std::uint16_t(sizeof(ouch_orders::server_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    ouch_orders::sequenced_message_type sequenced_message_type = ouch_orders::sequenced_message_type::enum_type::aiq_canceled_message;

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
    external_var_data aiq_canceled_appendage() const {
        return { tail_begin(), fields.appendage_length.get().value(), tail_end() };
    }


    // parse method
    static aiq_canceled_message* parse(std::byte* buffer) {
        return reinterpret_cast<aiq_canceled_message*>(buffer);
    }

    // parse method const
    static const aiq_canceled_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const aiq_canceled_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(aiq_canceled_message::fields_type, timestamp) == 0, "unexpected offset of aiq_canceled_message::fields_type::timestamp");
static_assert(offsetof(aiq_canceled_message::fields_type, user_ref_num) == 8, "unexpected offset of aiq_canceled_message::fields_type::user_ref_num");
static_assert(offsetof(aiq_canceled_message::fields_type, decrement_shares) == 12, "unexpected offset of aiq_canceled_message::fields_type::decrement_shares");
static_assert(offsetof(aiq_canceled_message::fields_type, order_cancel_reason) == 16, "unexpected offset of aiq_canceled_message::fields_type::order_cancel_reason");
static_assert(offsetof(aiq_canceled_message::fields_type, quantity_prevented_from_trading) == 17, "unexpected offset of aiq_canceled_message::fields_type::quantity_prevented_from_trading");
static_assert(offsetof(aiq_canceled_message::fields_type, execution_price) == 21, "unexpected offset of aiq_canceled_message::fields_type::execution_price");
static_assert(offsetof(aiq_canceled_message::fields_type, liquidity_flag) == 29, "unexpected offset of aiq_canceled_message::fields_type::liquidity_flag");
static_assert(offsetof(aiq_canceled_message::fields_type, aiq_strategy) == 30, "unexpected offset of aiq_canceled_message::fields_type::aiq_strategy");
static_assert(offsetof(aiq_canceled_message::fields_type, appendage_length) == 31, "unexpected offset of aiq_canceled_message::fields_type::appendage_length");
static_assert(sizeof(aiq_canceled_message::fields_type) == 33, "unexpected sizeof aiq_canceled_message::fields_type");

#pragma pack(pop)
}

#include "details/AiqCanceledMessageWriter.hpp"
#include "details/AiqCanceledMessageReader.hpp"
