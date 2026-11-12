#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct ResendRequest {
    int32_t begin_seq_no;
    int32_t end_seq_no;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
