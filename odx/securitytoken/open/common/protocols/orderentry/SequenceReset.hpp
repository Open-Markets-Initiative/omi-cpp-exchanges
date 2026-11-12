#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct SequenceReset {
    int32_t new_seq_no;
    char gap_fill_flag;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
