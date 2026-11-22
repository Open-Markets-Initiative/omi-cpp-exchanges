// Generated Nasdaq Stock Market Session matching engine — one component, one process.
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
        sim::IpcSocket to_clients_gatewayrequests("engine -> gatewayrequests", sim::IpcSocket::End::Writes, cfg.to_clients_gatewayrequests_link.host, cfg.to_clients_gatewayrequests_link.port);
        sim::IpcSocket to_clients_gatewaymakersecond("engine -> gatewaymakersecond", sim::IpcSocket::End::Writes, cfg.to_clients_gatewaymakersecond_link.host, cfg.to_clients_gatewaymakersecond_link.port);
        sim::IpcSocket to_clients_gatewaytakersecond("engine -> gatewaytakersecond", sim::IpcSocket::End::Writes, cfg.to_clients_gatewaytakersecond_link.host, cfg.to_clients_gatewaytakersecond_link.port);
        sim::IpcSocket to_clients_gatewayfixmaker("engine -> gatewayfixmaker", sim::IpcSocket::End::Writes, cfg.to_clients_gatewayfixmaker_link.host, cfg.to_clients_gatewayfixmaker_link.port);
        sim::IpcSocket to_clients_gatewayfixtaker("engine -> gatewayfixtaker", sim::IpcSocket::End::Writes, cfg.to_clients_gatewayfixtaker_link.host, cfg.to_clients_gatewayfixtaker_link.port);
        sim::IpcSocket to_totalview("engine -> totalview", sim::IpcSocket::End::Writes, cfg.to_totalview_link.host, cfg.to_totalview_link.port);

        std::atomic<std::uint64_t> trades{0}, events{0};

        // Every event goes to each consumer, as it does in the single binary.
        auto emit = [&](const char* bytes, std::size_t length) {
            events.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_clients_gatewaymakerbasic, bytes, length);
            sim::write_blocking(to_clients_gatewaytakerbasic, bytes, length);
            sim::write_blocking(to_clients_gatewayreplay, bytes, length);
            sim::write_blocking(to_clients_gatewayrequests, bytes, length);
            sim::write_blocking(to_clients_gatewaymakersecond, bytes, length);
            sim::write_blocking(to_clients_gatewaytakersecond, bytes, length);
            sim::write_blocking(to_clients_gatewayfixmaker, bytes, length);
            sim::write_blocking(to_clients_gatewayfixtaker, bytes, length);
            sim::write_blocking(to_totalview, bytes, length);
        };

        sim::components::Engine<decltype(emit), sim::instruments> engine(emit, trades);
        engine.dump(cfg.book_dump);   // its own book, also written out where the settings name a file

        std::printf("nasdaq-engine: matching, 1 in, 9 out\n");

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = engine.step(sequenced) || worked;
            to_clients_gatewaymakerbasic.deliver();
            to_clients_gatewaytakerbasic.deliver();
            to_clients_gatewayreplay.deliver();
            to_clients_gatewayrequests.deliver();
            to_clients_gatewaymakersecond.deliver();
            to_clients_gatewaytakersecond.deliver();
            to_clients_gatewayfixmaker.deliver();
            to_clients_gatewayfixtaker.deliver();
            to_totalview.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (sequenced.gaps() > 0) std::printf("nasdaq-engine: %llu gap(s) on sequencer -> engine — messages were lost\n", (unsigned long long)sequenced.gaps());
        std::printf("nasdaq-engine: stopped — %llu trades, %llu events\n",
                    (unsigned long long)trades.load(), (unsigned long long)events.load());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-engine: %s\n", problem.what());
        return 1;
    }

    return 0;
}
