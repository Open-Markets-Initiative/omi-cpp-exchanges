#pragma once

#include <cstddef>
#include "../types/MarketStatusNormal.hpp"
#include "../types/MarketStatusOddlot.hpp"
#include "../types/MarketStatusSpot.hpp"
#include "../types/MarketStatusAuction.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct st_market_status {

    nnf_orderentry::market_status_normal market_status_normal;
    nnf_orderentry::market_status_oddlot market_status_oddlot;
    nnf_orderentry::market_status_spot market_status_spot;
    nnf_orderentry::market_status_auction market_status_auction;

    // parse method
    static st_market_status* parse(std::byte* buffer) {
        return reinterpret_cast<st_market_status*>(buffer);
    }

    // parse method const
    static const st_market_status* parse(const std::byte* buffer) {
        return reinterpret_cast<const st_market_status*>(buffer);
    }
};

// layout verification
static_assert(offsetof(st_market_status, market_status_normal) == 0, "unexpected offset of st_market_status::market_status_normal");
static_assert(offsetof(st_market_status, market_status_oddlot) == 2, "unexpected offset of st_market_status::market_status_oddlot");
static_assert(offsetof(st_market_status, market_status_spot) == 4, "unexpected offset of st_market_status::market_status_spot");
static_assert(offsetof(st_market_status, market_status_auction) == 6, "unexpected offset of st_market_status::market_status_auction");
static_assert(sizeof(st_market_status) == 8, "unexpected sizeof st_market_status");

#pragma pack(pop)
}
