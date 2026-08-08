#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Execid.hpp"
#include "../types/Execrefid.hpp"
#include "../types/Exectranstype.hpp"
#include "../types/Lastpx.hpp"
#include "../types/Lastshares.hpp"
#include "../types/Orderid.hpp"
#include "../types/Origtime.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Transacttime.hpp"
#include "../types/Contrabroker.hpp"
#include "../types/Nocontrabrokers.hpp"
#include "../types/Account.hpp"
#include "../types/Symbolsfx.hpp"
#include "../types/Tradeliquidityindicator.hpp"
#include "../types/Feecode.hpp"

namespace iex::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct iextradebust {

    fix_orderentry::clordid clordid;
    fix_orderentry::execid execid;
    fix_orderentry::execrefid execrefid;
    fix_orderentry::exectranstype exectranstype;
    fix_orderentry::lastpx lastpx;
    fix_orderentry::lastshares lastshares;
    fix_orderentry::orderid orderid;
    fix_orderentry::origtime origtime;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::transacttime transacttime;
    fix_orderentry::contrabroker contrabroker;
    fix_orderentry::nocontrabrokers nocontrabrokers;
    fix_orderentry::account account;
    fix_orderentry::symbolsfx symbolsfx;
    fix_orderentry::tradeliquidityindicator tradeliquidityindicator;
    fix_orderentry::feecode feecode;

    // parse method
    static iextradebust* parse(std::byte* buffer) {
        return reinterpret_cast<iextradebust*>(buffer);
    }

    // parse method const
    static const iextradebust* parse(const std::byte* buffer) {
        return reinterpret_cast<const iextradebust*>(buffer);
    }
};

// layout verification
static_assert(offsetof(iextradebust, clordid) == 0, "unexpected offset of iextradebust::clordid");
static_assert(offsetof(iextradebust, execid) == 256, "unexpected offset of iextradebust::execid");
static_assert(offsetof(iextradebust, execrefid) == 512, "unexpected offset of iextradebust::execrefid");
static_assert(offsetof(iextradebust, exectranstype) == 768, "unexpected offset of iextradebust::exectranstype");
static_assert(offsetof(iextradebust, lastpx) == 769, "unexpected offset of iextradebust::lastpx");
static_assert(offsetof(iextradebust, lastshares) == 777, "unexpected offset of iextradebust::lastshares");
static_assert(offsetof(iextradebust, orderid) == 785, "unexpected offset of iextradebust::orderid");
static_assert(offsetof(iextradebust, origtime) == 1041, "unexpected offset of iextradebust::origtime");
static_assert(offsetof(iextradebust, side) == 1049, "unexpected offset of iextradebust::side");
static_assert(offsetof(iextradebust, symbol) == 1050, "unexpected offset of iextradebust::symbol");
static_assert(offsetof(iextradebust, transacttime) == 1306, "unexpected offset of iextradebust::transacttime");
static_assert(offsetof(iextradebust, contrabroker) == 1314, "unexpected offset of iextradebust::contrabroker");
static_assert(offsetof(iextradebust, nocontrabrokers) == 1570, "unexpected offset of iextradebust::nocontrabrokers");
static_assert(offsetof(iextradebust, account) == 1574, "unexpected offset of iextradebust::account");
static_assert(offsetof(iextradebust, symbolsfx) == 1830, "unexpected offset of iextradebust::symbolsfx");
static_assert(offsetof(iextradebust, tradeliquidityindicator) == 2086, "unexpected offset of iextradebust::tradeliquidityindicator");
static_assert(offsetof(iextradebust, feecode) == 2342, "unexpected offset of iextradebust::feecode");
static_assert(sizeof(iextradebust) == 2598, "unexpected sizeof iextradebust");

#pragma pack(pop)
}
