// Generated Nasdaq Stock Market Day matching engine — one component, one process.
// 
// The book and the orders it matches are the same ones the single binary runs; only
// the links differ, and those are rings in shared memory that the venue's launcher created.

#include "engine/Engine.hpp"
#include "run/SharedRing.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <thread>

namespace {
// Interrupted by the launcher stopping the venue; the handler only asks.
std::atomic<bool> running{true};
void on_interrupt(int) { running.store(false, std::memory_order_release); }
}

int main() {
    std::signal(SIGINT, on_interrupt);
    std::signal(SIGTERM, on_interrupt);

    try {
        // The links, attached the way their names say.
        sim::IpcRing sequenced("sequencer -> engine", sim::IpcRing::End::Reads);
        sim::IpcRing to_clients_gatewaymakerbasic("engine -> gatewaymakerbasic", sim::IpcRing::End::Writes);
        sim::IpcRing to_clients_gatewaytakerbasic("engine -> gatewaytakerbasic", sim::IpcRing::End::Writes);
        sim::IpcRing to_clients_gatewayreplay("engine -> gatewayreplay", sim::IpcRing::End::Writes);
        sim::IpcRing to_clients_gatewayrequests("engine -> gatewayrequests", sim::IpcRing::End::Writes);
        sim::IpcRing to_totalview("engine -> totalview", sim::IpcRing::End::Writes);

        std::atomic<std::uint64_t> trades{0}, events{0};

        // Every event goes to each consumer, as it does in the single binary.
        auto emit = [&](const char* bytes, std::size_t length) {
            events.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_clients_gatewaymakerbasic, bytes, length);
            sim::write_blocking(to_clients_gatewaytakerbasic, bytes, length);
            sim::write_blocking(to_clients_gatewayreplay, bytes, length);
            sim::write_blocking(to_clients_gatewayrequests, bytes, length);
            sim::write_blocking(to_totalview, bytes, length);
        };

        sim::components::Engine<decltype(emit)> engine(emit, trades);

        std::printf("nasdaq-engine: matching, 1 in, 5 out\n");

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = engine.step(sequenced) || worked;
            to_clients_gatewaymakerbasic.deliver();
            to_clients_gatewaytakerbasic.deliver();
            to_clients_gatewayreplay.deliver();
            to_clients_gatewayrequests.deliver();
            to_totalview.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("nasdaq-engine: stopped — %llu trades, %llu events\n",
                    (unsigned long long)trades.load(), (unsigned long long)events.load());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-engine: %s\n", problem.what());
        return 1;
    }

    return 0;
}
