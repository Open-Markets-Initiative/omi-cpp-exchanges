// Generated Nasdaq Stock Market Session trader FixTaker — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a ring instead of a socket.

#include "traders/fix/Orders.hpp"
#include "traders/TakerBasic.hpp"
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
        sim::MessageRing to_fixtaker("gatewayfixtaker -> fixtaker", sim::MessageRing::End::Reads);
        sim::MessageRing from_fixtaker("fixtaker -> gatewayfixtaker", sim::MessageRing::End::Writes);

        auto fixtaker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_fixtaker, b, n); };
        sim::traders::fix::Trader<decltype(fixtaker_send)> fixtaker("FixTaker", "FIXTKR", "fixtkr0000", fixtaker_send);
        fixtaker.orders().trade(sim::instruments[1]);
        sim::traders::TakerBasic<std::remove_reference_t<decltype(fixtaker.orders())>> fixtaker_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/9);
        int fixtaker_ticks = 0;
        std::chrono::steady_clock::time_point fixtaker_paced{};   // when a served trader next orders
        fixtaker.login();

        std::printf("nasdaq-fixtaker: trading as %s\n", fixtaker.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        auto paced = std::chrono::steady_clock::now();   // when it next orders
        sim::reading();   // launch.sh starts the next component once this one reads its links
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_fixtaker.read([&](std::string_view bytes) { fixtaker.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_fixtaker.deliver();
            fixtaker.tick();   // its session's heartbeats, and the venue gone silent
            if (fixtaker.logged_in() && std::chrono::steady_clock::now() >= paced) {
                fixtaker_strategy.step(fixtaker.orders());
                ++fixtaker_ticks;
                paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        std::printf("nasdaq-fixtaker: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)fixtaker.entered(), (unsigned long long)fixtaker.canceled(), fixtaker_ticks);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-fixtaker: %s\n", problem.what());
        return 1;
    }

    return 0;
}
