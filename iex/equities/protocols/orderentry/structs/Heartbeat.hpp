#pragma once

#include <cstddef>

namespace iex::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct heartbeat {


    // parse method
    static heartbeat* parse(std::byte* buffer) {
        return reinterpret_cast<heartbeat*>(buffer);
    }

    // parse method const
    static const heartbeat* parse(const std::byte* buffer) {
        return reinterpret_cast<const heartbeat*>(buffer);
    }
};

// layout verification
static_assert(sizeof(heartbeat) == 0, "unexpected sizeof heartbeat");

#pragma pack(pop)
}
