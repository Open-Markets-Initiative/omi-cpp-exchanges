#pragma once

#include <cstddef>
#include "../types/Refseqnum.hpp"
#include "../types/Sessionrejectreason.hpp"
#include "../types/Text.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::iexequities::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct reject {

    fix_orderentry::refseqnum refseqnum;
    fix_orderentry::sessionrejectreason sessionrejectreason;
    fix_orderentry::text text;

    // parse method
    static reject* parse(std::byte* buffer) {
        return reinterpret_cast<reject*>(buffer);
    }

    // parse method const
    static const reject* parse(const std::byte* buffer) {
        return reinterpret_cast<const reject*>(buffer);
    }
};

// layout verification
static_assert(offsetof(reject, refseqnum) == 0, "unexpected offset of reject::refseqnum");
static_assert(offsetof(reject, sessionrejectreason) == 4, "unexpected offset of reject::sessionrejectreason");
static_assert(offsetof(reject, text) == 8, "unexpected offset of reject::text");
static_assert(sizeof(reject) == 264, "unexpected sizeof reject");

#pragma pack(pop)
}
