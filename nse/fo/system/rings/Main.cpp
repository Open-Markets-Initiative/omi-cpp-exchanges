// Generated Nse Futures and Options links — created before the components start, removed after they stop.
// 
// The names are the ones the single binary calls its queues by, so both versions of the
// venue talk about the same link in the same words.

#include "run/SharedRing.hpp"

#include <cstdio>
#include <cstring>
#include <exception>

int main(int argc, char** argv) {
    const char* asked = argc > 1 ? argv[1] : "";
    const bool create  = std::strcmp(asked, "create")  == 0;
    const bool discard = std::strcmp(asked, "discard") == 0;

    if (!create && !discard) {
        std::fputs("usage: nse_rings create | discard\n", stderr);
        return 2;
    }

    try {
        if (create) sim::IpcRing::create("gateway -> engine");
        else        sim::IpcRing::discard("gateway -> engine");
        if (create) sim::IpcRing::create("engine -> gateway");
        else        sim::IpcRing::discard("engine -> gateway");
        if (create) sim::IpcRing::create("engine -> mtbt");
        else        sim::IpcRing::discard("engine -> mtbt");
        if (create) sim::MessageRing::create("gateway -> maker");
        else        sim::MessageRing::discard("gateway -> maker");
        if (create) sim::MessageRing::create("maker -> gateway");
        else        sim::MessageRing::discard("maker -> gateway");
        if (create) sim::MessageRing::create("gateway -> taker");
        else        sim::MessageRing::discard("gateway -> taker");
        if (create) sim::MessageRing::create("taker -> gateway");
        else        sim::MessageRing::discard("taker -> gateway");
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nse-rings: %s\n", problem.what());
        return 1;
    }

    std::printf("nse-rings: %s 7 links\n", create ? "created" : "discarded");
    return 0;
}
