#pragma once

#include <cstddef>
#include "../types/Newseqno.hpp"
#include "../types/Gapfillflag.hpp"

namespace iex::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct sequencereset {

    fix_orderentry::newseqno newseqno;
    fix_orderentry::gapfillflag gapfillflag;

    // parse method
    static sequencereset* parse(std::byte* buffer) {
        return reinterpret_cast<sequencereset*>(buffer);
    }

    // parse method const
    static const sequencereset* parse(const std::byte* buffer) {
        return reinterpret_cast<const sequencereset*>(buffer);
    }
};

// layout verification
static_assert(offsetof(sequencereset, newseqno) == 0, "unexpected offset of sequencereset::newseqno");
static_assert(offsetof(sequencereset, gapfillflag) == 4, "unexpected offset of sequencereset::gapfillflag");
static_assert(sizeof(sequencereset) == 5, "unexpected sizeof sequencereset");

#pragma pack(pop)
}
