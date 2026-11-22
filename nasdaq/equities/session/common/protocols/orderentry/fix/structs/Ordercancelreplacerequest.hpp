#pragma once

#include <cstddef>
#include "../types/Origclordid.hpp"
#include "../types/Clordid.hpp"
#include "../types/Handlinst.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Price.hpp"
#include "../types/Locatereqd.hpp"
#include "../types/Orderid.hpp"
#include "../types/Clientid.hpp"
#include "../types/Execinst.hpp"
#include "../types/Displayinst.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Rule80a.hpp"
#include "../types/Account.hpp"
#include "../types/Minqty.hpp"
#include "../types/Maxfloor.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct ordercancelreplacerequest {

    fix_orderentry::origclordid origclordid;
    fix_orderentry::clordid clordid;
    fix_orderentry::handlinst handlinst;
    fix_orderentry::symbol symbol;
    fix_orderentry::side side;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::price price;
    fix_orderentry::locatereqd locatereqd;
    fix_orderentry::orderid orderid;
    fix_orderentry::clientid clientid;
    fix_orderentry::execinst execinst;
    fix_orderentry::displayinst displayinst;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::rule80a rule80a;
    fix_orderentry::account account;
    fix_orderentry::minqty minqty;
    fix_orderentry::maxfloor maxfloor;

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
static_assert(offsetof(ordercancelreplacerequest, origclordid) == 0, "unexpected offset of ordercancelreplacerequest::origclordid");
static_assert(offsetof(ordercancelreplacerequest, clordid) == 256, "unexpected offset of ordercancelreplacerequest::clordid");
static_assert(offsetof(ordercancelreplacerequest, handlinst) == 512, "unexpected offset of ordercancelreplacerequest::handlinst");
static_assert(offsetof(ordercancelreplacerequest, symbol) == 513, "unexpected offset of ordercancelreplacerequest::symbol");
static_assert(offsetof(ordercancelreplacerequest, side) == 769, "unexpected offset of ordercancelreplacerequest::side");
static_assert(offsetof(ordercancelreplacerequest, orderqty) == 770, "unexpected offset of ordercancelreplacerequest::orderqty");
static_assert(offsetof(ordercancelreplacerequest, ordtype) == 778, "unexpected offset of ordercancelreplacerequest::ordtype");
static_assert(offsetof(ordercancelreplacerequest, price) == 779, "unexpected offset of ordercancelreplacerequest::price");
static_assert(offsetof(ordercancelreplacerequest, locatereqd) == 787, "unexpected offset of ordercancelreplacerequest::locatereqd");
static_assert(offsetof(ordercancelreplacerequest, orderid) == 788, "unexpected offset of ordercancelreplacerequest::orderid");
static_assert(offsetof(ordercancelreplacerequest, clientid) == 1044, "unexpected offset of ordercancelreplacerequest::clientid");
static_assert(offsetof(ordercancelreplacerequest, execinst) == 1300, "unexpected offset of ordercancelreplacerequest::execinst");
static_assert(offsetof(ordercancelreplacerequest, displayinst) == 1556, "unexpected offset of ordercancelreplacerequest::displayinst");
static_assert(offsetof(ordercancelreplacerequest, timeinforce) == 1557, "unexpected offset of ordercancelreplacerequest::timeinforce");
static_assert(offsetof(ordercancelreplacerequest, rule80a) == 1558, "unexpected offset of ordercancelreplacerequest::rule80a");
static_assert(offsetof(ordercancelreplacerequest, account) == 1559, "unexpected offset of ordercancelreplacerequest::account");
static_assert(offsetof(ordercancelreplacerequest, minqty) == 1815, "unexpected offset of ordercancelreplacerequest::minqty");
static_assert(offsetof(ordercancelreplacerequest, maxfloor) == 1823, "unexpected offset of ordercancelreplacerequest::maxfloor");
static_assert(sizeof(ordercancelreplacerequest) == 1831, "unexpected sizeof ordercancelreplacerequest");

#pragma pack(pop)
}
