#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct OrderCancelRequest {
    char cl_ord_id[256];
    double order_qty;
    char orig_cl_ord_id[256];
    char side;
    char symbol[256];
    uint64_t transact_time;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
