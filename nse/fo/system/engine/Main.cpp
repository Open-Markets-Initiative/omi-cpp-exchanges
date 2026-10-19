// Generated Nse Futures and Options matching engine — one component, one process.
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
        sim::IpcRing to_engine("gateway -> engine", sim::IpcRing::End::Reads);
        sim::IpcRing to_clients("engine -> gateway", sim::IpcRing::End::Writes);
        sim::IpcRing to_mtbt("engine -> mtbt", sim::IpcRing::End::Writes);

        std::atomic<std::uint64_t> trades{0}, events{0};

        // Every event goes to each consumer, as it does in the single binary.
        auto emit = [&](const char* bytes, std::size_t length) {
            events.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_clients, bytes, length);
            sim::write_blocking(to_mtbt, bytes, length);
        };

        sim::components::Engine<decltype(emit)> engine(emit, trades);

        std::printf("nse-engine: matching, 1 in, 2 out\n");

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = engine.step(to_engine) || worked;
            to_clients.deliver();
            to_mtbt.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("nse-engine: stopped — %llu trades, %llu events\n",
                    (unsigned long long)trades.load(), (unsigned long long)events.load());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nse-engine: %s\n", problem.what());
        return 1;
    }

    return 0;
}
