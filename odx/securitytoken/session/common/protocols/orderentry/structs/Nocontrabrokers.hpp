#pragma once

#include <cstddef>
#include "../types/Contrabroker.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct nocontrabrokers {

    fix_orderentry::contrabroker contrabroker;

    // parse method
    static nocontrabrokers* parse(std::byte* buffer) {
        return reinterpret_cast<nocontrabrokers*>(buffer);
    }

    // parse method const
    static const nocontrabrokers* parse(const std::byte* buffer) {
        return reinterpret_cast<const nocontrabrokers*>(buffer);
    }
};

// layout verification
static_assert(offsetof(nocontrabrokers, contrabroker) == 0, "unexpected offset of nocontrabrokers::contrabroker");
static_assert(sizeof(nocontrabrokers) == 256, "unexpected sizeof nocontrabrokers");

#pragma pack(pop)
}
