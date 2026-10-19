#pragma once

#include <cstddef>
#include "../types/Heartbtint.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::iexequities::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct logon {

    fix_orderentry::heartbtint heartbtint;

    // parse method
    static logon* parse(std::byte* buffer) {
        return reinterpret_cast<logon*>(buffer);
    }

    // parse method const
    static const logon* parse(const std::byte* buffer) {
        return reinterpret_cast<const logon*>(buffer);
    }
};

// layout verification
static_assert(offsetof(logon, heartbtint) == 0, "unexpected offset of logon::heartbtint");
static_assert(sizeof(logon) == 4, "unexpected sizeof logon");

#pragma pack(pop)
}
