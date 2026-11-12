#pragma once

#include <cstddef>
#include "../types/Account.hpp"
#include "../types/Avgpx.hpp"
#include "../types/Clordid.hpp"
#include "../types/Cumqty.hpp"
#include "../types/Execid.hpp"
#include "../types/Exectranstype.hpp"
#include "../types/Lastpx.hpp"
#include "../types/Lastshares.hpp"
#include "../types/Orderid.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordstatus.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Price.hpp"
#include "../types/Rule80a.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Text.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Transacttime.hpp"
#include "../types/Ordrejreason.hpp"
#include "../types/Exectype.hpp"
#include "../types/Leavesqty.hpp"
#include "../types/Execrestatementreason.hpp"
#include "../structs/Nocontrabrokers.hpp"
#include "../types/Trdmatchid.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct executionreport {

    fix_orderentry::account account;
    fix_orderentry::avgpx avgpx;
    fix_orderentry::clordid clordid;
    fix_orderentry::cumqty cumqty;
    fix_orderentry::execid execid;
    fix_orderentry::exectranstype exectranstype;
    fix_orderentry::lastpx lastpx;
    fix_orderentry::lastshares lastshares;
    fix_orderentry::orderid orderid;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordstatus ordstatus;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::price price;
    fix_orderentry::rule80a rule80a;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::text text;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::transacttime transacttime;
    fix_orderentry::ordrejreason ordrejreason;
    fix_orderentry::exectype exectype;
    fix_orderentry::leavesqty leavesqty;
    fix_orderentry::execrestatementreason execrestatementreason;
    fix_orderentry::nocontrabrokers nocontrabrokers;
    fix_orderentry::trdmatchid trdmatchid;

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
static_assert(offsetof(executionreport, account) == 0, "unexpected offset of executionreport::account");
static_assert(offsetof(executionreport, avgpx) == 256, "unexpected offset of executionreport::avgpx");
static_assert(offsetof(executionreport, clordid) == 264, "unexpected offset of executionreport::clordid");
static_assert(offsetof(executionreport, cumqty) == 520, "unexpected offset of executionreport::cumqty");
static_assert(offsetof(executionreport, execid) == 528, "unexpected offset of executionreport::execid");
static_assert(offsetof(executionreport, exectranstype) == 784, "unexpected offset of executionreport::exectranstype");
static_assert(offsetof(executionreport, lastpx) == 785, "unexpected offset of executionreport::lastpx");
static_assert(offsetof(executionreport, lastshares) == 793, "unexpected offset of executionreport::lastshares");
static_assert(offsetof(executionreport, orderid) == 801, "unexpected offset of executionreport::orderid");
static_assert(offsetof(executionreport, orderqty) == 1057, "unexpected offset of executionreport::orderqty");
static_assert(offsetof(executionreport, ordstatus) == 1065, "unexpected offset of executionreport::ordstatus");
static_assert(offsetof(executionreport, ordtype) == 1066, "unexpected offset of executionreport::ordtype");
static_assert(offsetof(executionreport, origclordid) == 1067, "unexpected offset of executionreport::origclordid");
static_assert(offsetof(executionreport, price) == 1323, "unexpected offset of executionreport::price");
static_assert(offsetof(executionreport, rule80a) == 1331, "unexpected offset of executionreport::rule80a");
static_assert(offsetof(executionreport, side) == 1332, "unexpected offset of executionreport::side");
static_assert(offsetof(executionreport, symbol) == 1333, "unexpected offset of executionreport::symbol");
static_assert(offsetof(executionreport, text) == 1589, "unexpected offset of executionreport::text");
static_assert(offsetof(executionreport, timeinforce) == 1845, "unexpected offset of executionreport::timeinforce");
static_assert(offsetof(executionreport, transacttime) == 1846, "unexpected offset of executionreport::transacttime");
static_assert(offsetof(executionreport, ordrejreason) == 1854, "unexpected offset of executionreport::ordrejreason");
static_assert(offsetof(executionreport, exectype) == 1858, "unexpected offset of executionreport::exectype");
static_assert(offsetof(executionreport, leavesqty) == 1859, "unexpected offset of executionreport::leavesqty");
static_assert(offsetof(executionreport, execrestatementreason) == 1867, "unexpected offset of executionreport::execrestatementreason");
static_assert(offsetof(executionreport, nocontrabrokers) == 1871, "unexpected offset of executionreport::nocontrabrokers");
static_assert(offsetof(executionreport, trdmatchid) == 1875, "unexpected offset of executionreport::trdmatchid");
static_assert(sizeof(executionreport) == 2131, "unexpected sizeof executionreport");

#pragma pack(pop)
}
