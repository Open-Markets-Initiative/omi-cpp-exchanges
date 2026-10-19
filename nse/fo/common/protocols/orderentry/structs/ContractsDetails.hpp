#pragma once

#include <cstddef>
#include "../types/Token.hpp"
#include "../types/NseSymbol.hpp"
#include "../types/InstrumentName.hpp"
#include "../types/ExpDay.hpp"
#include "../types/ExpMonth.hpp"
#include "../types/ExpYear.hpp"
#include "../types/OptionType.hpp"
#include "../types/StrikePrice.hpp"
#include "../types/BidPrice.hpp"
#include "../types/AskPrice.hpp"
#include "../types/BidSize.hpp"
#include "../types/AskSize.hpp"
#include "../types/Open.hpp"
#include "../types/High.hpp"
#include "../types/Low.hpp"
#include "../types/Last.hpp"
#include "../types/Close.hpp"
#include "../types/PrevClose.hpp"
#include "../types/LimitHigh.hpp"
#include "../types/LimitLow.hpp"
#include "../types/TotalTrades.hpp"
#include "../types/OpenInterestDouble.hpp"
#include "../types/Filler4.hpp"
#include "../types/Filler2.hpp"
#include "../types/Filler3.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct contracts_details {

    nnf_orderentry::token token;
    nnf_orderentry::nse_symbol nse_symbol;
    nnf_orderentry::instrument_name instrument_name;
    nnf_orderentry::exp_day exp_day;
    nnf_orderentry::exp_month exp_month;
    nnf_orderentry::exp_year exp_year;
    nnf_orderentry::option_type option_type;
    nnf_orderentry::strike_price strike_price;
    nnf_orderentry::bid_price bid_price;
    nnf_orderentry::ask_price ask_price;
    nnf_orderentry::bid_size bid_size;
    nnf_orderentry::ask_size ask_size;
    nnf_orderentry::open open;
    nnf_orderentry::high high;
    nnf_orderentry::low low;
    nnf_orderentry::last last;
    nnf_orderentry::close close;
    nnf_orderentry::prev_close prev_close;
    nnf_orderentry::limit_high limit_high;
    nnf_orderentry::limit_low limit_low;
    nnf_orderentry::total_trades total_trades;
    nnf_orderentry::open_interest_double open_interest_double;
    nnf_orderentry::filler_4 filler_4;
    nnf_orderentry::filler_2 filler_2;
    nnf_orderentry::filler_3 filler_3;

    // parse method
    static contracts_details* parse(std::byte* buffer) {
        return reinterpret_cast<contracts_details*>(buffer);
    }

    // parse method const
    static const contracts_details* parse(const std::byte* buffer) {
        return reinterpret_cast<const contracts_details*>(buffer);
    }
};

// layout verification
static_assert(offsetof(contracts_details, token) == 0, "unexpected offset of contracts_details::token");
static_assert(offsetof(contracts_details, nse_symbol) == 4, "unexpected offset of contracts_details::nse_symbol");
static_assert(offsetof(contracts_details, instrument_name) == 20, "unexpected offset of contracts_details::instrument_name");
static_assert(offsetof(contracts_details, exp_day) == 26, "unexpected offset of contracts_details::exp_day");
static_assert(offsetof(contracts_details, exp_month) == 28, "unexpected offset of contracts_details::exp_month");
static_assert(offsetof(contracts_details, exp_year) == 30, "unexpected offset of contracts_details::exp_year");
static_assert(offsetof(contracts_details, option_type) == 32, "unexpected offset of contracts_details::option_type");
static_assert(offsetof(contracts_details, strike_price) == 34, "unexpected offset of contracts_details::strike_price");
static_assert(offsetof(contracts_details, bid_price) == 38, "unexpected offset of contracts_details::bid_price");
static_assert(offsetof(contracts_details, ask_price) == 42, "unexpected offset of contracts_details::ask_price");
static_assert(offsetof(contracts_details, bid_size) == 46, "unexpected offset of contracts_details::bid_size");
static_assert(offsetof(contracts_details, ask_size) == 54, "unexpected offset of contracts_details::ask_size");
static_assert(offsetof(contracts_details, open) == 62, "unexpected offset of contracts_details::open");
static_assert(offsetof(contracts_details, high) == 66, "unexpected offset of contracts_details::high");
static_assert(offsetof(contracts_details, low) == 70, "unexpected offset of contracts_details::low");
static_assert(offsetof(contracts_details, last) == 74, "unexpected offset of contracts_details::last");
static_assert(offsetof(contracts_details, close) == 78, "unexpected offset of contracts_details::close");
static_assert(offsetof(contracts_details, prev_close) == 82, "unexpected offset of contracts_details::prev_close");
static_assert(offsetof(contracts_details, limit_high) == 86, "unexpected offset of contracts_details::limit_high");
static_assert(offsetof(contracts_details, limit_low) == 90, "unexpected offset of contracts_details::limit_low");
static_assert(offsetof(contracts_details, total_trades) == 94, "unexpected offset of contracts_details::total_trades");
static_assert(offsetof(contracts_details, open_interest_double) == 102, "unexpected offset of contracts_details::open_interest_double");
static_assert(offsetof(contracts_details, filler_4) == 110, "unexpected offset of contracts_details::filler_4");
static_assert(offsetof(contracts_details, filler_2) == 114, "unexpected offset of contracts_details::filler_2");
static_assert(offsetof(contracts_details, filler_3) == 118, "unexpected offset of contracts_details::filler_3");
static_assert(sizeof(contracts_details) == 122, "unexpected sizeof contracts_details");

#pragma pack(pop)
}
