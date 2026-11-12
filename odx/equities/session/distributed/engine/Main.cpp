// Generated Odx Equities Session matching engine — one component, one process.
// 
// The book and the orders it matches are the same ones the single binary runs; only
// the links differ, and those are sockets, dialled where the configuration says each component is.

#include "engine/Engine.hpp"
#include "run/SocketLink.hpp"
#include "SimulatorConfiguration.hpp"

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

int main(int argc, char** argv) {
    std::signal(SIGINT, on_interrupt);
    std::signal(SIGTERM, on_interrupt);

    try {
        sim::RuntimeConfig cfg = sim::load_config(argc, argv);

        // The links, attached the way their names say.
        sim::IpcSocket sequenced("sequencer -> engine", sim::IpcSocket::End::Reads, cfg.sequenced_link.host, cfg.sequenced_link.port);
        sim::IpcSocket to_clients_gatewaymakerbasic("engine -> gatewaymakerbasic", sim::IpcSocket::End::Writes, cfg.to_clients_gatewaymakerbasic_link.host, cfg.to_clients_gatewaymakerbasic_link.port);
        sim::IpcSocket to_clients_gatewaytakerbasic("engine -> gatewaytakerbasic", sim::IpcSocket::End::Writes, cfg.to_clients_gatewaytakerbasic_link.host, cfg.to_clients_gatewaytakerbasic_link.port);
        sim::IpcSocket to_clients_gatewayreplay("engine -> gatewayreplay", sim::IpcSocket::End::Writes, cfg.to_clients_gatewayreplay_link.host, cfg.to_clients_gatewayreplay_link.port);
        sim::IpcSocket to_itch("engine -> itch", sim::IpcSocket::End::Writes, cfg.to_itch_link.host, cfg.to_itch_link.port);

        std::atomic<std::uint64_t> trades{0}, events{0};

        // Every event goes to each consumer, as it does in the single binary.
        auto emit = [&](const char* bytes, std::size_t length) {
            events.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_clients_gatewaymakerbasic, bytes, length);
            sim::write_blocking(to_clients_gatewaytakerbasic, bytes, length);
            sim::write_blocking(to_clients_gatewayreplay, bytes, length);
            sim::write_blocking(to_itch, bytes, length);
        };

        sim::components::Engine<decltype(emit)> engine(emit, trades);

        std::printf("odx-engine: matching, 1 in, 4 out\n");

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = engine.step(sequenced) || worked;
            to_clients_gatewaymakerbasic.deliver();
            to_clients_gatewaytakerbasic.deliver();
            to_clients_gatewayreplay.deliver();
            to_itch.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (sequenced.gaps() > 0) std::printf("odx-engine: %llu gap(s) on sequencer -> engine — messages were lost\n", (unsigned long long)sequenced.gaps());
        std::printf("odx-engine: stopped — %llu trades, %llu events\n",
                    (unsigned long long)trades.load(), (unsigned long long)events.load());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-engine: %s\n", problem.what());
        return 1;
    }

    return 0;
}
