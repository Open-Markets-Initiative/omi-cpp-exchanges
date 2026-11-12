#pragma once

#include <cstddef>
#include "../types/Tradingsessionid.hpp"
#include "../types/Tradsesmode.hpp"
#include "../types/Tradsesstatus.hpp"

namespace odx::odxsecuritytoken::orderentry::fix::v1_1 {

namespace fix_orderentry = ::odx::odxsecuritytoken::orderentry::fix::v1_1;

#pragma pack(push, 1)

struct tradingsessionstatus {

    fix_orderentry::tradingsessionid tradingsessionid;
    fix_orderentry::tradsesmode tradsesmode;
    fix_orderentry::tradsesstatus tradsesstatus;

    // parse method
    static tradingsessionstatus* parse(std::byte* buffer) {
        return reinterpret_cast<tradingsessionstatus*>(buffer);
    }

    // parse method const
    static const tradingsessionstatus* parse(const std::byte* buffer) {
        return reinterpret_cast<const tradingsessionstatus*>(buffer);
    }
};

// layout verification
static_assert(offsetof(tradingsessionstatus, tradingsessionid) == 0, "unexpected offset of tradingsessionstatus::tradingsessionid");
static_assert(offsetof(tradingsessionstatus, tradsesmode) == 256, "unexpected offset of tradingsessionstatus::tradsesmode");
static_assert(offsetof(tradingsessionstatus, tradsesstatus) == 260, "unexpected offset of tradingsessionstatus::tradsesstatus");
static_assert(sizeof(tradingsessionstatus) == 264, "unexpected sizeof tradingsessionstatus");

#pragma pack(pop)
}
