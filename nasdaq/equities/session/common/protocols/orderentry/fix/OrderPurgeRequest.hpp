#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct OrderPurgeRequest {
    char account[256];
    int32_t purge_request_type;
    char client_id[256];
    char symbol[256];
    char side;
    int32_t group_id;
    char cl_ord_id[256];
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
