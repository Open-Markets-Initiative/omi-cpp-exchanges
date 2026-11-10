// Generated Investors Exchange Equities Session sequencer — one component, one process.
// 
// It puts what is bound for its engine in one order, as the single binary's does; only
// the links differ, and those are sockets, dialled where the configuration says each component is.

#include "sequencer/Sequencer.hpp"
#include "run/SocketLink.hpp"
#include "SimulatorConfiguration.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
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
        sim::RuntimeConfig cfg = sim::load_config(argc, argv);

        // The links, attached the way their names say.
        sim::IpcSocket to_sequencer_gatewaymakerbasic("gatewaymakerbasic -> sequencer", sim::IpcSocket::End::Reads, cfg.to_sequencer_gatewaymakerbasic_link.host, cfg.to_sequencer_gatewaymakerbasic_link.port);
        sim::IpcSocket to_sequencer_gatewaytakerbasic("gatewaytakerbasic -> sequencer", sim::IpcSocket::End::Reads, cfg.to_sequencer_gatewaytakerbasic_link.host, cfg.to_sequencer_gatewaytakerbasic_link.port);
        sim::IpcSocket from_day("day -> sequencer", sim::IpcSocket::End::Reads, cfg.from_day_link.host, cfg.from_day_link.port);
        sim::IpcSocket sequenced("sequencer -> engine", sim::IpcSocket::End::Writes, cfg.sequenced_link.host, cfg.sequenced_link.port);

        // What it puts in order goes to its engine.
        auto emit = [&](const char* bytes, std::size_t length) {
            sim::write_blocking(sequenced, bytes, length);
        };

        sim::components::Sequencer<decltype(emit)> sequencer(emit);

        std::printf("iex-sequencer: sequencing, 3 in, 1 out\n");

        // A message off each producer in turn; nothing to do means back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            const bool worked = sequencer.step(to_sequencer_gatewaymakerbasic, to_sequencer_gatewaytakerbasic, from_day);
            sequenced.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_sequencer_gatewaymakerbasic.gaps() > 0) std::printf("iex-sequencer: %llu gap(s) on gatewaymakerbasic -> sequencer — messages were lost\n", (unsigned long long)to_sequencer_gatewaymakerbasic.gaps());
        if (to_sequencer_gatewaytakerbasic.gaps() > 0) std::printf("iex-sequencer: %llu gap(s) on gatewaytakerbasic -> sequencer — messages were lost\n", (unsigned long long)to_sequencer_gatewaytakerbasic.gaps());
        if (from_day.gaps() > 0) std::printf("iex-sequencer: %llu gap(s) on day -> sequencer — messages were lost\n", (unsigned long long)from_day.gaps());
        std::printf("iex-sequencer: stopped — %llu sequenced\n", (unsigned long long)sequencer.sequenced());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "iex-sequencer: %s\n", problem.what());
        return 1;
    }

    return 0;
}
