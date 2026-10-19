#pragma once

#include <cstddef>
#include "../types/Avgprice.hpp"
#include "../types/Clordid.hpp"
#include "../types/Cumqty.hpp"
#include "../types/Execid.hpp"
#include "../types/Execinst.hpp"
#include "../types/Execrefid.hpp"
#include "../types/Exectranstype.hpp"
#include "../types/Lastmkt.hpp"
#include "../types/Orderid.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordstatus.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Origtime.hpp"
#include "../types/Price.hpp"
#include "../types/Ordercapacity.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Transacttime.hpp"
#include "../types/Clientid.hpp"
#include "../types/Expiretime.hpp"
#include "../types/Exectype.hpp"
#include "../types/Leavesqty.hpp"
#include "../types/Minqty.hpp"
#include "../types/Maxfloor.hpp"
#include "../types/Account.hpp"
#include "../types/Symbolsfx.hpp"
#include "../types/Secondaryorderid.hpp"
#include "../types/Antiinternalizationqualifiersaiqid.hpp"
#include "../types/Extendedexecinst.hpp"
#include "../types/Effectivelimitprice.hpp"
#include "../types/Tradeliquidityindicator.hpp"
#include "../types/Feecode.hpp"
#include "../types/NewType.hpp"
#include "../structs/Partialfill.hpp"
#include "../structs/Fill.hpp"
#include "../structs/Canceled.hpp"
#include "../structs/Replaced.hpp"
#include "../types/Pendingcancel.hpp"
#include "../structs/Rejected.hpp"
#include "../structs/Restated.hpp"
#include "../types/Pendingreplace.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::iexequities::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct executionreport {

    fix_orderentry::avgprice avgprice;
    fix_orderentry::clordid clordid;
    fix_orderentry::cumqty cumqty;
    fix_orderentry::execid execid;
    fix_orderentry::execinst execinst;
    fix_orderentry::execrefid execrefid;
    fix_orderentry::exectranstype exectranstype;
    fix_orderentry::lastmkt lastmkt;
    fix_orderentry::orderid orderid;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordstatus ordstatus;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::origtime origtime;
    fix_orderentry::price price;
    fix_orderentry::ordercapacity ordercapacity;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::transacttime transacttime;
    fix_orderentry::clientid clientid;
    fix_orderentry::expiretime expiretime;
    fix_orderentry::exectype exectype;
    fix_orderentry::leavesqty leavesqty;
    fix_orderentry::minqty minqty;
    fix_orderentry::maxfloor maxfloor;
    fix_orderentry::account account;
    fix_orderentry::symbolsfx symbolsfx;
    fix_orderentry::secondaryorderid secondaryorderid;
    fix_orderentry::antiinternalizationqualifiersaiqid antiinternalizationqualifiersaiqid;
    fix_orderentry::extendedexecinst extendedexecinst;
    fix_orderentry::effectivelimitprice effectivelimitprice;
    fix_orderentry::tradeliquidityindicator tradeliquidityindicator;
    fix_orderentry::feecode feecode;
    fix_orderentry::new_type new;
    fix_orderentry::partialfill partialfill;
    fix_orderentry::fill fill;
    fix_orderentry::canceled canceled;
    fix_orderentry::replaced replaced;
    fix_orderentry::pendingcancel pendingcancel;
    fix_orderentry::rejected rejected;
    fix_orderentry::restated restated;
    fix_orderentry::pendingreplace pendingreplace;

    // parse method
    static executionreport* parse(std::byte* buffer) {
        return reinterpret_cast<executionreport*>(buffer);
    }

    // parse method const
    static const executionreport* parse(const std::byte* buffer) {
        return reinterpret_cast<const executionreport*>(buffer);
    }
};

