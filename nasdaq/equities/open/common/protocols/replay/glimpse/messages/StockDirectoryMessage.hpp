#pragma once

#include <cstddef>
#include "../structs/ServerPacketHeader.hpp"
#include "../types/StockLocate.hpp"
#include "../types/TrackingNumber.hpp"
#include "../types/Timestamp.hpp"
#include "../types/Stock.hpp"
#include "../types/MarketCategory.hpp"
#include "../types/FinancialStatusIndicator.hpp"
#include "../types/RoundLotSize.hpp"
#include "../types/RoundLotsOnly.hpp"
#include "../types/IssueClassification.hpp"
#include "../types/IssueSubType.hpp"
#include "../types/Authenticity.hpp"
#include "../types/ShortSaleThresholdIndicator.hpp"
#include "../types/IpoFlag.hpp"
#include "../types/LuldReferencePriceTier.hpp"
#include "../types/EtpFlag.hpp"
#include "../types/EtpLeverageFactor.hpp"
#include "../types/InverseIndicator.hpp"
#include "../types/SequencedMessageType.hpp"

namespace nasdaq::nsmequities::totalview::glimpse::v5_0 {

namespace glimpse_totalview = ::nasdaq::nsmequities::totalview::glimpse::v5_0;

#pragma pack(push, 1)

// Stock Directory Message
struct stock_directory_message {

    struct fields_type {
        glimpse_totalview::stock_locate stock_locate;
        glimpse_totalview::tracking_number tracking_number;
        glimpse_totalview::timestamp timestamp;
        glimpse_totalview::stock stock;
        glimpse_totalview::market_category market_category;
        glimpse_totalview::financial_status_indicator financial_status_indicator;
        glimpse_totalview::round_lot_size round_lot_size;
        glimpse_totalview::round_lots_only round_lots_only;
        glimpse_totalview::issue_classification issue_classification;
        glimpse_totalview::issue_sub_type issue_sub_type;
        glimpse_totalview::authenticity authenticity;
        glimpse_totalview::short_sale_threshold_indicator short_sale_threshold_indicator;
        glimpse_totalview::ipo_flag ipo_flag;
        glimpse_totalview::luld_reference_price_tier luld_reference_price_tier;
        glimpse_totalview::etp_flag etp_flag;
        glimpse_totalview::etp_leverage_factor etp_leverage_factor;
        glimpse_totalview::inverse_indicator inverse_indicator;
    };

    glimpse_totalview::server_packet_header header = {std::uint16_t(sizeof(glimpse_totalview::server_packet_header) + sizeof(fields_type) + sizeof(glimpse_totalview::sequenced_message_type) - 2), server_packet_type::enum_type::sequenced_data_packet};
    glimpse_totalview::sequenced_message_type sequenced_message_type = glimpse_totalview::sequenced_message_type::enum_type::stock_directory_message;

    fields_type fields;

    // parse method
    static stock_directory_message* parse(std::byte* buffer) {
        return reinterpret_cast<stock_directory_message*>(buffer);
    }

    // parse method const
    static const stock_directory_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const stock_directory_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(stock_directory_message::fields_type, stock_locate) == 0, "unexpected offset of stock_directory_message::fields_type::stock_locate");
static_assert(offsetof(stock_directory_message::fields_type, tracking_number) == 2, "unexpected offset of stock_directory_message::fields_type::tracking_number");
static_assert(offsetof(stock_directory_message::fields_type, timestamp) == 4, "unexpected offset of stock_directory_message::fields_type::timestamp");
static_assert(offsetof(stock_directory_message::fields_type, stock) == 10, "unexpected offset of stock_directory_message::fields_type::stock");
static_assert(offsetof(stock_directory_message::fields_type, market_category) == 18, "unexpected offset of stock_directory_message::fields_type::market_category");
static_assert(offsetof(stock_directory_message::fields_type, financial_status_indicator) == 19, "unexpected offset of stock_directory_message::fields_type::financial_status_indicator");
static_assert(offsetof(stock_directory_message::fields_type, round_lot_size) == 20, "unexpected offset of stock_directory_message::fields_type::round_lot_size");
static_assert(offsetof(stock_directory_message::fields_type, round_lots_only) == 24, "unexpected offset of stock_directory_message::fields_type::round_lots_only");
static_assert(offsetof(stock_directory_message::fields_type, issue_classification) == 25, "unexpected offset of stock_directory_message::fields_type::issue_classification");
static_assert(offsetof(stock_directory_message::fields_type, issue_sub_type) == 26, "unexpected offset of stock_directory_message::fields_type::issue_sub_type");
static_assert(offsetof(stock_directory_message::fields_type, authenticity) == 28, "unexpected offset of stock_directory_message::fields_type::authenticity");
static_assert(offsetof(stock_directory_message::fields_type, short_sale_threshold_indicator) == 29, "unexpected offset of stock_directory_message::fields_type::short_sale_threshold_indicator");
static_assert(offsetof(stock_directory_message::fields_type, ipo_flag) == 30, "unexpected offset of stock_directory_message::fields_type::ipo_flag");
static_assert(offsetof(stock_directory_message::fields_type, luld_reference_price_tier) == 31, "unexpected offset of stock_directory_message::fields_type::luld_reference_price_tier");
static_assert(offsetof(stock_directory_message::fields_type, etp_flag) == 32, "unexpected offset of stock_directory_message::fields_type::etp_flag");
static_assert(offsetof(stock_directory_message::fields_type, etp_leverage_factor) == 33, "unexpected offset of stock_directory_message::fields_type::etp_leverage_factor");
static_assert(offsetof(stock_directory_message::fields_type, inverse_indicator) == 37, "unexpected offset of stock_directory_message::fields_type::inverse_indicator");
static_assert(sizeof(stock_directory_message::fields_type) == 38, "unexpected sizeof stock_directory_message::fields_type");
static_assert(sizeof(stock_directory_message) == sizeof(glimpse_totalview::server_packet_header) + sizeof(glimpse_totalview::sequenced_message_type) + 38, "unexpected sizeof stock_directory_message");

#pragma pack(pop)
}
