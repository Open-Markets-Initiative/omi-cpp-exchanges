#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iex::order_entry {

#pragma pack(push, 1)
struct SequenceReset {
    uint32_t new_seq_no;
    char gap_fill_flag;
};
#pragma pack(pop)

}  // namespace iex::order_entry
