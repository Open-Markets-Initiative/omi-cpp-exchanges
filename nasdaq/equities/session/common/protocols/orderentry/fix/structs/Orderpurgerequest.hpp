#pragma once

#include <cstddef>
#include "../types/Account.hpp"
#include "../types/Purgerequesttype.hpp"
#include "../types/Clientid.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Groupid.hpp"
#include "../types/Clordid.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct orderpurgerequest {

    fix_orderentry::account account;
    fix_orderentry::purgerequesttype purgerequesttype;
    fix_orderentry::clientid clientid;
    fix_orderentry::symbol symbol;
    fix_orderentry::side side;
    fix_orderentry::groupid groupid;
    fix_orderentry::clordid clordid;

    // parse method
    static orderpurgerequest* parse(std::byte* buffer) {
        return reinterpret_cast<orderpurgerequest*>(buffer);
    }

    // parse method const
    static const orderpurgerequest* parse(const std::byte* buffer) {
        return reinterpret_cast<const orderpurgerequest*>(buffer);
    }
};

// layout verification
static_assert(offsetof(orderpurgerequest, account) == 0, "unexpected offset of orderpurgerequest::account");
static_assert(offsetof(orderpurgerequest, purgerequesttype) == 256, "unexpected offset of orderpurgerequest::purgerequesttype");
static_assert(offsetof(orderpurgerequest, clientid) == 260, "unexpected offset of orderpurgerequest::clientid");
static_assert(offsetof(orderpurgerequest, symbol) == 516, "unexpected offset of orderpurgerequest::symbol");
static_assert(offsetof(orderpurgerequest, side) == 772, "unexpected offset of orderpurgerequest::side");
static_assert(offsetof(orderpurgerequest, groupid) == 773, "unexpected offset of orderpurgerequest::groupid");
static_assert(offsetof(orderpurgerequest, clordid) == 777, "unexpected offset of orderpurgerequest::clordid");
static_assert(sizeof(orderpurgerequest) == 1033, "unexpected sizeof orderpurgerequest");

#pragma pack(pop)
}
