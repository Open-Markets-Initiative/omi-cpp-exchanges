#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/StockLocate.hpp"
#include "../types/TrackingNumber.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Stock.hpp"
#include "../types/InterestFlag.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// Retail Interest Message
struct retail_interest_message {

    struct fields_type {
        glimpse_totalview::stock_locate stock_locate;
        glimpse_totalview::tracking_number tracking_number;
        glimpse_totalview::timestamp timestamp;
        glimpse_totalview::stock stock;
        glimpse_totalview::interest_flag interest_flag;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::retail_interest_message;

    fields_type fields;

    // parse method
    static retail_interest_message* parse(std::byte* buffer) {
        return reinterpret_cast<retail_interest_message*>(buffer);
    }

    // parse method const
    static const retail_interest_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const retail_interest_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(retail_interest_message::fields_type, stock_locate) == 0, "unexpected offset of retail_interest_message::fields_type::stock_locate");
static_assert(offsetof(retail_interest_message::fields_type, tracking_number) == 2, "unexpected offset of retail_interest_message::fields_type::tracking_number");
static_assert(offsetof(retail_interest_message::fields_type, timestamp) == 4, "unexpected offset of retail_interest_message::fields_type::timestamp");
static_assert(offsetof(retail_interest_message::fields_type, stock) == 10, "unexpected offset of retail_interest_message::fields_type::stock");
static_assert(offsetof(retail_interest_message::fields_type, interest_flag) == 18, "unexpected offset of retail_interest_message::fields_type::interest_flag");
static_assert(sizeof(retail_interest_message::fields_type) == 19, "unexpected sizeof retail_interest_message::fields_type");
static_assert(sizeof(retail_interest_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 19, "unexpected sizeof retail_interest_message");

#pragma pack(pop)
}
