// Generated Nse Futures and Options trader Maker — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a ring instead of a socket.

#include "traders/Orders.hpp"
#include "traders/Maker.hpp"
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
        sim::MessageRing to_maker("gateway -> maker", sim::MessageRing::End::Reads);
        sim::MessageRing from_maker("maker -> gateway", sim::MessageRing::End::Writes);

        auto maker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_maker, b, n); };
        sim::traders::Trader<decltype(maker_send)> maker("Maker", "10001", "alice001", maker_send);
        sim::traders::SimpleMaker<std::remove_reference_t<decltype(maker.orders())>> maker_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3);
        int maker_ticks = 0;
        maker.login();

        std::printf("nse-maker: trading as %s\n", maker.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_maker.read([&](std::string_view bytes) { maker.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_maker.deliver();
            if (maker.logged_in()) {
                maker_strategy.step(maker.orders());
                ++maker_ticks;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        std::printf("nse-maker: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)maker.entered(), (unsigned long long)maker.canceled(), maker_ticks);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nse-maker: %s\n", problem.what());
        return 1;
    }

    return 0;
}
