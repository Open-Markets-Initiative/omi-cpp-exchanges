#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace iexiexequities::order_entry {

#pragma pack(push, 1)
struct Logout {
    char text[256];
};
#pragma pack(pop)

}  // namespace iexiexequities::order_entry
