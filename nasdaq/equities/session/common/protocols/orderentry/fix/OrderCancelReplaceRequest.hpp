#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct OrderCancelReplaceRequest {
    char orig_cl_ord_id[256];
    char cl_ord_id[256];
    char handl_inst;
    char symbol[256];
    char side;
    double order_qty;
    char ord_type;
    double price;
    char locate_reqd;
    char order_id[256];
    char client_id[256];
    char exec_inst[256];
    char display_inst;
    char time_in_force;
    char rule80_a;
    char account[256];
    double min_qty;
    double max_floor;
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
