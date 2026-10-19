#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/StockLocate.hpp"
#include "../types/TrackingNumber.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Stock.hpp"
#include "../types/MarketCode.hpp"
#include "../types/OperationalHaltAction.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// Operational Halt Message
struct operational_halt_message {

    struct fields_type {
        glimpse_totalview::stock_locate stock_locate;
        glimpse_totalview::tracking_number tracking_number;
        glimpse_totalview::timestamp timestamp;
        glimpse_totalview::stock stock;
        glimpse_totalview::market_code market_code;
        glimpse_totalview::operational_halt_action operational_halt_action;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::operational_halt_message;

    fields_type fields;

    // parse method
    static operational_halt_message* parse(std::byte* buffer) {
        return reinterpret_cast<operational_halt_message*>(buffer);
    }

    // parse method const
    static const operational_halt_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const operational_halt_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(operational_halt_message::fields_type, stock_locate) == 0, "unexpected offset of operational_halt_message::fields_type::stock_locate");
static_assert(offsetof(operational_halt_message::fields_type, tracking_number) == 2, "unexpected offset of operational_halt_message::fields_type::tracking_number");
static_assert(offsetof(operational_halt_message::fields_type, timestamp) == 4, "unexpected offset of operational_halt_message::fields_type::timestamp");
static_assert(offsetof(operational_halt_message::fields_type, stock) == 10, "unexpected offset of operational_halt_message::fields_type::stock");
static_assert(offsetof(operational_halt_message::fields_type, market_code) == 18, "unexpected offset of operational_halt_message::fields_type::market_code");
static_assert(offsetof(operational_halt_message::fields_type, operational_halt_action) == 19, "unexpected offset of operational_halt_message::fields_type::operational_halt_action");
static_assert(sizeof(operational_halt_message::fields_type) == 20, "unexpected sizeof operational_halt_message::fields_type");
static_assert(sizeof(operational_halt_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 20, "unexpected sizeof operational_halt_message");

#pragma pack(pop)
}
