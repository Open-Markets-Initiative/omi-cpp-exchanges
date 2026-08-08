#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Execinst.hpp"
#include "../types/Handlinst.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Rule80a.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Transacttime.hpp"
#include "../types/Account.hpp"
#include "../types/Price.hpp"
#include "../types/Symbolsfx.hpp"
#include "../types/Minqty.hpp"
#include "../types/Extendedexecinst.hpp"

namespace iex::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct ordercancelreplacerequest {

    fix_orderentry::clordid clordid;
    fix_orderentry::execinst execinst;
    fix_orderentry::handlinst handlinst;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::rule80a rule80a;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::transacttime transacttime;
    fix_orderentry::account account;
    fix_orderentry::price price;
    fix_orderentry::symbolsfx symbolsfx;
    fix_orderentry::minqty minqty;
    fix_orderentry::extendedexecinst extendedexecinst;

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
static_assert(offsetof(ordercancelreplacerequest, execinst) == 256, "unexpected offset of ordercancelreplacerequest::execinst");
static_assert(offsetof(ordercancelreplacerequest, handlinst) == 512, "unexpected offset of ordercancelreplacerequest::handlinst");
static_assert(offsetof(ordercancelreplacerequest, orderqty) == 513, "unexpected offset of ordercancelreplacerequest::orderqty");
static_assert(offsetof(ordercancelreplacerequest, ordtype) == 521, "unexpected offset of ordercancelreplacerequest::ordtype");
static_assert(offsetof(ordercancelreplacerequest, origclordid) == 522, "unexpected offset of ordercancelreplacerequest::origclordid");
static_assert(offsetof(ordercancelreplacerequest, rule80a) == 778, "unexpected offset of ordercancelreplacerequest::rule80a");
static_assert(offsetof(ordercancelreplacerequest, side) == 779, "unexpected offset of ordercancelreplacerequest::side");
static_assert(offsetof(ordercancelreplacerequest, symbol) == 780, "unexpected offset of ordercancelreplacerequest::symbol");
static_assert(offsetof(ordercancelreplacerequest, timeinforce) == 1036, "unexpected offset of ordercancelreplacerequest::timeinforce");
static_assert(offsetof(ordercancelreplacerequest, transacttime) == 1037, "unexpected offset of ordercancelreplacerequest::transacttime");
static_assert(offsetof(ordercancelreplacerequest, account) == 1045, "unexpected offset of ordercancelreplacerequest::account");
static_assert(offsetof(ordercancelreplacerequest, price) == 1301, "unexpected offset of ordercancelreplacerequest::price");
static_assert(offsetof(ordercancelreplacerequest, symbolsfx) == 1309, "unexpected offset of ordercancelreplacerequest::symbolsfx");
static_assert(offsetof(ordercancelreplacerequest, minqty) == 1565, "unexpected offset of ordercancelreplacerequest::minqty");
static_assert(offsetof(ordercancelreplacerequest, extendedexecinst) == 1573, "unexpected offset of ordercancelreplacerequest::extendedexecinst");
static_assert(sizeof(ordercancelreplacerequest) == 1574, "unexpected sizeof ordercancelreplacerequest");

#pragma pack(pop)
}
