#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace nasdaqnsmequities::order_entry {

#pragma pack(push, 1)
struct Logon {
    int32_t encrypt_method;
    int32_t heart_bt_int;
    char reset_seq_num_flag;
    uint32_t raw_data_length;
    char raw_data[256];
};
#pragma pack(pop)

}  // namespace nasdaqnsmequities::order_entry
