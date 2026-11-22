#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct Reject {
    uint32_t ref_seq_num;
    int32_t ref_tag_id;
    char ref_msg_type[256];
    int32_t session_reject_reason;
    char text[256];
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
