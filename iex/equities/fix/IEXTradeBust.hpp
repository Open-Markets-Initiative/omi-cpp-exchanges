#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iex::order_entry {

#pragma pack(push, 1)
struct IEXTradeBust {
    char cl_ord_id[256];
    char exec_id[256];
    char exec_ref_id[256];
    char exec_trans_type;
    double last_px;
    double last_shares;
    char order_id[256];
    uint64_t orig_time;
    char side;
    char symbol[256];
    uint64_t transact_time;
    char contra_broker[256];
    uint32_t no_contra_brokers;
    char account[256];
    char symbol_sfx[256];
    char trade_liquidity_indicator[256];
    char fee_code[256];
};
#pragma pack(pop)

}  // namespace iex::order_entry
