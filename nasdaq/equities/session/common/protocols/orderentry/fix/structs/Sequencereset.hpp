#pragma once

#include <cstddef>
#include "../types/Gapfillflag.hpp"
#include "../types/Newseqno.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct sequencereset {

    fix_orderentry::gapfillflag gapfillflag;
    fix_orderentry::newseqno newseqno;

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
static_assert(offsetof(sequencereset, gapfillflag) == 0, "unexpected offset of sequencereset::gapfillflag");
static_assert(offsetof(sequencereset, newseqno) == 1, "unexpected offset of sequencereset::newseqno");
static_assert(sizeof(sequencereset) == 5, "unexpected sizeof sequencereset");

#pragma pack(pop)
}
