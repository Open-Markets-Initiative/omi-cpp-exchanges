// Generated Nasdaq Stock Market Session trader TakerSecond — one component, one process.
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
        sim::MessageSocket to_takersecond("gatewaytakersecond -> takersecond", sim::MessageSocket::End::Reads, cfg.to_takersecond_link.host, cfg.to_takersecond_link.port);
        sim::MessageSocket from_takersecond("takersecond -> gatewaytakersecond", sim::MessageSocket::End::Writes, cfg.from_takersecond_link.host, cfg.from_takersecond_link.port);

        auto takersecond_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_takersecond, b, n); };
        sim::traders::Trader<decltype(takersecond_send)> takersecond("TakerSecond", "TAKER2", "taker20000", takersecond_send);
        takersecond.orders().trade(sim::instruments[1]);
        sim::traders::TakerBasic<std::remove_reference_t<decltype(takersecond.orders())>> takersecond_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
        int takersecond_ticks = 0;
        std::chrono::steady_clock::time_point takersecond_paced{};   // when a served trader next orders
        takersecond.login();

        std::printf("nasdaq-takersecond: trading as %s\n", takersecond.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        auto paced = std::chrono::steady_clock::now();   // when it next orders
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_takersecond.read([&](std::string_view bytes) { takersecond.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_takersecond.deliver();
            takersecond.tick();   // its session's heartbeats, and the venue gone silent
            if (takersecond.logged_in() && std::chrono::steady_clock::now() >= paced) {
                takersecond_strategy.step(takersecond.orders());
                ++takersecond_ticks;
                paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        if (to_takersecond.gaps() > 0) std::printf("nasdaq-takersecond: %llu gap(s) on gatewaytakersecond -> takersecond — messages were lost\n", (unsigned long long)to_takersecond.gaps());
        std::printf("nasdaq-takersecond: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)takersecond.entered(), (unsigned long long)takersecond.canceled(), takersecond_ticks);
        if (takersecond.silences() > 0)
            std::printf("nasdaq-takersecond: the venue went silent on it %llu time(s); %s\n",
                        (unsigned long long)takersecond.silences(), takersecond.logged_in() ? "logged in again" : "not logged in again");
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-takersecond: %s\n", problem.what());
        return 1;
    }

    return 0;
}
