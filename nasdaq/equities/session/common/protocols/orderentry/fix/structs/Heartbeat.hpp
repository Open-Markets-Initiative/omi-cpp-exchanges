#pragma once

#include <cstddef>
#include "../types/Testreqid.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct heartbeat {

    fix_orderentry::testreqid testreqid;

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
static_assert(offsetof(heartbeat, testreqid) == 0, "unexpected offset of heartbeat::testreqid");
static_assert(sizeof(heartbeat) == 256, "unexpected sizeof heartbeat");

#pragma pack(pop)
}
