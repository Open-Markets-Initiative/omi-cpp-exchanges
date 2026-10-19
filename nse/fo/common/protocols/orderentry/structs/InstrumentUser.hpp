#pragma once

#include <cstddef>
#include "../types/BranchBuyValueLimit.hpp"
#include "../types/BranchSellValueLimit.hpp"
#include "../types/BranchUsedBuyValueLimit.hpp"
#include "../types/BranchUsedSellValueLimit.hpp"
#include "../types/UserOrderBuyValueLimit.hpp"
#include "../types/UserOrderSellValueLimit.hpp"
#include "../types/UserOrderUsedBuyValueLimit.hpp"
#include "../types/UserOrderUsedSellValueLimit.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct instrument_user {

    nnf_orderentry::branch_buy_value_limit branch_buy_value_limit;
    nnf_orderentry::branch_sell_value_limit branch_sell_value_limit;
    nnf_orderentry::branch_used_buy_value_limit branch_used_buy_value_limit;
    nnf_orderentry::branch_used_sell_value_limit branch_used_sell_value_limit;
    nnf_orderentry::user_order_buy_value_limit user_order_buy_value_limit;
    nnf_orderentry::user_order_sell_value_limit user_order_sell_value_limit;
    nnf_orderentry::user_order_used_buy_value_limit user_order_used_buy_value_limit;
    nnf_orderentry::user_order_used_sell_value_limit user_order_used_sell_value_limit;

    // parse method
    static instrument_user* parse(std::byte* buffer) {
        return reinterpret_cast<instrument_user*>(buffer);
    }

    // parse method const
    static const instrument_user* parse(const std::byte* buffer) {
        return reinterpret_cast<const instrument_user*>(buffer);
    }
};

// layout verification
static_assert(offsetof(instrument_user, branch_buy_value_limit) == 0, "unexpected offset of instrument_user::branch_buy_value_limit");
static_assert(offsetof(instrument_user, branch_sell_value_limit) == 8, "unexpected offset of instrument_user::branch_sell_value_limit");
static_assert(offsetof(instrument_user, branch_used_buy_value_limit) == 16, "unexpected offset of instrument_user::branch_used_buy_value_limit");
static_assert(offsetof(instrument_user, branch_used_sell_value_limit) == 24, "unexpected offset of instrument_user::branch_used_sell_value_limit");
static_assert(offsetof(instrument_user, user_order_buy_value_limit) == 32, "unexpected offset of instrument_user::user_order_buy_value_limit");
static_assert(offsetof(instrument_user, user_order_sell_value_limit) == 40, "unexpected offset of instrument_user::user_order_sell_value_limit");
static_assert(offsetof(instrument_user, user_order_used_buy_value_limit) == 48, "unexpected offset of instrument_user::user_order_used_buy_value_limit");
static_assert(offsetof(instrument_user, user_order_used_sell_value_limit) == 56, "unexpected offset of instrument_user::user_order_used_sell_value_limit");
static_assert(sizeof(instrument_user) == 64, "unexpected sizeof instrument_user");

#pragma pack(pop)
}
