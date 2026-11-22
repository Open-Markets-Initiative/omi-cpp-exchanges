#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct ResendRequest {
    uint32_t begin_seq_no;
    uint32_t end_seq_no;
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
