// Generated Odx Equities Open trading day — one component, one process.
// 
// It asks the engine for each change of phase on the clock, and what the auction collecting
// would cross at, over a link of its own.

#include "day/Day.hpp"
#include "run/SocketLink.hpp"
#include "SimulatorConfiguration.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
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
        // The day's times, and where its link goes when that is a socket.
        sim::RuntimeConfig cfg = sim::load_config(argc, argv);

        // The link, attached the way its name says.
        sim::IpcSocket from_day("day -> sequencer", sim::IpcSocket::End::Writes, cfg.from_day_link.host, cfg.from_day_link.port);

        auto emit = [&](const char* b, std::size_t n) { sim::write_blocking(from_day, b, n); };
        sim::components::Day<decltype(emit)> day(emit);
        day.serve(cfg.day_at);
        std::printf("odx-day: serving the day, %zu change(s)\n", day.schedule.size());

        // A pass with nothing due is a pass the day had nothing to ask; it waits a moment rather than spin.
        while (running.load(std::memory_order_acquire)) {
            const bool worked = day.step();
            from_day.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::printf("odx-day: stopped — asked %zu of %zu change(s)\n", day.asked(), day.schedule.size());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-day: %s\n", problem.what());
        return 1;
    }

    return 0;
}
