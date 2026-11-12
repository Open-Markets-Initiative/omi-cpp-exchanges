#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct NewOrderSingle {
    char account[256];
    char cl_ord_id[256];
    char handl_inst;
    char idsource;
    double order_qty;
    char ord_type;
    double price;
    char rule80_a;
    char security_id[256];
    char side;
    char symbol[256];
    char time_in_force;
    uint64_t transact_time;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
