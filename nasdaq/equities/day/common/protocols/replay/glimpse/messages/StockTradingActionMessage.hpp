#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/StockLocate.hpp"
#include "../types/TrackingNumber.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Stock.hpp"
#include "../types/TradingState.hpp"
#include "../types/Reserved1.hpp"
#include "../types/Reason.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// Stock Trading Action Message
struct stock_trading_action_message {

    struct fields_type {
        glimpse_totalview::stock_locate stock_locate;
        glimpse_totalview::tracking_number tracking_number;
        glimpse_totalview::timestamp timestamp;
        glimpse_totalview::stock stock;
        glimpse_totalview::trading_state trading_state;
        glimpse_totalview::reserved_1 reserved_1;
        glimpse_totalview::reason reason;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::stock_trading_action_message;

    fields_type fields;

    // parse method
    static stock_trading_action_message* parse(std::byte* buffer) {
        return reinterpret_cast<stock_trading_action_message*>(buffer);
    }

    // parse method const
    static const stock_trading_action_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const stock_trading_action_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(stock_trading_action_message::fields_type, stock_locate) == 0, "unexpected offset of stock_trading_action_message::fields_type::stock_locate");
static_assert(offsetof(stock_trading_action_message::fields_type, tracking_number) == 2, "unexpected offset of stock_trading_action_message::fields_type::tracking_number");
static_assert(offsetof(stock_trading_action_message::fields_type, timestamp) == 4, "unexpected offset of stock_trading_action_message::fields_type::timestamp");
static_assert(offsetof(stock_trading_action_message::fields_type, stock) == 10, "unexpected offset of stock_trading_action_message::fields_type::stock");
static_assert(offsetof(stock_trading_action_message::fields_type, trading_state) == 18, "unexpected offset of stock_trading_action_message::fields_type::trading_state");
static_assert(offsetof(stock_trading_action_message::fields_type, reserved_1) == 19, "unexpected offset of stock_trading_action_message::fields_type::reserved_1");
static_assert(offsetof(stock_trading_action_message::fields_type, reason) == 20, "unexpected offset of stock_trading_action_message::fields_type::reason");
static_assert(sizeof(stock_trading_action_message::fields_type) == 24, "unexpected sizeof stock_trading_action_message::fields_type");
static_assert(sizeof(stock_trading_action_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 24, "unexpected sizeof stock_trading_action_message");

#pragma pack(pop)
}
