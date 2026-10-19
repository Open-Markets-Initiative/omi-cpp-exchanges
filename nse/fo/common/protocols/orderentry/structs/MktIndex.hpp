#pragma once

#include <cstddef>
#include "../types/MarketOpeningIndex.hpp"
#include "../types/MarketHighIndex.hpp"
#include "../types/MarketLowIndex.hpp"
#include "../types/MarketClosingIndex.hpp"
#include "../types/MarketStartIndex.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct mkt_index {

    nnf_orderentry::market_opening_index market_opening_index;
    nnf_orderentry::market_high_index market_high_index;
    nnf_orderentry::market_low_index market_low_index;
    nnf_orderentry::market_closing_index market_closing_index;
    nnf_orderentry::market_start_index market_start_index;

    // parse method
    static mkt_index* parse(std::byte* buffer) {
        return reinterpret_cast<mkt_index*>(buffer);
    }

    // parse method const
    static const mkt_index* parse(const std::byte* buffer) {
        return reinterpret_cast<const mkt_index*>(buffer);
    }
};

// layout verification
static_assert(offsetof(mkt_index, market_opening_index) == 0, "unexpected offset of mkt_index::market_opening_index");
static_assert(offsetof(mkt_index, market_high_index) == 2, "unexpected offset of mkt_index::market_high_index");
static_assert(offsetof(mkt_index, market_low_index) == 4, "unexpected offset of mkt_index::market_low_index");
static_assert(offsetof(mkt_index, market_closing_index) == 6, "unexpected offset of mkt_index::market_closing_index");
static_assert(offsetof(mkt_index, market_start_index) == 8, "unexpected offset of mkt_index::market_start_index");
static_assert(sizeof(mkt_index) == 10, "unexpected sizeof mkt_index");

#pragma pack(pop)
}
