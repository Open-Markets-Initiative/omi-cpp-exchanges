#pragma once

#include <cstddef>
#include "../types/Encryptmethod.hpp"
#include "../types/Heartbtint.hpp"
#include "../types/Resetseqnumflag.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct logon {

    fix_orderentry::encryptmethod encryptmethod;
    fix_orderentry::heartbtint heartbtint;
    fix_orderentry::resetseqnumflag resetseqnumflag;

    // parse method
    static logon* parse(std::byte* buffer) {
        return reinterpret_cast<logon*>(buffer);
    }

    // parse method const
    static const logon* parse(const std::byte* buffer) {
        return reinterpret_cast<const logon*>(buffer);
    }
};

// layout verification
static_assert(offsetof(logon, encryptmethod) == 0, "unexpected offset of logon::encryptmethod");
static_assert(offsetof(logon, heartbtint) == 4, "unexpected offset of logon::heartbtint");
static_assert(offsetof(logon, resetseqnumflag) == 8, "unexpected offset of logon::resetseqnumflag");
static_assert(sizeof(logon) == 9, "unexpected sizeof logon");

#pragma pack(pop)
}
