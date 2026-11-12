#pragma once

#include <cstddef>
#include "../types/Refseqnum.hpp"
#include "../types/Text.hpp"
#include "../types/Reftagid.hpp"
#include "../types/Refmsgtype.hpp"
#include "../types/Sessionrejectreason.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct reject {

    fix_orderentry::refseqnum refseqnum;
    fix_orderentry::text text;
    fix_orderentry::reftagid reftagid;
    fix_orderentry::refmsgtype refmsgtype;
    fix_orderentry::sessionrejectreason sessionrejectreason;

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
static_assert(offsetof(reject, text) == 4, "unexpected offset of reject::text");
static_assert(offsetof(reject, reftagid) == 260, "unexpected offset of reject::reftagid");
static_assert(offsetof(reject, refmsgtype) == 264, "unexpected offset of reject::refmsgtype");
static_assert(offsetof(reject, sessionrejectreason) == 520, "unexpected offset of reject::sessionrejectreason");
static_assert(sizeof(reject) == 524, "unexpected sizeof reject");

#pragma pack(pop)
}
