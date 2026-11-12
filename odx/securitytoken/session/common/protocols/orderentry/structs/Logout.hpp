#pragma once

#include <cstddef>
#include "../types/Text.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct logout {

    fix_orderentry::text text;

    // parse method
    static logout* parse(std::byte* buffer) {
        return reinterpret_cast<logout*>(buffer);
    }

    // parse method const
    static const logout* parse(const std::byte* buffer) {
        return reinterpret_cast<const logout*>(buffer);
    }
};

// layout verification
static_assert(offsetof(logout, text) == 0, "unexpected offset of logout::text");
static_assert(sizeof(logout) == 256, "unexpected sizeof logout");

#pragma pack(pop)
}
