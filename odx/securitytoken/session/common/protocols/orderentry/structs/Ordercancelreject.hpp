#pragma once

#include <cstddef>
#include "../types/Clordid.hpp"
#include "../types/Orderid.hpp"
#include "../types/Ordstatus.hpp"
#include "../types/Origclordid.hpp"
#include "../types/Text.hpp"
#include "../types/Cxlrejreason.hpp"
#include "../types/Cxlrejresponseto.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct ordercancelreject {

    fix_orderentry::clordid clordid;
    fix_orderentry::orderid orderid;
    fix_orderentry::ordstatus ordstatus;
    fix_orderentry::origclordid origclordid;
    fix_orderentry::text text;
    fix_orderentry::cxlrejreason cxlrejreason;
    fix_orderentry::cxlrejresponseto cxlrejresponseto;

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
static_assert(offsetof(ordercancelreject, text) == 769, "unexpected offset of ordercancelreject::text");
static_assert(offsetof(ordercancelreject, cxlrejreason) == 1025, "unexpected offset of ordercancelreject::cxlrejreason");
static_assert(offsetof(ordercancelreject, cxlrejresponseto) == 1029, "unexpected offset of ordercancelreject::cxlrejresponseto");
static_assert(sizeof(ordercancelreject) == 1030, "unexpected sizeof ordercancelreject");

#pragma pack(pop)
}
