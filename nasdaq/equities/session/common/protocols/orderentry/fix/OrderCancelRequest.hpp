#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct OrderCancelRequest {
    char orig_cl_ord_id[256];
    char cl_ord_id[256];
    char symbol[256];
    char side;
    double order_qty;
    char order_id[256];
    char client_id[256];
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
