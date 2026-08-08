#pragma once

#include <cstddef>
#include "../types/Testreqid.hpp"

namespace iex::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct testrequest {

    fix_orderentry::testreqid testreqid;

    // parse method
    static testrequest* parse(std::byte* buffer) {
        return reinterpret_cast<testrequest*>(buffer);
    }

    // parse method const
    static const testrequest* parse(const std::byte* buffer) {
        return reinterpret_cast<const testrequest*>(buffer);
    }
};

// layout verification
static_assert(offsetof(testrequest, testreqid) == 0, "unexpected offset of testrequest::testreqid");
static_assert(sizeof(testrequest) == 256, "unexpected sizeof testrequest");

#pragma pack(pop)
}
