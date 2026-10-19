#pragma once

#include <cstddef>
#include "../types/Reserved32.hpp"
#include "../types/UserOrderBuyValueLimit.hpp"
#include "../types/UserOrderSellValueLimit.hpp"
#include "../types/Reserved16.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct user_limits {

    nnf_orderentry::reserved_32 reserved_32;
    nnf_orderentry::user_order_buy_value_limit user_order_buy_value_limit;
    nnf_orderentry::user_order_sell_value_limit user_order_sell_value_limit;
    nnf_orderentry::reserved_16 reserved_16;

    // parse method
    static user_limits* parse(std::byte* buffer) {
        return reinterpret_cast<user_limits*>(buffer);
    }

    // parse method const
    static const user_limits* parse(const std::byte* buffer) {
        return reinterpret_cast<const user_limits*>(buffer);
    }
};

// layout verification
static_assert(offsetof(user_limits, reserved_32) == 0, "unexpected offset of user_limits::reserved_32");
static_assert(offsetof(user_limits, user_order_buy_value_limit) == 32, "unexpected offset of user_limits::user_order_buy_value_limit");
static_assert(offsetof(user_limits, user_order_sell_value_limit) == 40, "unexpected offset of user_limits::user_order_sell_value_limit");
static_assert(offsetof(user_limits, reserved_16) == 48, "unexpected offset of user_limits::reserved_16");
static_assert(sizeof(user_limits) == 64, "unexpected sizeof user_limits");

#pragma pack(pop)
}
