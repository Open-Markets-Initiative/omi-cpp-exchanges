#pragma once

#include <cstddef>
#include "../types/BranchBuyValueLimit.hpp"
#include "../types/BranchSellValueLimit.hpp"
#include "../types/Reserved16.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct branch_limits {

    nnf_orderentry::branch_buy_value_limit branch_buy_value_limit;
    nnf_orderentry::branch_sell_value_limit branch_sell_value_limit;
    nnf_orderentry::reserved_16 reserved_16;

    // parse method
    static branch_limits* parse(std::byte* buffer) {
        return reinterpret_cast<branch_limits*>(buffer);
    }

    // parse method const
    static const branch_limits* parse(const std::byte* buffer) {
        return reinterpret_cast<const branch_limits*>(buffer);
    }
};

// layout verification
static_assert(offsetof(branch_limits, branch_buy_value_limit) == 0, "unexpected offset of branch_limits::branch_buy_value_limit");
static_assert(offsetof(branch_limits, branch_sell_value_limit) == 8, "unexpected offset of branch_limits::branch_sell_value_limit");
static_assert(offsetof(branch_limits, reserved_16) == 16, "unexpected offset of branch_limits::reserved_16");
static_assert(sizeof(branch_limits) == 32, "unexpected sizeof branch_limits");

#pragma pack(pop)
}
