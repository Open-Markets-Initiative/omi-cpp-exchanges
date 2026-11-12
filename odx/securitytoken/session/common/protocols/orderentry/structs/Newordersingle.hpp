#pragma once

#include <cstddef>
#include "../types/Account.hpp"
#include "../types/Clordid.hpp"
#include "../types/Handlinst.hpp"
#include "../types/Idsource.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Ordtype.hpp"
#include "../types/Price.hpp"
#include "../types/Rule80a.hpp"
#include "../types/Securityid.hpp"
#include "../types/Side.hpp"
#include "../types/Symbol.hpp"
#include "../types/Timeinforce.hpp"
#include "../types/Transacttime.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct newordersingle {

    fix_orderentry::account account;
    fix_orderentry::clordid clordid;
    fix_orderentry::handlinst handlinst;
    fix_orderentry::idsource idsource;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::ordtype ordtype;
    fix_orderentry::price price;
    fix_orderentry::rule80a rule80a;
    fix_orderentry::securityid securityid;
    fix_orderentry::side side;
    fix_orderentry::symbol symbol;
    fix_orderentry::timeinforce timeinforce;
    fix_orderentry::transacttime transacttime;

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
static_assert(offsetof(newordersingle, account) == 0, "unexpected offset of newordersingle::account");
static_assert(offsetof(newordersingle, clordid) == 256, "unexpected offset of newordersingle::clordid");
static_assert(offsetof(newordersingle, handlinst) == 512, "unexpected offset of newordersingle::handlinst");
static_assert(offsetof(newordersingle, idsource) == 513, "unexpected offset of newordersingle::idsource");
static_assert(offsetof(newordersingle, orderqty) == 514, "unexpected offset of newordersingle::orderqty");
static_assert(offsetof(newordersingle, ordtype) == 522, "unexpected offset of newordersingle::ordtype");
static_assert(offsetof(newordersingle, price) == 523, "unexpected offset of newordersingle::price");
static_assert(offsetof(newordersingle, rule80a) == 531, "unexpected offset of newordersingle::rule80a");
static_assert(offsetof(newordersingle, securityid) == 532, "unexpected offset of newordersingle::securityid");
static_assert(offsetof(newordersingle, side) == 788, "unexpected offset of newordersingle::side");
static_assert(offsetof(newordersingle, symbol) == 789, "unexpected offset of newordersingle::symbol");
static_assert(offsetof(newordersingle, timeinforce) == 1045, "unexpected offset of newordersingle::timeinforce");
static_assert(offsetof(newordersingle, transacttime) == 1046, "unexpected offset of newordersingle::transacttime");
static_assert(sizeof(newordersingle) == 1054, "unexpected sizeof newordersingle");

#pragma pack(pop)
}
