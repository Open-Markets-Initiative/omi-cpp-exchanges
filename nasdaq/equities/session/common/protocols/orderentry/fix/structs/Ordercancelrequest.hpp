#pragma once

#include <cstddef>
#include "../types/Origclordid.hpp"
#include "../types/Clordid.hpp"
#include "../types/Symbol.hpp"
#include "../types/Side.hpp"
#include "../types/Orderqty.hpp"
#include "../types/Orderid.hpp"
#include "../types/Clientid.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct ordercancelrequest {

    fix_orderentry::origclordid origclordid;
    fix_orderentry::clordid clordid;
    fix_orderentry::symbol symbol;
    fix_orderentry::side side;
    fix_orderentry::orderqty orderqty;
    fix_orderentry::orderid orderid;
    fix_orderentry::clientid clientid;

    // parse method
    static ordercancelrequest* parse(std::byte* buffer) {
        return reinterpret_cast<ordercancelrequest*>(buffer);
    }

    // parse method const
    static const ordercancelrequest* parse(const std::byte* buffer) {
        return reinterpret_cast<const ordercancelrequest*>(buffer);
    }
};

// layout verification
static_assert(offsetof(ordercancelrequest, origclordid) == 0, "unexpected offset of ordercancelrequest::origclordid");
static_assert(offsetof(ordercancelrequest, clordid) == 256, "unexpected offset of ordercancelrequest::clordid");
static_assert(offsetof(ordercancelrequest, symbol) == 512, "unexpected offset of ordercancelrequest::symbol");
static_assert(offsetof(ordercancelrequest, side) == 768, "unexpected offset of ordercancelrequest::side");
static_assert(offsetof(ordercancelrequest, orderqty) == 769, "unexpected offset of ordercancelrequest::orderqty");
static_assert(offsetof(ordercancelrequest, orderid) == 777, "unexpected offset of ordercancelrequest::orderid");
static_assert(offsetof(ordercancelrequest, clientid) == 1033, "unexpected offset of ordercancelrequest::clientid");
static_assert(sizeof(ordercancelrequest) == 1289, "unexpected sizeof ordercancelrequest");

#pragma pack(pop)
}