// layout verification
static_assert(offsetof(executionreport, avgprice) == 0, "unexpected offset of executionreport::avgprice");
static_assert(offsetof(executionreport, clordid) == 8, "unexpected offset of executionreport::clordid");
static_assert(offsetof(executionreport, cumqty) == 264, "unexpected offset of executionreport::cumqty");
static_assert(offsetof(executionreport, execid) == 272, "unexpected offset of executionreport::execid");
static_assert(offsetof(executionreport, execinst) == 528, "unexpected offset of executionreport::execinst");
static_assert(offsetof(executionreport, execrefid) == 784, "unexpected offset of executionreport::execrefid");
static_assert(offsetof(executionreport, exectranstype) == 1040, "unexpected offset of executionreport::exectranstype");
static_assert(offsetof(executionreport, lastmkt) == 1041, "unexpected offset of executionreport::lastmkt");
static_assert(offsetof(executionreport, orderid) == 1297, "unexpected offset of executionreport::orderid");
static_assert(offsetof(executionreport, orderqty) == 1553, "unexpected offset of executionreport::orderqty");
static_assert(offsetof(executionreport, ordstatus) == 1561, "unexpected offset of executionreport::ordstatus");
static_assert(offsetof(executionreport, ordtype) == 1562, "unexpected offset of executionreport::ordtype");
static_assert(offsetof(executionreport, origtime) == 1563, "unexpected offset of executionreport::origtime");
static_assert(offsetof(executionreport, price) == 1571, "unexpected offset of executionreport::price");
static_assert(offsetof(executionreport, ordercapacity) == 1579, "unexpected offset of executionreport::ordercapacity");
static_assert(offsetof(executionreport, side) == 1580, "unexpected offset of executionreport::side");
static_assert(offsetof(executionreport, symbol) == 1581, "unexpected offset of executionreport::symbol");
static_assert(offsetof(executionreport, timeinforce) == 1837, "unexpected offset of executionreport::timeinforce");
static_assert(offsetof(executionreport, transacttime) == 1838, "unexpected offset of executionreport::transacttime");
static_assert(offsetof(executionreport, clientid) == 1846, "unexpected offset of executionreport::clientid");
static_assert(offsetof(executionreport, expiretime) == 2102, "unexpected offset of executionreport::expiretime");
static_assert(offsetof(executionreport, exectype) == 2110, "unexpected offset of executionreport::exectype");
static_assert(offsetof(executionreport, leavesqty) == 2111, "unexpected offset of executionreport::leavesqty");
static_assert(offsetof(executionreport, minqty) == 2119, "unexpected offset of executionreport::minqty");
static_assert(offsetof(executionreport, maxfloor) == 2127, "unexpected offset of executionreport::maxfloor");
static_assert(offsetof(executionreport, account) == 2135, "unexpected offset of executionreport::account");
static_assert(offsetof(executionreport, symbolsfx) == 2391, "unexpected offset of executionreport::symbolsfx");
static_assert(offsetof(executionreport, secondaryorderid) == 2647, "unexpected offset of executionreport::secondaryorderid");
static_assert(offsetof(executionreport, antiinternalizationqualifiersaiqid) == 2903, "unexpected offset of executionreport::antiinternalizationqualifiersaiqid");
static_assert(offsetof(executionreport, extendedexecinst) == 3159, "unexpected offset of executionreport::extendedexecinst");
static_assert(offsetof(executionreport, effectivelimitprice) == 3160, "unexpected offset of executionreport::effectivelimitprice");
static_assert(offsetof(executionreport, tradeliquidityindicator) == 3168, "unexpected offset of executionreport::tradeliquidityindicator");
static_assert(offsetof(executionreport, feecode) == 3424, "unexpected offset of executionreport::feecode");
static_assert(offsetof(executionreport, new) == 3680, "unexpected offset of executionreport::new");
static_assert(offsetof(executionreport, partialfill) == 3936, "unexpected offset of executionreport::partialfill");
static_assert(offsetof(executionreport, fill) == 4192, "unexpected offset of executionreport::fill");
static_assert(offsetof(executionreport, canceled) == 4448, "unexpected offset of executionreport::canceled");
static_assert(offsetof(executionreport, replaced) == 4704, "unexpected offset of executionreport::replaced");
static_assert(offsetof(executionreport, pendingcancel) == 4960, "unexpected offset of executionreport::pendingcancel");
static_assert(offsetof(executionreport, rejected) == 5216, "unexpected offset of executionreport::rejected");
static_assert(offsetof(executionreport, restated) == 5472, "unexpected offset of executionreport::restated");
static_assert(offsetof(executionreport, pendingreplace) == 5728, "unexpected offset of executionreport::pendingreplace");
static_assert(sizeof(executionreport) == 5984, "unexpected sizeof executionreport");

#pragma pack(pop)
}
