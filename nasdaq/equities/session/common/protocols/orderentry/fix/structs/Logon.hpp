#pragma once

#include <cstddef>
#include "../types/Encryptmethod.hpp"
#include "../types/Heartbtint.hpp"
#include "../types/Resetseqnumflag.hpp"
#include "../types/Rawdatalength.hpp"
#include "../types/Rawdata.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct logon {

    fix_orderentry::encryptmethod encryptmethod;
    fix_orderentry::heartbtint heartbtint;
    fix_orderentry::resetseqnumflag resetseqnumflag;
    fix_orderentry::rawdatalength rawdatalength;
    fix_orderentry::rawdata rawdata;

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
static_assert(offsetof(logon, rawdatalength) == 9, "unexpected offset of logon::rawdatalength");
static_assert(offsetof(logon, rawdata) == 13, "unexpected offset of logon::rawdata");
static_assert(sizeof(logon) == 269, "unexpected sizeof logon");

#pragma pack(pop)
}
