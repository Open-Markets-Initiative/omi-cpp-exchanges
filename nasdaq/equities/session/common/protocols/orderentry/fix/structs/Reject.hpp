#pragma once

#include <cstddef>
#include "../types/Refseqnum.hpp"
#include "../types/Reftagid.hpp"
#include "../types/Refmsgtype.hpp"
#include "../types/Sessionrejectreason.hpp"
#include "../types/Text.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct reject {

    fix_orderentry::refseqnum refseqnum;
    fix_orderentry::reftagid reftagid;
    fix_orderentry::refmsgtype refmsgtype;
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
static_assert(offsetof(reject, reftagid) == 4, "unexpected offset of reject::reftagid");
static_assert(offsetof(reject, refmsgtype) == 8, "unexpected offset of reject::refmsgtype");
static_assert(offsetof(reject, sessionrejectreason) == 264, "unexpected offset of reject::sessionrejectreason");
static_assert(offsetof(reject, text) == 268, "unexpected offset of reject::text");
static_assert(sizeof(reject) == 524, "unexpected sizeof reject");

#pragma pack(pop)
}
