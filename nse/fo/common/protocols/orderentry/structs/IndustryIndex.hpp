#pragma once

#include <cstddef>
#include "../types/IndustryName.hpp"
#include "../types/Opening.hpp"
#include "../types/High.hpp"
#include "../types/Low.hpp"
#include "../types/Closing.hpp"
#include "../types/Start.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct industry_index {

    nnf_orderentry::industry_name industry_name;
    nnf_orderentry::opening opening;
    nnf_orderentry::high high;
    nnf_orderentry::low low;
    nnf_orderentry::closing closing;
    nnf_orderentry::start start;

    // parse method
    static industry_index* parse(std::byte* buffer) {
        return reinterpret_cast<industry_index*>(buffer);
    }

    // parse method const
    static const industry_index* parse(const std::byte* buffer) {
        return reinterpret_cast<const industry_index*>(buffer);
    }
};

// layout verification
static_assert(offsetof(industry_index, industry_name) == 0, "unexpected offset of industry_index::industry_name");
static_assert(offsetof(industry_index, opening) == 15, "unexpected offset of industry_index::opening");
static_assert(offsetof(industry_index, high) == 19, "unexpected offset of industry_index::high");
static_assert(offsetof(industry_index, low) == 23, "unexpected offset of industry_index::low");
static_assert(offsetof(industry_index, closing) == 27, "unexpected offset of industry_index::closing");
static_assert(offsetof(industry_index, start) == 31, "unexpected offset of industry_index::start");
static_assert(sizeof(industry_index) == 35, "unexpected sizeof industry_index");

#pragma pack(pop)
}
