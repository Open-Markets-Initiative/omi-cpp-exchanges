#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Handlinst.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Price.hpp"
#include "../types/Rule80a.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Transacttime.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct ordercancelreplacerequest {

    fix_orderentry::clordid clordid;
    fix_orderentry::handlinst handlinst;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::price price;
    fix_orderentry::rule80a rule80a;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::transacttime transacttime;

    // parse method
    static ordercancelreplacerequest* parse(std::byte* buffer) {
        return reinterpret_cast<ordercancelreplacerequest*>(buffer);
    }

    // parse method const
    static const ordercancelreplacerequest* parse(const std::byte* buffer) {
        return reinterpret_cast<const ordercancelreplacerequest*>(buffer);
    }
};

// layout verification
static_assert(offsetof(ordercancelreplacerequest, clordid) == 0, "unexpected offset of ordercancelreplacerequest::clordid");
static_assert(offsetof(ordercancelreplacerequest, handlinst) == 256, "unexpected offset of ordercancelreplacerequest::handlinst");
static_assert(offsetof(ordercancelreplacerequest, orderqty) == 257, "unexpected offset of ordercancelreplacerequest::orderqty");
static_assert(offsetof(ordercancelreplacerequest, ordtype) == 265, "unexpected offset of ordercancelreplacerequest::ordtype");
static_assert(offsetof(ordercancelreplacerequest, origclordid) == 266, "unexpected offset of ordercancelreplacerequest::origclordid");
static_assert(offsetof(ordercancelreplacerequest, price) == 522, "unexpected offset of ordercancelreplacerequest::price");
static_assert(offsetof(ordercancelreplacerequest, rule80a) == 530, "unexpected offset of ordercancelreplacerequest::rule80a");
static_assert(offsetof(ordercancelreplacerequest, side) == 531, "unexpected offset of ordercancelreplacerequest::side");
static_assert(offsetof(ordercancelreplacerequest, symbol) == 532, "unexpected offset of ordercancelreplacerequest::symbol");
static_assert(offsetof(ordercancelreplacerequest, timeinforce) == 788, "unexpected offset of ordercancelreplacerequest::timeinforce");
static_assert(offsetof(ordercancelreplacerequest, transacttime) == 789, "unexpected offset of ordercancelreplacerequest::transacttime");
static_assert(sizeof(ordercancelreplacerequest) == 797, "unexpected sizeof ordercancelreplacerequest");

#pragma pack(pop)
}
