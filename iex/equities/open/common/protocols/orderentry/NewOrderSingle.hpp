#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iexiexequities::order_entry {

#pragma pack(push, 1)
struct NewOrderSingle {
    char cl_ord_id[256];
    char exec_inst[256];
    char handl_inst;
    double order_qty;
    char ord_type;
    double price;
    char order_capacity;
    char side;
    char symbol[256];
    char time_in_force;
    uint64_t transact_time;
    char account[256];
    char text[256];
    char symbol_sfx[256];
    double min_qty;
    double max_floor;
    char locate_reqd;
    uint64_t expire_time;
    double peg_difference;
    char anti_internalization_qualifiers_aiqid[256];
    double display_range;
    char investor_id[256];
    char extended_exec_inst;
    char min_qty_instruction;
};
#pragma pack(pop)

}  // namespace iexiexequities::order_entry
