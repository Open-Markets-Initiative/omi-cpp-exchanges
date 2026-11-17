#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iexiexequities::order_entry {

#pragma pack(push, 1)
struct Heartbeat {
    char test_req_id[256];
};
#pragma pack(pop)

}  // namespace iexiexequities::order_entry
