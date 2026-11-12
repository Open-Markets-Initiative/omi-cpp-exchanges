#pragma once

#include <cstddef>
#include "../types/Beginseqno.hpp"
#include "../types/Endseqno.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct resendrequest {

    fix_orderentry::beginseqno beginseqno;
    fix_orderentry::endseqno endseqno;

    // parse method
    static resendrequest* parse(std::byte* buffer) {
        return reinterpret_cast<resendrequest*>(buffer);
    }

    // parse method const
    static const resendrequest* parse(const std::byte* buffer) {
        return reinterpret_cast<const resendrequest*>(buffer);
    }
};

// layout verification
static_assert(offsetof(resendrequest, beginseqno) == 0, "unexpected offset of resendrequest::beginseqno");
static_assert(offsetof(resendrequest, endseqno) == 4, "unexpected offset of resendrequest::endseqno");
static_assert(sizeof(resendrequest) == 8, "unexpected sizeof resendrequest");

#pragma pack(pop)
}
