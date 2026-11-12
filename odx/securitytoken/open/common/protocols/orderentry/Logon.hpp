#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct Logon {
    int32_t encrypt_method;
    int32_t heart_bt_int;
    char reset_seq_num_flag;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
