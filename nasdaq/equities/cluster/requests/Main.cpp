// Generated Nasdaq Stock Market trader Requests — one component, one process.
// 
// It speaks the venue's own order entry, as a client session like any other; its bytes
// reach the gateway over a link of its own rather than order entry.

#include "traders/Orders.hpp"
#include "traders/CaptureRequests.hpp"
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
        sim::MessageSocket to_requests("gateway -> requests", sim::MessageSocket::End::Reads, cfg.to_requests_link.host, cfg.to_requests_link.port);
        sim::MessageSocket from_requests("requests -> gateway", sim::MessageSocket::End::Writes, cfg.from_requests_link.host, cfg.from_requests_link.port);

        auto requests_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_requests, b, n); };
        sim::traders::Trader<decltype(requests_send)> requests("Requests", "DAVE01", "dave000001", requests_send);
        // Requests takes its orders from a capture of this venue's own order entry: the requests a
        // client sent, replayed here as this trader's own. A capture it cannot read leaves it quiet.
        sim::traders::Capture requests_capture("captures/Ouch.pcap");
        if (!requests_capture.ok())
            std::printf("nasdaq-simulator: trader Requests has no orders — captures/Ouch.pcap %s\n", requests_capture.error().c_str());
        sim::traders::CaptureRequests requests_read(requests_capture);
        sim::traders::CaptureReplay<std::remove_reference_t<decltype(requests.orders())>> requests_strategy(requests_read.requests(), /*quotes*/3);
        int requests_ticks = 0;
        requests.login();

        std::printf("nasdaq-requests: trading as %s\n", requests.name().c_str());

        // A step a trader does not take is a step it had nothing to do: it reads what the
        // venue said, and acts only once it is admitted.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_requests.read([&](std::string_view bytes) { requests.on_bytes(bytes.data(), bytes.size()); }) || worked;
            from_requests.deliver();
            if (requests.logged_in()) {
                requests_strategy.step(requests.orders());
                ++requests_ticks;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            } else if (!worked) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        if (to_requests.gaps() > 0) std::printf("nasdaq-requests: %llu gap(s) on gateway -> requests — messages were lost\n", (unsigned long long)to_requests.gaps());
        std::printf("nasdaq-requests: stopped — entered %llu, canceled %llu over %d turn(s)\n",
                    (unsigned long long)requests.entered(), (unsigned long long)requests.canceled(), requests_ticks);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-requests: %s\n", problem.what());
        return 1;
    }

    return 0;
}
