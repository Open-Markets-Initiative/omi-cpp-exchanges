#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/Timestamp.hpp"
#include "../types/UserRefNum.hpp"
#include "../types/Quantity.hpp"
#include "../types/Price.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/MatchNumber.hpp"
#include "../types/TradeCorrectionReason.hpp"
#include "../types/Clordid.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;

#pragma pack(push, 1)

// Trade Correction Message
struct trade_correction_message {

    struct fields_type {
        ouch_orders::timestamp timestamp;
        ouch_orders::user_ref_num user_ref_num;
        ouch_orders::quantity quantity;
        ouch_orders::price price;
        ouch_orders::liquidity_flag liquidity_flag;
        ouch_orders::match_number match_number;
        ouch_orders::trade_correction_reason trade_correction_reason;
        ouch_orders::clordid clordid;
    };

    ouch_orders::server_packet_header header = {std::uint16_t(sizeof(ouch_orders::server_packet_header) + sizeof(fields_type) + sizeof(ouch_orders::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    ouch_orders::sequenced_message_type sequenced_message_type = ouch_orders::sequenced_message_type::enum_type::trade_correction_message;

    fields_type fields;

    // parse method
    static trade_correction_message* parse(std::byte* buffer) {
        return reinterpret_cast<trade_correction_message*>(buffer);
    }

    // parse method const
    static const trade_correction_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const trade_correction_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(trade_correction_message::fields_type, timestamp) == 0, "unexpected offset of trade_correction_message::fields_type::timestamp");
static_assert(offsetof(trade_correction_message::fields_type, user_ref_num) == 8, "unexpected offset of trade_correction_message::fields_type::user_ref_num");
static_assert(offsetof(trade_correction_message::fields_type, quantity) == 12, "unexpected offset of trade_correction_message::fields_type::quantity");
static_assert(offsetof(trade_correction_message::fields_type, price) == 16, "unexpected offset of trade_correction_message::fields_type::price");
static_assert(offsetof(trade_correction_message::fields_type, liquidity_flag) == 24, "unexpected offset of trade_correction_message::fields_type::liquidity_flag");
static_assert(offsetof(trade_correction_message::fields_type, match_number) == 25, "unexpected offset of trade_correction_message::fields_type::match_number");
static_assert(offsetof(trade_correction_message::fields_type, trade_correction_reason) == 33, "unexpected offset of trade_correction_message::fields_type::trade_correction_reason");
static_assert(offsetof(trade_correction_message::fields_type, clordid) == 34, "unexpected offset of trade_correction_message::fields_type::clordid");
static_assert(sizeof(trade_correction_message::fields_type) == 48, "unexpected sizeof trade_correction_message::fields_type");
static_assert(sizeof(trade_correction_message) == sizeof(ouch_orders::server_packet_header) + sizeof(ouch_orders::sequenced_message_type) + 48, "unexpected sizeof trade_correction_message");

#pragma pack(pop)
}
