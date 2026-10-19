#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Orderid.hpp"
#include "../types/Ordstatus.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Cxlrejreason.hpp"
#include "../types/Cxlrejresponseto.hpp"
#include "../types/Account.hpp"
#include "../types/Text.hpp"
#include "../types/Symbolsfx.hpp"

namespace iex::iexequities::orderentry::fix::v2_91 {

namespace fix_orderentry = ::iex::iexequities::orderentry::fix::v2_91;

#pragma pack(push, 1)

struct ordercancelreject {

    fix_orderentry::clordid clordid;
    fix_orderentry::orderid orderid;
    fix_orderentry::ordstatus ordstatus;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::cxlrejreason cxlrejreason;
    fix_orderentry::cxlrejresponseto cxlrejresponseto;
    fix_orderentry::account account;
    fix_orderentry::text text;
    fix_orderentry::symbolsfx symbolsfx;

    // parse method
    static ordercancelreject* parse(std::byte* buffer) {
        return reinterpret_cast<ordercancelreject*>(buffer);
    }

    // parse method const
    static const ordercancelreject* parse(const std::byte* buffer) {
        return reinterpret_cast<const ordercancelreject*>(buffer);
    }
};

// layout verification
static_assert(offsetof(ordercancelreject, clordid) == 0, "unexpected offset of ordercancelreject::clordid");
static_assert(offsetof(ordercancelreject, orderid) == 256, "unexpected offset of ordercancelreject::orderid");
static_assert(offsetof(ordercancelreject, ordstatus) == 512, "unexpected offset of ordercancelreject::ordstatus");
static_assert(offsetof(ordercancelreject, origclordid) == 513, "unexpected offset of ordercancelreject::origclordid");
static_assert(offsetof(ordercancelreject, cxlrejreason) == 769, "unexpected offset of ordercancelreject::cxlrejreason");
static_assert(offsetof(ordercancelreject, cxlrejresponseto) == 773, "unexpected offset of ordercancelreject::cxlrejresponseto");
static_assert(offsetof(ordercancelreject, account) == 774, "unexpected offset of ordercancelreject::account");
static_assert(offsetof(ordercancelreject, text) == 1030, "unexpected offset of ordercancelreject::text");
static_assert(offsetof(ordercancelreject, symbolsfx) == 1286, "unexpected offset of ordercancelreject::symbolsfx");
static_assert(sizeof(ordercancelreject) == 1542, "unexpected sizeof ordercancelreject");

#pragma pack(pop)
}
