// Generated Odx Equities Session trader MakerBasic — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a ring instead of a socket.

#include "traders/Orders.hpp"
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
        sim::MessageRing to_makerbasic("gatewaymakerbasic -> makerbasic", sim::MessageRing::End::Reads);
        sim::MessageRing from_makerbasic("makerbasic -> gatewaymakerbasic", sim::MessageRing::End::Writes);

        auto makerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_makerbasic, b, n); };
        sim::traders::Trader<decltype(makerbasic_send)> makerbasic("MakerBasic", "ALICE1", "alice00001", makerbasic_send);
        sim::traders::MakerBasic<std::remove_reference_t<decltype(makerbasic.orders())>> makerbasic_strategy(/*mid*/10000, /*spread*/20, /*size*/100, /*levels*/3, /*tick*/10);
        int makerbasic_ticks = 0;
        std::chrono::steady_clock::time_point makerbasic_paced{};   // when a served trader next orders
        makerbasic.login();

        std::printf("odx-makerbasic: trading as %s\n", makerbasic.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        auto paced = std::chrono::steady_clock::now();   // when it next orders
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_makerbasic.read([&](std::string_view bytes) { makerbasic.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_makerbasic.deliver();
            makerbasic.tick();   // its session's heartbeats, and the venue gone silent
            if (makerbasic.logged_in() && std::chrono::steady_clock::now() >= paced) {
                makerbasic_strategy.step(makerbasic.orders());
                ++makerbasic_ticks;
                paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        std::printf("odx-makerbasic: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)makerbasic.entered(), (unsigned long long)makerbasic.canceled(), makerbasic_ticks);
        if (makerbasic.silences() > 0)
            std::printf("odx-makerbasic: the venue went silent on it %llu time(s); %s\n",
                        (unsigned long long)makerbasic.silences(), makerbasic.logged_in() ? "logged in again" : "not logged in again");
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-makerbasic: %s\n", problem.what());
        return 1;
    }

    return 0;
}
