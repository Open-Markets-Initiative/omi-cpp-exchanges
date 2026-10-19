// Generated Nasdaq Stock Market trader Replay — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a ring instead of a socket.

#include "traders/Orders.hpp"
#include "traders/CaptureQuotes.hpp"
#include "run/SharedRing.hpp"
#include "SimulatorConfiguration.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <string_view>
#include <thread>
#include <type_traits>

namespace {
// Interrupted by the launcher stopping the venue; the handler only asks.
std::atomic<bool> running{true};
void on_interrupt(int) { running.store(false, std::memory_order_release); }
}

int main(int argc, char** argv) {
    std::signal(SIGINT, on_interrupt);
    std::signal(SIGTERM, on_interrupt);

    try {
        (void)argc; (void)argv;

        // The links, attached the way their names say.
        sim::MessageRing to_replay("gateway -> replay", sim::MessageRing::End::Reads);
        sim::MessageRing from_replay("replay -> gateway", sim::MessageRing::End::Writes);

        auto replay_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_replay, b, n); };
        sim::traders::Trader<decltype(replay_send)> replay("Replay", "CAROL1", "carol00001", replay_send);
        // Replay takes its orders from a capture of Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026: what that market showed,
        // posted here as this trader's own orders. A capture it cannot read leaves it quiet.
        sim::traders::Capture replay_capture("captures/TotalView.pcap");
        if (!replay_capture.ok())
            std::printf("nasdaq-simulator: trader Replay has no orders — captures/TotalView.pcap %s\n", replay_capture.error().c_str());
        sim::traders::CaptureQuotes replay_read(replay_capture);
        sim::traders::CaptureMaker<std::remove_reference_t<decltype(replay.orders())>> replay_strategy(replay_read.quotes(), /*quotes*/4);
        int replay_ticks = 0;
        replay.login();

        std::printf("nasdaq-replay: trading as %s\n", replay.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_replay.read([&](std::string_view bytes) { replay.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_replay.deliver();
            if (replay.logged_in()) {
                replay_strategy.step(replay.orders());
                ++replay_ticks;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        std::printf("nasdaq-replay: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)replay.entered(), (unsigned long long)replay.canceled(), replay_ticks);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-replay: %s\n", problem.what());
        return 1;
    }

    return 0;
}
