#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct OrderCancelReplaceRequest {
    char cl_ord_id[256];
    char handl_inst;
    double order_qty;
    char ord_type;
    char orig_cl_ord_id[256];
    double price;
    char rule80_a;
    char side;
    char symbol[256];
    char time_in_force;
    uint64_t transact_time;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
