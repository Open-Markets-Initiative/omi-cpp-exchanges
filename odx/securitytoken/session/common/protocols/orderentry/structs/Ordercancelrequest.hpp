#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Transacttime.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct ordercancelrequest {

    fix_orderentry::clordid clordid;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::transacttime transacttime;

    // parse method
    static ordercancelrequest* parse(std::byte* buffer) {
        return reinterpret_cast<ordercancelrequest*>(buffer);
    }

    // parse method const
    static const ordercancelrequest* parse(const std::byte* buffer) {
        return reinterpret_cast<const ordercancelrequest*>(buffer);
    }
};

// layout verification
static_assert(offsetof(ordercancelrequest, clordid) == 0, "unexpected offset of ordercancelrequest::clordid");
static_assert(offsetof(ordercancelrequest, orderqty) == 256, "unexpected offset of ordercancelrequest::orderqty");
static_assert(offsetof(ordercancelrequest, origclordid) == 264, "unexpected offset of ordercancelrequest::origclordid");
static_assert(offsetof(ordercancelrequest, side) == 520, "unexpected offset of ordercancelrequest::side");
static_assert(offsetof(ordercancelrequest, symbol) == 521, "unexpected offset of ordercancelrequest::symbol");
static_assert(offsetof(ordercancelrequest, transacttime) == 777, "unexpected offset of ordercancelrequest::transacttime");
static_assert(sizeof(ordercancelrequest) == 785, "unexpected sizeof ordercancelrequest");

#pragma pack(pop)
}
