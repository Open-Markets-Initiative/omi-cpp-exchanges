#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iex::order_entry {

#pragma pack(push, 1)
struct Logon {
    int32_t heart_bt_int;
};
#pragma pack(pop)

}  // namespace iex::order_entry
