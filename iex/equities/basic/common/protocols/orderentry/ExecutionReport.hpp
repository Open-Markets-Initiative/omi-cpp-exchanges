#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iexiexequities::order_entry {

#pragma pack(push, 1)
struct ExecutionReport {
    double avg_price;
    char cl_ord_id[256];
    double cum_qty;
    char exec_id[256];
    char exec_inst[256];
    char exec_ref_id[256];
    char exec_trans_type;
    char last_mkt[256];
    char order_id[256];
    double order_qty;
    char ord_status;
    char ord_type;
    uint64_t orig_time;
    double price;
    char order_capacity;
    char side;
    char symbol[256];
    char time_in_force;
    uint64_t transact_time;
    char client_id[256];
    uint64_t expire_time;
    char exec_type;
    double leaves_qty;
    double min_qty;
    double max_floor;
    char account[256];
    char symbol_sfx[256];
    char secondary_order_id[256];
    char anti_internalization_qualifiers_aiqid[256];
    char extended_exec_inst;
    double effective_limit_price;
    char trade_liquidity_indicator[256];
    char fee_code[256];
};
#pragma pack(pop)

}  // namespace iexiexequities::order_entry
