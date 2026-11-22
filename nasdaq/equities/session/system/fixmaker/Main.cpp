// Generated Nasdaq Stock Market Session trader FixMaker — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a ring instead of a socket.

#include "traders/fix/Orders.hpp"
#include "traders/MakerBasic.hpp"
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
        sim::MessageRing to_fixmaker("gatewayfixmaker -> fixmaker", sim::MessageRing::End::Reads);
        sim::MessageRing from_fixmaker("fixmaker -> gatewayfixmaker", sim::MessageRing::End::Writes);

        auto fixmaker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_fixmaker, b, n); };
        sim::traders::fix::Trader<decltype(fixmaker_send)> fixmaker("FixMaker", "FIXMKR", "fixmkr0000", fixmaker_send);
        fixmaker.orders().trade(sim::instruments[1]);
        sim::traders::MakerBasic<std::remove_reference_t<decltype(fixmaker.orders())>> fixmaker_strategy(/*mid*/10000, /*spread*/40, /*size*/100, /*levels*/3, /*tick*/1);
        int fixmaker_ticks = 0;
        std::chrono::steady_clock::time_point fixmaker_paced{};   // when a served trader next orders
        fixmaker.login();

        std::printf("nasdaq-fixmaker: trading as %s\n", fixmaker.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        auto paced = std::chrono::steady_clock::now();   // when it next orders
        sim::reading();   // launch.sh starts the next component once this one reads its links
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_fixmaker.read([&](std::string_view bytes) { fixmaker.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_fixmaker.deliver();
            fixmaker.tick();   // its session's heartbeats, and the venue gone silent
            if (fixmaker.logged_in() && std::chrono::steady_clock::now() >= paced) {
                fixmaker_strategy.step(fixmaker.orders());
                ++fixmaker_ticks;
                paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        std::printf("nasdaq-fixmaker: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)fixmaker.entered(), (unsigned long long)fixmaker.canceled(), fixmaker_ticks);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-fixmaker: %s\n", problem.what());
        return 1;
    }

    return 0;
}
