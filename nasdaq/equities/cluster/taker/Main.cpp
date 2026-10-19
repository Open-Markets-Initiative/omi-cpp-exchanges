// Generated Nasdaq Stock Market trader Taker — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a link of its own rather than order entry.

#include "traders/Orders.hpp"
#include "traders/Taker.hpp"
#include "run/SocketLink.hpp"
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
        sim::RuntimeConfig cfg = sim::load_config(argc, argv);

        // The links, attached the way their names say.
        sim::MessageSocket to_taker("gateway -> taker", sim::MessageSocket::End::Reads, cfg.to_taker_link.host, cfg.to_taker_link.port);
        sim::MessageSocket from_taker("taker -> gateway", sim::MessageSocket::End::Writes, cfg.from_taker_link.host, cfg.from_taker_link.port);

        auto taker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_taker, b, n); };
        sim::traders::Trader<decltype(taker_send)> taker("Taker", "BOB001", "bob0000001", taker_send);
        sim::traders::SimpleTaker<std::remove_reference_t<decltype(taker.orders())>> taker_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
        int taker_ticks = 0;
        taker.login();

        std::printf("nasdaq-taker: trading as %s\n", taker.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_taker.read([&](std::string_view bytes) { taker.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_taker.deliver();
            if (taker.logged_in()) {
                taker_strategy.step(taker.orders());
                ++taker_ticks;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        if (to_taker.gaps() > 0) std::printf("nasdaq-taker: %llu gap(s) on gateway -> taker — messages were lost\n", (unsigned long long)to_taker.gaps());
        std::printf("nasdaq-taker: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)taker.entered(), (unsigned long long)taker.canceled(), taker_ticks);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-taker: %s\n", problem.what());
        return 1;
    }

    return 0;
}
