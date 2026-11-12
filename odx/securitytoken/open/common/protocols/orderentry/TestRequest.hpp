#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct TestRequest {
    char test_req_id[256];
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
