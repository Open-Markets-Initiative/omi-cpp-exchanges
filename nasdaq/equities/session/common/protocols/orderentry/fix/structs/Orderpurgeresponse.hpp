#pragma once

#include <cstddef>
#include "../types/Account.hpp"
#include "../types/Purgerequesttype.hpp"
#include "../types/Purgerequestresponse.hpp"
#include "../types/Rejectreason.hpp"
#include "../types/Clientid.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Groupid.hpp"
#include "../types/Clordid.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct orderpurgeresponse {

    fix_orderentry::account account;
    fix_orderentry::purgerequesttype purgerequesttype;
    fix_orderentry::purgerequestresponse purgerequestresponse;
    fix_orderentry::rejectreason rejectreason;
    fix_orderentry::clientid clientid;
    fix_orderentry::symbol symbol;
    fix_orderentry::side side;
    fix_orderentry::groupid groupid;
    fix_orderentry::clordid clordid;

    // parse method
    static orderpurgeresponse* parse(std::byte* buffer) {
        return reinterpret_cast<orderpurgeresponse*>(buffer);
    }

    // parse method const
    static const orderpurgeresponse* parse(const std::byte* buffer) {
        return reinterpret_cast<const orderpurgeresponse*>(buffer);
    }
};

// layout verification
static_assert(offsetof(orderpurgeresponse, account) == 0, "unexpected offset of orderpurgeresponse::account");
static_assert(offsetof(orderpurgeresponse, purgerequesttype) == 256, "unexpected offset of orderpurgeresponse::purgerequesttype");
static_assert(offsetof(orderpurgeresponse, purgerequestresponse) == 260, "unexpected offset of orderpurgeresponse::purgerequestresponse");
static_assert(offsetof(orderpurgeresponse, rejectreason) == 264, "unexpected offset of orderpurgeresponse::rejectreason");
static_assert(offsetof(orderpurgeresponse, clientid) == 520, "unexpected offset of orderpurgeresponse::clientid");
static_assert(offsetof(orderpurgeresponse, symbol) == 776, "unexpected offset of orderpurgeresponse::symbol");
static_assert(offsetof(orderpurgeresponse, side) == 1032, "unexpected offset of orderpurgeresponse::side");
static_assert(offsetof(orderpurgeresponse, groupid) == 1033, "unexpected offset of orderpurgeresponse::groupid");
static_assert(offsetof(orderpurgeresponse, clordid) == 1037, "unexpected offset of orderpurgeresponse::clordid");
static_assert(sizeof(orderpurgeresponse) == 1293, "unexpected sizeof orderpurgeresponse");

#pragma pack(pop)
}
