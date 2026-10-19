#pragma once

#include <cstddef>
#include "../structs/ContractDesc.hpp"
#include "../types/MarketType.hpp"
#include "../types/OpenPrice.hpp"
#include "../types/HighPrice.hpp"
#include "../types/LowPrice.hpp"
#include "../types/ClosingPrice.hpp"
#include "../types/TotalQuantityTradedShort.hpp"
#include "../types/TotalValueTraded.hpp"
#include "../types/PreviousClosePrice.hpp"
#include "../types/OpenInterestLong.hpp"
#include "../types/ChgOpenInterest.hpp"
#include "../types/Indicator.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct enhncd_mkt_stats_data {

    nnf_orderentry::contract_desc contract_desc;
    nnf_orderentry::market_type market_type;
    nnf_orderentry::open_price open_price;
    nnf_orderentry::high_price high_price;
    nnf_orderentry::low_price low_price;
    nnf_orderentry::closing_price closing_price;
    nnf_orderentry::total_quantity_traded_short total_quantity_traded_short;
    nnf_orderentry::total_value_traded total_value_traded;
    nnf_orderentry::previous_close_price previous_close_price;
    nnf_orderentry::open_interest_long open_interest_long;
    nnf_orderentry::chg_open_interest chg_open_interest;
    nnf_orderentry::indicator indicator;

    // parse method
    static enhncd_mkt_stats_data* parse(std::byte* buffer) {
        return reinterpret_cast<enhncd_mkt_stats_data*>(buffer);
    }

    // parse method const
    static const enhncd_mkt_stats_data* parse(const std::byte* buffer) {
        return reinterpret_cast<const enhncd_mkt_stats_data*>(buffer);
    }
};

// layout verification
static_assert(offsetof(enhncd_mkt_stats_data, contract_desc) == 0, "unexpected offset of enhncd_mkt_stats_data::contract_desc");
static_assert(offsetof(enhncd_mkt_stats_data, market_type) == 28, "unexpected offset of enhncd_mkt_stats_data::market_type");
static_assert(offsetof(enhncd_mkt_stats_data, open_price) == 30, "unexpected offset of enhncd_mkt_stats_data::open_price");
static_assert(offsetof(enhncd_mkt_stats_data, high_price) == 34, "unexpected offset of enhncd_mkt_stats_data::high_price");
static_assert(offsetof(enhncd_mkt_stats_data, low_price) == 38, "unexpected offset of enhncd_mkt_stats_data::low_price");
static_assert(offsetof(enhncd_mkt_stats_data, closing_price) == 42, "unexpected offset of enhncd_mkt_stats_data::closing_price");
static_assert(offsetof(enhncd_mkt_stats_data, total_quantity_traded_short) == 46, "unexpected offset of enhncd_mkt_stats_data::total_quantity_traded_short");
static_assert(offsetof(enhncd_mkt_stats_data, total_value_traded) == 50, "unexpected offset of enhncd_mkt_stats_data::total_value_traded");
static_assert(offsetof(enhncd_mkt_stats_data, previous_close_price) == 58, "unexpected offset of enhncd_mkt_stats_data::previous_close_price");
static_assert(offsetof(enhncd_mkt_stats_data, open_interest_long) == 62, "unexpected offset of enhncd_mkt_stats_data::open_interest_long");
static_assert(offsetof(enhncd_mkt_stats_data, chg_open_interest) == 70, "unexpected offset of enhncd_mkt_stats_data::chg_open_interest");
static_assert(offsetof(enhncd_mkt_stats_data, indicator) == 78, "unexpected offset of enhncd_mkt_stats_data::indicator");
static_assert(sizeof(enhncd_mkt_stats_data) == 82, "unexpected sizeof enhncd_mkt_stats_data");

#pragma pack(pop)
}
