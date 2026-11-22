#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Handlinst.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Groupid.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Price.hpp"
#include "../types/Clientid.hpp"
#include "../types/Execbroker.hpp"
#include "../types/Account.hpp"
#include "../types/Settlmnttyp.hpp"
#include "../types/Futsettdate.hpp"
#include "../types/Execinst.hpp"
#include "../types/Displayinst.hpp"
#include "../types/Minqty.hpp"
#include "../types/Maxfloor.hpp"
#include "../types/Locatereqd.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Rule80a.hpp"
#include "../types/Pegdifference.hpp"
#include "../types/Text.hpp"
#include "../types/Expiretime.hpp"
#include "../types/Marketabilityinstructions.hpp"
#include "../types/Displayrange.hpp"
#include "../types/Displayexecinst.hpp"
#include "../types/Displayprice.hpp"
#include "../types/Displaypegdifference.hpp"
#include "../types/Crosstradeflag.hpp"
#include "../types/Customertype.hpp"
#include "../types/Locatebroker.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct newordersingle {

    fix_orderentry::clordid clordid;
    fix_orderentry::handlinst handlinst;
    fix_orderentry::symbol symbol;
    fix_orderentry::side side;
    fix_orderentry::groupid groupid;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::price price;
    fix_orderentry::clientid clientid;
    fix_orderentry::execbroker execbroker;
    fix_orderentry::account account;
    fix_orderentry::settlmnttyp settlmnttyp;
    fix_orderentry::futsettdate futsettdate;
    fix_orderentry::execinst execinst;
    fix_orderentry::displayinst displayinst;
    fix_orderentry::minqty minqty;
    fix_orderentry::maxfloor maxfloor;
    fix_orderentry::locatereqd locatereqd;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::rule80a rule80a;
    fix_orderentry::pegdifference pegdifference;
    fix_orderentry::text text;
    fix_orderentry::expiretime expiretime;
    fix_orderentry::marketabilityinstructions marketabilityinstructions;
    fix_orderentry::displayrange displayrange;
    fix_orderentry::displayexecinst displayexecinst;
    fix_orderentry::displayprice displayprice;
    fix_orderentry::displaypegdifference displaypegdifference;
    fix_orderentry::crosstradeflag crosstradeflag;
    fix_orderentry::customertype customertype;
    fix_orderentry::locatebroker locatebroker;

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
static_assert(offsetof(newordersingle, handlinst) == 256, "unexpected offset of newordersingle::handlinst");
static_assert(offsetof(newordersingle, symbol) == 257, "unexpected offset of newordersingle::symbol");
static_assert(offsetof(newordersingle, side) == 513, "unexpected offset of newordersingle::side");
static_assert(offsetof(newordersingle, groupid) == 514, "unexpected offset of newordersingle::groupid");
static_assert(offsetof(newordersingle, orderqty) == 518, "unexpected offset of newordersingle::orderqty");
static_assert(offsetof(newordersingle, ordtype) == 526, "unexpected offset of newordersingle::ordtype");
static_assert(offsetof(newordersingle, price) == 527, "unexpected offset of newordersingle::price");
static_assert(offsetof(newordersingle, clientid) == 535, "unexpected offset of newordersingle::clientid");
static_assert(offsetof(newordersingle, execbroker) == 791, "unexpected offset of newordersingle::execbroker");
static_assert(offsetof(newordersingle, account) == 1047, "unexpected offset of newordersingle::account");
static_assert(offsetof(newordersingle, settlmnttyp) == 1303, "unexpected offset of newordersingle::settlmnttyp");
static_assert(offsetof(newordersingle, futsettdate) == 1304, "unexpected offset of newordersingle::futsettdate");
static_assert(offsetof(newordersingle, execinst) == 1312, "unexpected offset of newordersingle::execinst");
static_assert(offsetof(newordersingle, displayinst) == 1568, "unexpected offset of newordersingle::displayinst");
static_assert(offsetof(newordersingle, minqty) == 1569, "unexpected offset of newordersingle::minqty");
static_assert(offsetof(newordersingle, maxfloor) == 1577, "unexpected offset of newordersingle::maxfloor");
static_assert(offsetof(newordersingle, locatereqd) == 1585, "unexpected offset of newordersingle::locatereqd");
static_assert(offsetof(newordersingle, timeinforce) == 1586, "unexpected offset of newordersingle::timeinforce");
static_assert(offsetof(newordersingle, rule80a) == 1587, "unexpected offset of newordersingle::rule80a");
static_assert(offsetof(newordersingle, pegdifference) == 1588, "unexpected offset of newordersingle::pegdifference");
static_assert(offsetof(newordersingle, text) == 1596, "unexpected offset of newordersingle::text");
static_assert(offsetof(newordersingle, expiretime) == 1852, "unexpected offset of newordersingle::expiretime");
static_assert(offsetof(newordersingle, marketabilityinstructions) == 1860, "unexpected offset of newordersingle::marketabilityinstructions");
static_assert(offsetof(newordersingle, displayrange) == 1864, "unexpected offset of newordersingle::displayrange");
static_assert(offsetof(newordersingle, displayexecinst) == 1872, "unexpected offset of newordersingle::displayexecinst");
static_assert(offsetof(newordersingle, displayprice) == 1873, "unexpected offset of newordersingle::displayprice");
static_assert(offsetof(newordersingle, displaypegdifference) == 1881, "unexpected offset of newordersingle::displaypegdifference");
static_assert(offsetof(newordersingle, crosstradeflag) == 1889, "unexpected offset of newordersingle::crosstradeflag");
static_assert(offsetof(newordersingle, customertype) == 1890, "unexpected offset of newordersingle::customertype");
static_assert(offsetof(newordersingle, locatebroker) == 1891, "unexpected offset of newordersingle::locatebroker");
static_assert(sizeof(newordersingle) == 2147, "unexpected sizeof newordersingle");

#pragma pack(pop)
}
