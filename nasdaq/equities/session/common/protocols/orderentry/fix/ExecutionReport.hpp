#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct ExecutionReport {
    char order_id[256];
    char exec_id[256];
    char exec_trans_type;
    char exec_broker[256];
    char exec_type;
    char ord_status;
    char symbol[256];
    char side;
    int32_t group_id;
    double order_qty;
    double leaves_qty;
    double cum_qty;
    double avg_px;
    char exec_ref_id[256];
    char account[256];
    char cl_ord_id[256];
    double price;
    double peg_difference;
    char client_id[256];
    char exec_inst[256];
    char display_inst;
    uint64_t trade_date;
    char time_in_force;
    char ord_type;
    uint64_t transact_time;
    char rule80_a;
    char liquidity_flag;
    double max_floor;
    char customer_type;
    double intraday_indicative_price;
    char contra_broker[256];
    int32_t no_contra_brokers;
    char extended_reason_code[256];
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
