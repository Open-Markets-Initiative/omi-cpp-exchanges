#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iexiexequities::order_entry {

#pragma pack(push, 1)
struct OrderCancelReject {
    char cl_ord_id[256];
    char order_id[256];
    char ord_status;
    char orig_cl_ord_id[256];
    int32_t cxl_rej_reason;
    char cxl_rej_response_to;
    char account[256];
    char text[256];
    char symbol_sfx[256];
};
#pragma pack(pop)

}  // namespace iexiexequities::order_entry
