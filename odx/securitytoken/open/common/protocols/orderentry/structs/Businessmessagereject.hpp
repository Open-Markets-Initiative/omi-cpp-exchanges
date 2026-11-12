#pragma once

#include <cstddef>
#include "../types/Refseqnum.hpp"
#include "../types/Text.hpp"
#include "../types/Refmsgtype.hpp"
#include "../types/Businessrejectreason.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct businessmessagereject {

    fix_orderentry::refseqnum refseqnum;
    fix_orderentry::text text;
    fix_orderentry::refmsgtype refmsgtype;
    fix_orderentry::businessrejectreason businessrejectreason;

    // parse method
    static businessmessagereject* parse(std::byte* buffer) {
        return reinterpret_cast<businessmessagereject*>(buffer);
    }

    // parse method const
    static const businessmessagereject* parse(const std::byte* buffer) {
        return reinterpret_cast<const businessmessagereject*>(buffer);
    }
};

// layout verification
static_assert(offsetof(businessmessagereject, refseqnum) == 0, "unexpected offset of businessmessagereject::refseqnum");
static_assert(offsetof(businessmessagereject, text) == 4, "unexpected offset of businessmessagereject::text");
static_assert(offsetof(businessmessagereject, refmsgtype) == 260, "unexpected offset of businessmessagereject::refmsgtype");
static_assert(offsetof(businessmessagereject, businessrejectreason) == 516, "unexpected offset of businessmessagereject::businessrejectreason");
static_assert(sizeof(businessmessagereject) == 520, "unexpected sizeof businessmessagereject");

#pragma pack(pop)
}
