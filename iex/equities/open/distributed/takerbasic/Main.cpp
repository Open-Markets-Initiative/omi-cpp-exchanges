// Generated Investors Exchange Equities Open trader TakerBasic — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a link of its own rather than order entry.

#include "traders/Orders.hpp"
#include "traders/TakerBasic.hpp"
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
        sim::MessageSocket to_takerbasic("gatewaytakerbasic -> takerbasic", sim::MessageSocket::End::Reads, cfg.to_takerbasic_link.host, cfg.to_takerbasic_link.port);
        sim::MessageSocket from_takerbasic("takerbasic -> gatewaytakerbasic", sim::MessageSocket::End::Writes, cfg.from_takerbasic_link.host, cfg.from_takerbasic_link.port);

        auto takerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_takerbasic, b, n); };
        sim::traders::Trader<decltype(takerbasic_send)> takerbasic("TakerBasic", "bob", "bob", takerbasic_send);
        sim::traders::TakerBasic<std::remove_reference_t<decltype(takerbasic.orders())>> takerbasic_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
        int takerbasic_ticks = 0;
        std::chrono::steady_clock::time_point takerbasic_paced{};   // when a served trader next orders
        takerbasic.login();

        std::printf("iex-takerbasic: trading as %s\n", takerbasic.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        auto paced = std::chrono::steady_clock::now();   // when it next orders
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_takerbasic.read([&](std::string_view bytes) { takerbasic.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_takerbasic.deliver();
            takerbasic.tick();   // its session's heartbeats, and the venue gone silent
            if (takerbasic.logged_in() && std::chrono::steady_clock::now() >= paced) {
                takerbasic_strategy.step(takerbasic.orders());
                ++takerbasic_ticks;
                paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        if (to_takerbasic.gaps() > 0) std::printf("iex-takerbasic: %llu gap(s) on gatewaytakerbasic -> takerbasic — messages were lost\n", (unsigned long long)to_takerbasic.gaps());
        std::printf("iex-takerbasic: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)takerbasic.entered(), (unsigned long long)takerbasic.canceled(), takerbasic_ticks);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "iex-takerbasic: %s\n", problem.what());
        return 1;
    }

    return 0;
}
