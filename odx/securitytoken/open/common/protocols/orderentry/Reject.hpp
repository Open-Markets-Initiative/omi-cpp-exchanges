#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct Reject {
    int32_t ref_seq_num;
    char text[256];
    int32_t ref_tag_id;
    char ref_msg_type[256];
    int32_t session_reject_reason;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
