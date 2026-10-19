// Generated Nasdaq Stock Market matching engine — one component, one process.
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
        sim::IpcSocket to_engine("gateway -> engine", sim::IpcSocket::End::Reads, cfg.to_engine_link.host, cfg.to_engine_link.port);
        sim::IpcSocket to_clients("engine -> gateway", sim::IpcSocket::End::Writes, cfg.to_clients_link.host, cfg.to_clients_link.port);
        sim::IpcSocket to_totalview("engine -> totalview", sim::IpcSocket::End::Writes, cfg.to_totalview_link.host, cfg.to_totalview_link.port);

        std::atomic<std::uint64_t> trades{0}, events{0};

        // Every event goes to each consumer, as it does in the single binary.
        auto emit = [&](const char* bytes, std::size_t length) {
            events.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_clients, bytes, length);
            sim::write_blocking(to_totalview, bytes, length);
        };

        sim::components::Engine<decltype(emit)> engine(emit, trades);

        std::printf("nasdaq-engine: matching, 1 in, 2 out\n");

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = engine.step(to_engine) || worked;
            to_clients.deliver();
            to_totalview.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_engine.gaps() > 0) std::printf("nasdaq-engine: %llu gap(s) on gateway -> engine — messages were lost\n", (unsigned long long)to_engine.gaps());
        std::printf("nasdaq-engine: stopped — %llu trades, %llu events\n",
                    (unsigned long long)trades.load(), (unsigned long long)events.load());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-engine: %s\n", problem.what());
        return 1;
    }

    return 0;
}
