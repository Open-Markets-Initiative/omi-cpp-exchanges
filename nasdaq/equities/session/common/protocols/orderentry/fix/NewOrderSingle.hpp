#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct NewOrderSingle {
    char cl_ord_id[256];
    char handl_inst;
    char symbol[256];
    char side;
    int32_t group_id;
    double order_qty;
    char ord_type;
    double price;
    char client_id[256];
    char exec_broker[256];
    char account[256];
    char settlmnt_typ;
    uint64_t fut_sett_date;
    char exec_inst[256];
    char display_inst;
    double min_qty;
    double max_floor;
    char locate_reqd;
    char time_in_force;
    char rule80_a;
    double peg_difference;
    char text[256];
    uint64_t expire_time;
    int32_t marketability_instructions;
    double display_range;
    char display_exec_inst;
    double display_price;
    double display_peg_difference;
    char cross_trade_flag;
    char customer_type;
    char locate_broker[256];
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
