#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct OrderCancelReject {
    char cl_ord_id[256];
    char order_id[256];
    char ord_status;
    char orig_cl_ord_id[256];
    char text[256];
    int32_t cxl_rej_reason;
    char cxl_rej_response_to;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
