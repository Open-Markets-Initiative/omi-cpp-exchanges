#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct OrderCancelReject {
    char order_id[256];
    char cl_ord_id[256];
    char orig_cl_ord_id[256];
    char ord_status;
    char client_id[256];
    int32_t cxl_rej_reason;
    int32_t cxl_rej_response_to;
    char text[256];
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
