#pragma once

#include <cstddef>
#include "../types/Orderid.hpp"
#include "../types/Clordid.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Ordstatus.hpp"
#include "../types/Clientid.hpp"
#include "../types/Cxlrejreason.hpp"
#include "../types/Cxlrejresponseto.hpp"
#include "../types/Text.hpp"

namespace nasdaq::nsmequities::orderentry::fix::v4_2 {

namespace fix_orderentry = ::nasdaq::nsmequities::orderentry::fix::v4_2;

#pragma pack(push, 1)

struct ordercancelreject {

    fix_orderentry::orderid orderid;
    fix_orderentry::clordid clordid;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::ordstatus ordstatus;
    fix_orderentry::clientid clientid;
    fix_orderentry::cxlrejreason cxlrejreason;
    fix_orderentry::cxlrejresponseto cxlrejresponseto;
    fix_orderentry::text text;

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
static_assert(offsetof(ordercancelreject, orderid) == 0, "unexpected offset of ordercancelreject::orderid");
static_assert(offsetof(ordercancelreject, clordid) == 256, "unexpected offset of ordercancelreject::clordid");
static_assert(offsetof(ordercancelreject, origclordid) == 512, "unexpected offset of ordercancelreject::origclordid");
static_assert(offsetof(ordercancelreject, ordstatus) == 768, "unexpected offset of ordercancelreject::ordstatus");
static_assert(offsetof(ordercancelreject, clientid) == 769, "unexpected offset of ordercancelreject::clientid");
static_assert(offsetof(ordercancelreject, cxlrejreason) == 1025, "unexpected offset of ordercancelreject::cxlrejreason");
static_assert(offsetof(ordercancelreject, cxlrejresponseto) == 1029, "unexpected offset of ordercancelreject::cxlrejresponseto");
static_assert(offsetof(ordercancelreject, text) == 1033, "unexpected offset of ordercancelreject::text");
static_assert(sizeof(ordercancelreject) == 1289, "unexpected sizeof ordercancelreject");

#pragma pack(pop)
}
