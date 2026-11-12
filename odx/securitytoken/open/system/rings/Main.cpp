// Generated Odx Security Token Open links — created before the components start, removed after they stop.
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
        std::fputs("usage: odx_rings create | discard\n", stderr);
        return 2;
    }

    try {
        if (create) sim::IpcRing::create("gatewaymakerbasic -> sequencer");
        else        sim::IpcRing::discard("gatewaymakerbasic -> sequencer");
        if (create) sim::IpcRing::create("gatewaytakerbasic -> sequencer");
        else        sim::IpcRing::discard("gatewaytakerbasic -> sequencer");
        if (create) sim::IpcRing::create("day -> sequencer");
        else        sim::IpcRing::discard("day -> sequencer");
        if (create) sim::IpcRing::create("sequencer -> engine");
        else        sim::IpcRing::discard("sequencer -> engine");
        if (create) sim::IpcRing::create("engine -> gatewaymakerbasic");
        else        sim::IpcRing::discard("engine -> gatewaymakerbasic");
        if (create) sim::IpcRing::create("engine -> gatewaytakerbasic");
        else        sim::IpcRing::discard("engine -> gatewaytakerbasic");
        if (create) sim::IpcRing::create("engine -> itch");
        else        sim::IpcRing::discard("engine -> itch");
        if (create) sim::MessageRing::create("itch -> itchtcp");
        else        sim::MessageRing::discard("itch -> itchtcp");
        if (create) sim::MessageRing::create("gatewaymakerbasic -> makerbasic");
        else        sim::MessageRing::discard("gatewaymakerbasic -> makerbasic");
        if (create) sim::MessageRing::create("makerbasic -> gatewaymakerbasic");
        else        sim::MessageRing::discard("makerbasic -> gatewaymakerbasic");
        if (create) sim::MessageRing::create("gatewaytakerbasic -> takerbasic");
        else        sim::MessageRing::discard("gatewaytakerbasic -> takerbasic");
        if (create) sim::MessageRing::create("takerbasic -> gatewaytakerbasic");
        else        sim::MessageRing::discard("takerbasic -> gatewaytakerbasic");
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-rings: %s\n", problem.what());
        return 1;
    }

    std::printf("odx-rings: %s 12 links\n", create ? "created" : "discarded");
    return 0;
}
