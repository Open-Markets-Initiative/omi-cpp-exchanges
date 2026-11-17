#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Transacttime.hpp"
#include "../types/Symbolsfx.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::iexequities::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct ordercancelrequest {

    fix_orderentry::clordid clordid;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::transacttime transacttime;
    fix_orderentry::symbolsfx symbolsfx;

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
static_assert(offsetof(ordercancelrequest, symbolsfx) == 785, "unexpected offset of ordercancelrequest::symbolsfx");
static_assert(sizeof(ordercancelrequest) == 1041, "unexpected sizeof ordercancelrequest");

#pragma pack(pop)
}
