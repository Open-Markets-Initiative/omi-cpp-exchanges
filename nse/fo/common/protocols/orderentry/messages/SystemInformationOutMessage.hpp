#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/StMarketStatus.hpp"
#include "../structs/StExMarketStatus.hpp"
#include "../structs/StPlMarketStatus.hpp"
#include "../types/UpdatePortfolio.hpp"
#include "../types/MarketIndex.hpp"
#include "../types/DefaultSettlementPeriodNormal.hpp"
#include "../types/DefaultSettlementPeriodSpot.hpp"
#include "../types/DefaultSettlementPeriodAuction.hpp"
#include "../types/CompetitorPeriod.hpp"
#include "../types/SolicitorPeriod.hpp"
#include "../types/WarningPercent.hpp"
#include "../types/VolumeFreezePercent.hpp"
#include "../types/SnapQuoteTime.hpp"
#include "../types/Reserved2.hpp"
#include "../types/BoardLotQuantity.hpp"
#include "../types/TickSize.hpp"
#include "../types/MaximumGtcDays.hpp"
#include "../types/StStockEligibleIndicators.hpp"
#include "../types/DisclosedQuantityPercentAllowed.hpp"
#include "../types/RiskFreeInterestRate.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// System Information Out Message
struct system_information_out_message {

    struct fields_type {
        nnf_orderentry::st_market_status st_market_status;
        nnf_orderentry::st_ex_market_status st_ex_market_status;
        nnf_orderentry::st_pl_market_status st_pl_market_status;
        nnf_orderentry::update_portfolio update_portfolio;
        nnf_orderentry::market_index market_index;
        nnf_orderentry::default_settlement_period_normal default_settlement_period_normal;
        nnf_orderentry::default_settlement_period_spot default_settlement_period_spot;
        nnf_orderentry::default_settlement_period_auction default_settlement_period_auction;
        nnf_orderentry::competitor_period competitor_period;
        nnf_orderentry::solicitor_period solicitor_period;
        nnf_orderentry::warning_percent warning_percent;
        nnf_orderentry::volume_freeze_percent volume_freeze_percent;
        nnf_orderentry::snap_quote_time snap_quote_time;
        nnf_orderentry::reserved_2 reserved_2;
        nnf_orderentry::board_lot_quantity board_lot_quantity;
        nnf_orderentry::tick_size tick_size;
        nnf_orderentry::maximum_gtc_days maximum_gtc_days;
        nnf_orderentry::st_stock_eligible_indicators st_stock_eligible_indicators;
        nnf_orderentry::disclosed_quantity_percent_allowed disclosed_quantity_percent_allowed;
        nnf_orderentry::risk_free_interest_rate risk_free_interest_rate;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::system_information_out_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static system_information_out_message* parse(std::byte* buffer) {
        return reinterpret_cast<system_information_out_message*>(buffer);
    }

    // parse method const
    static const system_information_out_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const system_information_out_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(system_information_out_message::fields_type, st_market_status) == 0, "unexpected offset of system_information_out_message::fields_type::st_market_status");
static_assert(offsetof(system_information_out_message::fields_type, st_ex_market_status) == 8, "unexpected offset of system_information_out_message::fields_type::st_ex_market_status");
static_assert(offsetof(system_information_out_message::fields_type, st_pl_market_status) == 16, "unexpected offset of system_information_out_message::fields_type::st_pl_market_status");
static_assert(offsetof(system_information_out_message::fields_type, update_portfolio) == 24, "unexpected offset of system_information_out_message::fields_type::update_portfolio");
static_assert(offsetof(system_information_out_message::fields_type, market_index) == 25, "unexpected offset of system_information_out_message::fields_type::market_index");
static_assert(offsetof(system_information_out_message::fields_type, default_settlement_period_normal) == 29, "unexpected offset of system_information_out_message::fields_type::default_settlement_period_normal");
static_assert(offsetof(system_information_out_message::fields_type, default_settlement_period_spot) == 31, "unexpected offset of system_information_out_message::fields_type::default_settlement_period_spot");
static_assert(offsetof(system_information_out_message::fields_type, default_settlement_period_auction) == 33, "unexpected offset of system_information_out_message::fields_type::default_settlement_period_auction");
static_assert(offsetof(system_information_out_message::fields_type, competitor_period) == 35, "unexpected offset of system_information_out_message::fields_type::competitor_period");
static_assert(offsetof(system_information_out_message::fields_type, solicitor_period) == 37, "unexpected offset of system_information_out_message::fields_type::solicitor_period");
static_assert(offsetof(system_information_out_message::fields_type, warning_percent) == 39, "unexpected offset of system_information_out_message::fields_type::warning_percent");
static_assert(offsetof(system_information_out_message::fields_type, volume_freeze_percent) == 41, "unexpected offset of system_information_out_message::fields_type::volume_freeze_percent");
static_assert(offsetof(system_information_out_message::fields_type, snap_quote_time) == 43, "unexpected offset of system_information_out_message::fields_type::snap_quote_time");
static_assert(offsetof(system_information_out_message::fields_type, reserved_2) == 45, "unexpected offset of system_information_out_message::fields_type::reserved_2");
static_assert(offsetof(system_information_out_message::fields_type, board_lot_quantity) == 47, "unexpected offset of system_information_out_message::fields_type::board_lot_quantity");
static_assert(offsetof(system_information_out_message::fields_type, tick_size) == 51, "unexpected offset of system_information_out_message::fields_type::tick_size");
static_assert(offsetof(system_information_out_message::fields_type, maximum_gtc_days) == 55, "unexpected offset of system_information_out_message::fields_type::maximum_gtc_days");
static_assert(offsetof(system_information_out_message::fields_type, st_stock_eligible_indicators) == 57, "unexpected offset of system_information_out_message::fields_type::st_stock_eligible_indicators");
static_assert(offsetof(system_information_out_message::fields_type, disclosed_quantity_percent_allowed) == 59, "unexpected offset of system_information_out_message::fields_type::disclosed_quantity_percent_allowed");
static_assert(offsetof(system_information_out_message::fields_type, risk_free_interest_rate) == 61, "unexpected offset of system_information_out_message::fields_type::risk_free_interest_rate");
static_assert(sizeof(system_information_out_message::fields_type) == 65, "unexpected sizeof system_information_out_message::fields_type");
static_assert(sizeof(system_information_out_message) == sizeof(nnf_orderentry::message_header) + 65, "unexpected sizeof system_information_out_message");

#pragma pack(pop)
}
