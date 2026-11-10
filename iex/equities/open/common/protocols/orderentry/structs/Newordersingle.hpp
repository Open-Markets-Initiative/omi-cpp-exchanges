#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Execinst.hpp"
#include "../types/Handlinst.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Price.hpp"
#include "../types/Ordercapacity.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Transacttime.hpp"
#include "../types/Account.hpp"
#include "../types/Text.hpp"
#include "../types/Symbolsfx.hpp"
#include "../types/Minqty.hpp"
#include "../types/Maxfloor.hpp"
#include "../types/Locatereqd.hpp"
#include "../types/Expiretime.hpp"
#include "../types/Pegdifference.hpp"
#include "../types/Antiinternalizationqualifiersaiqid.hpp"
#include "../types/Displayrange.hpp"
#include "../types/Investorid.hpp"
#include "../types/Extendedexecinst.hpp"
#include "../types/Minqtyinstruction.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::iexequities::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct newordersingle {

    fix_orderentry::clordid clordid;
    fix_orderentry::execinst execinst;
    fix_orderentry::handlinst handlinst;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::price price;
    fix_orderentry::ordercapacity ordercapacity;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::transacttime transacttime;
    fix_orderentry::account account;
    fix_orderentry::text text;
    fix_orderentry::symbolsfx symbolsfx;
    fix_orderentry::minqty minqty;
    fix_orderentry::maxfloor maxfloor;
    fix_orderentry::locatereqd locatereqd;
    fix_orderentry::expiretime expiretime;
    fix_orderentry::pegdifference pegdifference;
    fix_orderentry::antiinternalizationqualifiersaiqid antiinternalizationqualifiersaiqid;
    fix_orderentry::displayrange displayrange;
    fix_orderentry::investorid investorid;
    fix_orderentry::extendedexecinst extendedexecinst;
    fix_orderentry::minqtyinstruction minqtyinstruction;

    // parse method
    static newordersingle* parse(std::byte* buffer) {
        return reinterpret_cast<newordersingle*>(buffer);
    }

    // parse method const
    static const newordersingle* parse(const std::byte* buffer) {
        return reinterpret_cast<const newordersingle*>(buffer);
    }
};

// layout verification
static_assert(offsetof(newordersingle, clordid) == 0, "unexpected offset of newordersingle::clordid");
static_assert(offsetof(newordersingle, execinst) == 256, "unexpected offset of newordersingle::execinst");
static_assert(offsetof(newordersingle, handlinst) == 512, "unexpected offset of newordersingle::handlinst");
static_assert(offsetof(newordersingle, orderqty) == 513, "unexpected offset of newordersingle::orderqty");
static_assert(offsetof(newordersingle, ordtype) == 521, "unexpected offset of newordersingle::ordtype");
static_assert(offsetof(newordersingle, price) == 522, "unexpected offset of newordersingle::price");
static_assert(offsetof(newordersingle, ordercapacity) == 530, "unexpected offset of newordersingle::ordercapacity");
static_assert(offsetof(newordersingle, side) == 531, "unexpected offset of newordersingle::side");
static_assert(offsetof(newordersingle, symbol) == 532, "unexpected offset of newordersingle::symbol");
static_assert(offsetof(newordersingle, timeinforce) == 788, "unexpected offset of newordersingle::timeinforce");
static_assert(offsetof(newordersingle, transacttime) == 789, "unexpected offset of newordersingle::transacttime");
static_assert(offsetof(newordersingle, account) == 797, "unexpected offset of newordersingle::account");
static_assert(offsetof(newordersingle, text) == 1053, "unexpected offset of newordersingle::text");
static_assert(offsetof(newordersingle, symbolsfx) == 1309, "unexpected offset of newordersingle::symbolsfx");
static_assert(offsetof(newordersingle, minqty) == 1565, "unexpected offset of newordersingle::minqty");
static_assert(offsetof(newordersingle, maxfloor) == 1573, "unexpected offset of newordersingle::maxfloor");
static_assert(offsetof(newordersingle, locatereqd) == 1581, "unexpected offset of newordersingle::locatereqd");
static_assert(offsetof(newordersingle, expiretime) == 1582, "unexpected offset of newordersingle::expiretime");
static_assert(offsetof(newordersingle, pegdifference) == 1590, "unexpected offset of newordersingle::pegdifference");
static_assert(offsetof(newordersingle, antiinternalizationqualifiersaiqid) == 1598, "unexpected offset of newordersingle::antiinternalizationqualifiersaiqid");
static_assert(offsetof(newordersingle, displayrange) == 1854, "unexpected offset of newordersingle::displayrange");
static_assert(offsetof(newordersingle, investorid) == 1862, "unexpected offset of newordersingle::investorid");
static_assert(offsetof(newordersingle, extendedexecinst) == 2118, "unexpected offset of newordersingle::extendedexecinst");
static_assert(offsetof(newordersingle, minqtyinstruction) == 2119, "unexpected offset of newordersingle::minqtyinstruction");
static_assert(sizeof(newordersingle) == 2120, "unexpected sizeof newordersingle");

#pragma pack(pop)
}
