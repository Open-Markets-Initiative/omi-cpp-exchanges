#pragma once

#include <cstddef>
#include "../types/Orderid.hpp"
#include "../types/Execid.hpp"
#include "../types/Exectranstype.hpp"
#include "../types/Execbroker.hpp"
#include "../types/Exectype.hpp"
#include "../types/Ordstatus.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Groupid.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Leavesqty.hpp"
#include "../types/Cumqty.hpp"
#include "../types/Avgpx.hpp"
#include "../types/Execrefid.hpp"
#include "../types/Account.hpp"
#include "../types/Clordid.hpp"
#include "../types/Price.hpp"
#include "../types/Pegdifference.hpp"
#include "../types/Clientid.hpp"
#include "../types/Execinst.hpp"
#include "../types/Displayinst.hpp"
#include "../types/Tradedate.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Transacttime.hpp"
#include "../types/Rule80a.hpp"
#include "../types/Liquidityflag.hpp"
#include "../types/Maxfloor.hpp"
#include "../types/Customertype.hpp"
#include "../types/Intradayindicativeprice.hpp"
#include "../types/Contrabroker.hpp"
#include "../types/Nocontrabrokers.hpp"
#include "../types/Extendedreasoncode.hpp"
#include "../types/NewType.hpp"
#include "../structs/Partialfill.hpp"
#include "../structs/Fill.hpp"
#include "../structs/Canceled.hpp"
#include "../structs/Replaced.hpp"
#include "../types/Pendingcancel.hpp"
#include "../structs/Rejected.hpp"
#include "../types/Pendingnew.hpp"
#include "../structs/Restated.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct executionreport {

    fix_orderentry::orderid orderid;
    fix_orderentry::execid execid;
    fix_orderentry::exectranstype exectranstype;
    fix_orderentry::execbroker execbroker;
    fix_orderentry::exectype exectype;
    fix_orderentry::ordstatus ordstatus;
    fix_orderentry::symbol symbol;
    fix_orderentry::side side;
    fix_orderentry::groupid groupid;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::leavesqty leavesqty;
    fix_orderentry::cumqty cumqty;
    fix_orderentry::avgpx avgpx;
    fix_orderentry::execrefid execrefid;
    fix_orderentry::account account;
    fix_orderentry::clordid clordid;
    fix_orderentry::price price;
    fix_orderentry::pegdifference pegdifference;
    fix_orderentry::clientid clientid;
    fix_orderentry::execinst execinst;
    fix_orderentry::displayinst displayinst;
    fix_orderentry::tradedate tradedate;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::transacttime transacttime;
    fix_orderentry::rule80a rule80a;
    fix_orderentry::liquidityflag liquidityflag;
    fix_orderentry::maxfloor maxfloor;
    fix_orderentry::customertype customertype;
    fix_orderentry::intradayindicativeprice intradayindicativeprice;
    fix_orderentry::contrabroker contrabroker;
    fix_orderentry::nocontrabrokers nocontrabrokers;
    fix_orderentry::extendedreasoncode extendedreasoncode;
    fix_orderentry::new_type new;
    fix_orderentry::partialfill partialfill;
    fix_orderentry::fill fill;
    fix_orderentry::canceled canceled;
    fix_orderentry::replaced replaced;
    fix_orderentry::pendingcancel pendingcancel;
    fix_orderentry::rejected rejected;
    fix_orderentry::pendingnew pendingnew;
    fix_orderentry::restated restated;

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
static_assert(offsetof(executionreport, orderid) == 0, "unexpected offset of executionreport::orderid");
static_assert(offsetof(executionreport, execid) == 256, "unexpected offset of executionreport::execid");
static_assert(offsetof(executionreport, exectranstype) == 512, "unexpected offset of executionreport::exectranstype");
static_assert(offsetof(executionreport, execbroker) == 513, "unexpected offset of executionreport::execbroker");
static_assert(offsetof(executionreport, exectype) == 769, "unexpected offset of executionreport::exectype");
static_assert(offsetof(executionreport, ordstatus) == 770, "unexpected offset of executionreport::ordstatus");
static_assert(offsetof(executionreport, symbol) == 771, "unexpected offset of executionreport::symbol");
static_assert(offsetof(executionreport, side) == 1027, "unexpected offset of executionreport::side");
static_assert(offsetof(executionreport, groupid) == 1028, "unexpected offset of executionreport::groupid");
static_assert(offsetof(executionreport, orderqty) == 1032, "unexpected offset of executionreport::orderqty");
static_assert(offsetof(executionreport, leavesqty) == 1040, "unexpected offset of executionreport::leavesqty");
static_assert(offsetof(executionreport, cumqty) == 1048, "unexpected offset of executionreport::cumqty");
static_assert(offsetof(executionreport, avgpx) == 1056, "unexpected offset of executionreport::avgpx");
static_assert(offsetof(executionreport, execrefid) == 1064, "unexpected offset of executionreport::execrefid");
static_assert(offsetof(executionreport, account) == 1320, "unexpected offset of executionreport::account");
static_assert(offsetof(executionreport, clordid) == 1576, "unexpected offset of executionreport::clordid");
static_assert(offsetof(executionreport, price) == 1832, "unexpected offset of executionreport::price");
static_assert(offsetof(executionreport, pegdifference) == 1840, "unexpected offset of executionreport::pegdifference");
static_assert(offsetof(executionreport, clientid) == 1848, "unexpected offset of executionreport::clientid");
static_assert(offsetof(executionreport, execinst) == 2104, "unexpected offset of executionreport::execinst");
static_assert(offsetof(executionreport, displayinst) == 2360, "unexpected offset of executionreport::displayinst");
static_assert(offsetof(executionreport, tradedate) == 2361, "unexpected offset of executionreport::tradedate");
static_assert(offsetof(executionreport, timeinforce) == 2369, "unexpected offset of executionreport::timeinforce");
static_assert(offsetof(executionreport, ordtype) == 2370, "unexpected offset of executionreport::ordtype");
static_assert(offsetof(executionreport, transacttime) == 2371, "unexpected offset of executionreport::transacttime");
static_assert(offsetof(executionreport, rule80a) == 2379, "unexpected offset of executionreport::rule80a");
static_assert(offsetof(executionreport, liquidityflag) == 2380, "unexpected offset of executionreport::liquidityflag");
static_assert(offsetof(executionreport, maxfloor) == 2381, "unexpected offset of executionreport::maxfloor");
static_assert(offsetof(executionreport, customertype) == 2389, "unexpected offset of executionreport::customertype");
static_assert(offsetof(executionreport, intradayindicativeprice) == 2390, "unexpected offset of executionreport::intradayindicativeprice");
static_assert(offsetof(executionreport, contrabroker) == 2398, "unexpected offset of executionreport::contrabroker");
static_assert(offsetof(executionreport, nocontrabrokers) == 2654, "unexpected offset of executionreport::nocontrabrokers");
static_assert(offsetof(executionreport, extendedreasoncode) == 2658, "unexpected offset of executionreport::extendedreasoncode");
static_assert(offsetof(executionreport, new) == 2914, "unexpected offset of executionreport::new");
static_assert(offsetof(executionreport, partialfill) == 3170, "unexpected offset of executionreport::partialfill");
static_assert(offsetof(executionreport, fill) == 3426, "unexpected offset of executionreport::fill");
static_assert(offsetof(executionreport, canceled) == 3682, "unexpected offset of executionreport::canceled");
static_assert(offsetof(executionreport, replaced) == 3938, "unexpected offset of executionreport::replaced");
static_assert(offsetof(executionreport, pendingcancel) == 4194, "unexpected offset of executionreport::pendingcancel");
static_assert(offsetof(executionreport, rejected) == 4450, "unexpected offset of executionreport::rejected");
static_assert(offsetof(executionreport, pendingnew) == 4706, "unexpected offset of executionreport::pendingnew");
static_assert(offsetof(executionreport, restated) == 4962, "unexpected offset of executionreport::restated");
static_assert(sizeof(executionreport) == 5218, "unexpected sizeof executionreport");

#pragma pack(pop)
}
