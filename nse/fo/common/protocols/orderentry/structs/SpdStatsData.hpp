#pragma once

#include <cstddef>
#include "../types/MarketType.hpp"
#include "../types/Instrumentname1.hpp"
#include "../types/Symbol1.hpp"
#include "../types/Expirydate1.hpp"
#include "../types/Strikeprice1.hpp"
#include "../types/Optiontype1.hpp"
#include "../types/Calevel1.hpp"
#include "../types/Instrumentname2.hpp"
#include "../types/Symbol2.hpp"
#include "../types/Expirydate2.hpp"
#include "../types/Strikeprice2.hpp"
#include "../types/Optiontype2.hpp"
#include "../types/Calevel2.hpp"
#include "../types/Openpd.hpp"
#include "../types/Hipd.hpp"
#include "../types/Lowpd.hpp"
#include "../types/Lasttradedpd.hpp"
#include "../types/Noofcontractstraded.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct spd_stats_data {

    nnf_orderentry::market_type market_type;
    nnf_orderentry::instrumentname_1 instrumentname_1;
    nnf_orderentry::symbol_1 symbol_1;
    nnf_orderentry::expirydate_1 expirydate_1;
    nnf_orderentry::strikeprice_1 strikeprice_1;
    nnf_orderentry::optiontype_1 optiontype_1;
    nnf_orderentry::calevel_1 calevel_1;
    nnf_orderentry::instrumentname_2 instrumentname_2;
    nnf_orderentry::symbol_2 symbol_2;
    nnf_orderentry::expirydate_2 expirydate_2;
    nnf_orderentry::strikeprice_2 strikeprice_2;
    nnf_orderentry::optiontype_2 optiontype_2;
    nnf_orderentry::calevel_2 calevel_2;
    nnf_orderentry::openpd openpd;
    nnf_orderentry::hipd hipd;
    nnf_orderentry::lowpd lowpd;
    nnf_orderentry::lasttradedpd lasttradedpd;
    nnf_orderentry::noofcontractstraded noofcontractstraded;

    // parse method
    static spd_stats_data* parse(std::byte* buffer) {
        return reinterpret_cast<spd_stats_data*>(buffer);
    }

    // parse method const
    static const spd_stats_data* parse(const std::byte* buffer) {
        return reinterpret_cast<const spd_stats_data*>(buffer);
    }
};

// layout verification
static_assert(offsetof(spd_stats_data, market_type) == 0, "unexpected offset of spd_stats_data::market_type");
static_assert(offsetof(spd_stats_data, instrumentname_1) == 2, "unexpected offset of spd_stats_data::instrumentname_1");
static_assert(offsetof(spd_stats_data, symbol_1) == 8, "unexpected offset of spd_stats_data::symbol_1");
static_assert(offsetof(spd_stats_data, expirydate_1) == 18, "unexpected offset of spd_stats_data::expirydate_1");
static_assert(offsetof(spd_stats_data, strikeprice_1) == 22, "unexpected offset of spd_stats_data::strikeprice_1");
static_assert(offsetof(spd_stats_data, optiontype_1) == 26, "unexpected offset of spd_stats_data::optiontype_1");
static_assert(offsetof(spd_stats_data, calevel_1) == 28, "unexpected offset of spd_stats_data::calevel_1");
static_assert(offsetof(spd_stats_data, instrumentname_2) == 30, "unexpected offset of spd_stats_data::instrumentname_2");
static_assert(offsetof(spd_stats_data, symbol_2) == 36, "unexpected offset of spd_stats_data::symbol_2");
static_assert(offsetof(spd_stats_data, expirydate_2) == 46, "unexpected offset of spd_stats_data::expirydate_2");
static_assert(offsetof(spd_stats_data, strikeprice_2) == 50, "unexpected offset of spd_stats_data::strikeprice_2");
static_assert(offsetof(spd_stats_data, optiontype_2) == 54, "unexpected offset of spd_stats_data::optiontype_2");
static_assert(offsetof(spd_stats_data, calevel_2) == 56, "unexpected offset of spd_stats_data::calevel_2");
static_assert(offsetof(spd_stats_data, openpd) == 58, "unexpected offset of spd_stats_data::openpd");
static_assert(offsetof(spd_stats_data, hipd) == 62, "unexpected offset of spd_stats_data::hipd");
static_assert(offsetof(spd_stats_data, lowpd) == 66, "unexpected offset of spd_stats_data::lowpd");
static_assert(offsetof(spd_stats_data, lasttradedpd) == 70, "unexpected offset of spd_stats_data::lasttradedpd");
static_assert(offsetof(spd_stats_data, noofcontractstraded) == 74, "unexpected offset of spd_stats_data::noofcontractstraded");
static_assert(sizeof(spd_stats_data) == 78, "unexpected sizeof spd_stats_data");

#pragma pack(pop)
}
