#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iexiexequities::order_entry {

#pragma pack(push, 1)
struct OrderCancelRequest {
    char cl_ord_id[256];
    double order_qty;
    char orig_cl_ord_id[256];
    char side;
    char symbol[256];
    uint64_t transact_time;
    char symbol_sfx[256];
};
#pragma pack(pop)

}  // namespace iexiexequities::order_entry
