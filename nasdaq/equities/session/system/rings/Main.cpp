// Generated Nasdaq Stock Market Session links — created before the components start, removed after they stop.
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
        std::fputs("usage: nasdaq_rings create | discard\n", stderr);
        return 2;
    }

    try {
        if (create) sim::IpcRing::create("gatewaymakerbasic -> sequencer");
        else        sim::IpcRing::discard("gatewaymakerbasic -> sequencer");
        if (create) sim::IpcRing::create("gatewaytakerbasic -> sequencer");
        else        sim::IpcRing::discard("gatewaytakerbasic -> sequencer");
        if (create) sim::IpcRing::create("gatewayreplay -> sequencer");
        else        sim::IpcRing::discard("gatewayreplay -> sequencer");
        if (create) sim::IpcRing::create("gatewayrequests -> sequencer");
        else        sim::IpcRing::discard("gatewayrequests -> sequencer");
        if (create) sim::IpcRing::create("day -> sequencer");
        else        sim::IpcRing::discard("day -> sequencer");
        if (create) sim::IpcRing::create("sequencer -> engine");
        else        sim::IpcRing::discard("sequencer -> engine");
        if (create) sim::IpcRing::create("engine -> gatewaymakerbasic");
        else        sim::IpcRing::discard("engine -> gatewaymakerbasic");
        if (create) sim::IpcRing::create("engine -> gatewaytakerbasic");
        else        sim::IpcRing::discard("engine -> gatewaytakerbasic");
        if (create) sim::IpcRing::create("engine -> gatewayreplay");
        else        sim::IpcRing::discard("engine -> gatewayreplay");
        if (create) sim::IpcRing::create("engine -> gatewayrequests");
        else        sim::IpcRing::discard("engine -> gatewayrequests");
        if (create) sim::IpcRing::create("engine -> totalview");
        else        sim::IpcRing::discard("engine -> totalview");
        if (create) sim::MessageRing::create("totalview -> glimpse");
        else        sim::MessageRing::discard("totalview -> glimpse");
        if (create) sim::MessageRing::create("totalview -> totalviewtcp");
        else        sim::MessageRing::discard("totalview -> totalviewtcp");
        if (create) sim::MessageRing::create("gatewaymakerbasic -> makerbasic");
        else        sim::MessageRing::discard("gatewaymakerbasic -> makerbasic");
        if (create) sim::MessageRing::create("makerbasic -> gatewaymakerbasic");
        else        sim::MessageRing::discard("makerbasic -> gatewaymakerbasic");
        if (create) sim::MessageRing::create("gatewaytakerbasic -> takerbasic");
        else        sim::MessageRing::discard("gatewaytakerbasic -> takerbasic");
        if (create) sim::MessageRing::create("takerbasic -> gatewaytakerbasic");
        else        sim::MessageRing::discard("takerbasic -> gatewaytakerbasic");
        if (create) sim::MessageRing::create("gatewayreplay -> replay");
        else        sim::MessageRing::discard("gatewayreplay -> replay");
        if (create) sim::MessageRing::create("replay -> gatewayreplay");
        else        sim::MessageRing::discard("replay -> gatewayreplay");
        if (create) sim::MessageRing::create("gatewayrequests -> requests");
        else        sim::MessageRing::discard("gatewayrequests -> requests");
        if (create) sim::MessageRing::create("requests -> gatewayrequests");
        else        sim::MessageRing::discard("requests -> gatewayrequests");
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-rings: %s\n", problem.what());
        return 1;
    }

    std::printf("nasdaq-rings: %s 21 links\n", create ? "created" : "discarded");
    return 0;
}
