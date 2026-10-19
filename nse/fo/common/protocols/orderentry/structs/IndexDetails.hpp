#pragma once

#include <cstddef>
#include "../types/Token.hpp"
#include "../types/NameType.hpp"
#include "../types/Open.hpp"
#include "../types/High.hpp"
#include "../types/Low.hpp"
#include "../types/Last.hpp"
#include "../types/Close.hpp"
#include "../types/PrevClose.hpp"
#include "../types/LifeHigh.hpp"
#include "../types/LifeLow.hpp"
#include "../types/Filler4.hpp"
#include "../types/Filler2.hpp"
#include "../types/Filler3.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct index_details {

    nnf_orderentry::token token;
    nnf_orderentry::name_type name;
    nnf_orderentry::open open;
    nnf_orderentry::high high;
    nnf_orderentry::low low;
    nnf_orderentry::last last;
    nnf_orderentry::close close;
    nnf_orderentry::prev_close prev_close;
    nnf_orderentry::life_high life_high;
    nnf_orderentry::life_low life_low;
    nnf_orderentry::filler_4 filler_4;
    nnf_orderentry::filler_2 filler_2;
    nnf_orderentry::filler_3 filler_3;

    // parse method
    static index_details* parse(std::byte* buffer) {
        return reinterpret_cast<index_details*>(buffer);
    }

    // parse method const
    static const index_details* parse(const std::byte* buffer) {
        return reinterpret_cast<const index_details*>(buffer);
    }
};

// layout verification
static_assert(offsetof(index_details, token) == 0, "unexpected offset of index_details::token");
static_assert(offsetof(index_details, name) == 4, "unexpected offset of index_details::name");
static_assert(offsetof(index_details, open) == 54, "unexpected offset of index_details::open");
static_assert(offsetof(index_details, high) == 58, "unexpected offset of index_details::high");
static_assert(offsetof(index_details, low) == 62, "unexpected offset of index_details::low");
static_assert(offsetof(index_details, last) == 66, "unexpected offset of index_details::last");
static_assert(offsetof(index_details, close) == 70, "unexpected offset of index_details::close");
static_assert(offsetof(index_details, prev_close) == 74, "unexpected offset of index_details::prev_close");
static_assert(offsetof(index_details, life_high) == 78, "unexpected offset of index_details::life_high");
static_assert(offsetof(index_details, life_low) == 82, "unexpected offset of index_details::life_low");
static_assert(offsetof(index_details, filler_4) == 86, "unexpected offset of index_details::filler_4");
static_assert(offsetof(index_details, filler_2) == 90, "unexpected offset of index_details::filler_2");
static_assert(offsetof(index_details, filler_3) == 94, "unexpected offset of index_details::filler_3");
static_assert(sizeof(index_details) == 98, "unexpected sizeof index_details");

#pragma pack(pop)
}
