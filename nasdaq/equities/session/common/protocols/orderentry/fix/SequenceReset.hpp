#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct SequenceReset {
    char gap_fill_flag;
    uint32_t new_seq_no;
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
