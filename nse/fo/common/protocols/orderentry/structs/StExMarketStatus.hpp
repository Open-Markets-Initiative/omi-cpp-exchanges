#pragma once

#include <cstddef>
#include "../types/ExMarketStatusNormal.hpp"
#include "../types/ExMarketStatusOddlot.hpp"
#include "../types/ExMarketStatusSpot.hpp"
#include "../types/ExMarketStatusAuction.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct st_ex_market_status {

    nnf_orderentry::ex_market_status_normal ex_market_status_normal;
    nnf_orderentry::ex_market_status_oddlot ex_market_status_oddlot;
    nnf_orderentry::ex_market_status_spot ex_market_status_spot;
    nnf_orderentry::ex_market_status_auction ex_market_status_auction;

    // parse method
    static st_ex_market_status* parse(std::byte* buffer) {
        return reinterpret_cast<st_ex_market_status*>(buffer);
    }

    // parse method const
    static const st_ex_market_status* parse(const std::byte* buffer) {
        return reinterpret_cast<const st_ex_market_status*>(buffer);
    }
};

// layout verification
static_assert(offsetof(st_ex_market_status, ex_market_status_normal) == 0, "unexpected offset of st_ex_market_status::ex_market_status_normal");
static_assert(offsetof(st_ex_market_status, ex_market_status_oddlot) == 2, "unexpected offset of st_ex_market_status::ex_market_status_oddlot");
static_assert(offsetof(st_ex_market_status, ex_market_status_spot) == 4, "unexpected offset of st_ex_market_status::ex_market_status_spot");
static_assert(offsetof(st_ex_market_status, ex_market_status_auction) == 6, "unexpected offset of st_ex_market_status::ex_market_status_auction");
static_assert(sizeof(st_ex_market_status) == 8, "unexpected sizeof st_ex_market_status");

#pragma pack(pop)
}
