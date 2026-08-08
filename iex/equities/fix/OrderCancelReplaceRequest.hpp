#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iex::order_entry {

#pragma pack(push, 1)
struct OrderCancelReplaceRequest {
    char cl_ord_id[256];
    char exec_inst[256];
    char handl_inst;
    double order_qty;
    char ord_type;
    char orig_cl_ord_id[256];
    char rule80_a;
    char side;
    char symbol[256];
    char time_in_force;
    uint64_t transact_time;
    char account[256];
    double price;
    char symbol_sfx[256];
    double min_qty;
    char extended_exec_inst;
};
#pragma pack(pop)

}  // namespace iex::order_entry
