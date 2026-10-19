#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/StockLocate.hpp"
#include "../types/TrackingNumber.hpp"
#include "../types/Timestamp.hpp"
#include "../types/OrderReferenceNumber.hpp"
#include "../types/BuySellIndicator.hpp"
#include "../types/Shares.hpp"
#include "../types/Stock.hpp"
#include "../types/Price.hpp"
#include "../types/Attribution.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// Add Order Mpid Attribution Message
struct add_order_mpid_attribution_message {

    struct fields_type {
        glimpse_totalview::stock_locate stock_locate;
        glimpse_totalview::tracking_number tracking_number;
        glimpse_totalview::timestamp timestamp;
        glimpse_totalview::order_reference_number order_reference_number;
        glimpse_totalview::buy_sell_indicator buy_sell_indicator;
        glimpse_totalview::shares shares;
        glimpse_totalview::stock stock;
        glimpse_totalview::price price;
        glimpse_totalview::attribution attribution;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::add_order_mpid_attribution_message;

    fields_type fields;

    // parse method
    static add_order_mpid_attribution_message* parse(std::byte* buffer) {
        return reinterpret_cast<add_order_mpid_attribution_message*>(buffer);
    }

    // parse method const
    static const add_order_mpid_attribution_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const add_order_mpid_attribution_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, stock_locate) == 0, "unexpected offset of add_order_mpid_attribution_message::fields_type::stock_locate");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, tracking_number) == 2, "unexpected offset of add_order_mpid_attribution_message::fields_type::tracking_number");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, timestamp) == 4, "unexpected offset of add_order_mpid_attribution_message::fields_type::timestamp");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, order_reference_number) == 10, "unexpected offset of add_order_mpid_attribution_message::fields_type::order_reference_number");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, buy_sell_indicator) == 18, "unexpected offset of add_order_mpid_attribution_message::fields_type::buy_sell_indicator");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, shares) == 19, "unexpected offset of add_order_mpid_attribution_message::fields_type::shares");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, stock) == 23, "unexpected offset of add_order_mpid_attribution_message::fields_type::stock");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, price) == 31, "unexpected offset of add_order_mpid_attribution_message::fields_type::price");
static_assert(offsetof(add_order_mpid_attribution_message::fields_type, attribution) == 35, "unexpected offset of add_order_mpid_attribution_message::fields_type::attribution");
static_assert(sizeof(add_order_mpid_attribution_message::fields_type) == 39, "unexpected sizeof add_order_mpid_attribution_message::fields_type");
static_assert(sizeof(add_order_mpid_attribution_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 39, "unexpected sizeof add_order_mpid_attribution_message");

#pragma pack(pop)
}
