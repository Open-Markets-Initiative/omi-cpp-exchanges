// Generated Odx Equities Open sequencer — one component, one process.
// 
// It puts what is bound for its engine in one order, as the single binary's does; only
// the links differ, and those are rings in shared memory that the venue's launcher created.

#include "sequencer/Sequencer.hpp"
#include "run/SharedRing.hpp"

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

int main() {
    std::signal(SIGINT, on_interrupt);
    std::signal(SIGTERM, on_interrupt);

    try {
        // The links, attached the way their names say.
        sim::IpcRing to_sequencer_gatewaymakerbasic("gatewaymakerbasic -> sequencer", sim::IpcRing::End::Reads);
        sim::IpcRing to_sequencer_gatewaytakerbasic("gatewaytakerbasic -> sequencer", sim::IpcRing::End::Reads);
        sim::IpcRing to_sequencer_gatewayreplay("gatewayreplay -> sequencer", sim::IpcRing::End::Reads);
        sim::IpcRing from_day("day -> sequencer", sim::IpcRing::End::Reads);
        sim::IpcRing sequenced("sequencer -> engine", sim::IpcRing::End::Writes);

        // What it puts in order goes to its engine.
        auto emit = [&](const char* bytes, std::size_t length) {
            sim::write_blocking(sequenced, bytes, length);
        };

        sim::components::Sequencer<decltype(emit)> sequencer(emit);

        std::printf("odx-sequencer: sequencing, 4 in, 1 out\n");

        // A message off each producer in turn; nothing to do means back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            const bool worked = sequencer.step(to_sequencer_gatewaymakerbasic, to_sequencer_gatewaytakerbasic, to_sequencer_gatewayreplay, from_day);
            sequenced.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("odx-sequencer: stopped — %llu sequenced\n", (unsigned long long)sequencer.sequenced());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-sequencer: %s\n", problem.what());
        return 1;
    }

    return 0;
}
