// Generated Investors Exchange Equities Open DeepPlus feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are rings in shared memory that the venue's launcher created.

#include "protocols/marketdata/deepplus/PacketPublisher.hpp"
#include "protocols/ipc/IpcBus.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
#include "run/SharedRing.hpp"
#include "MarketDataLog.hpp"
#include "net/Network.hpp"
#include "SimulatorConfiguration.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <string_view>
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
        sim::IpcRing to_deepplus("engine -> deepplus", sim::IpcRing::End::Reads);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog deepplus_rec("recordings/marketdata/DeepPlus.dat");
        net::UdpMulticastPublisher deepplus_mc(cfg.deepplus_feed.host, cfg.deepplus_feed.port);
        sim::deepplus::PacketPublisher deepplus("", [&](const char* s, std::size_t n) { deepplus_rec(s, n); deepplus_mc(s, n); });

        // Market data: each feed's events, translated by the transforms declared for it (IexEquities.Ipc.Transforms.xml).
        sim::IpcEventServer deepplus_events(
            [&](const sim::ipc_ns::trade_message& t) {
                // trade_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                auto deepplus_trade = sim::to_deepplus_trade(t);
                deepplus.publish_message(reinterpret_cast<const char*>(&deepplus_trade), sizeof deepplus_trade);
                deepplus.flush();
            },
            [&](const sim::ipc_ns::order_rested_message& o) {
                // order_rested_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                auto deepplus_add_order = sim::to_deepplus_add_order(o);
                deepplus.publish_message(reinterpret_cast<const char*>(&deepplus_add_order), sizeof deepplus_add_order);
                deepplus.flush();
            },
            [](const sim::ipc_ns::order_canceled_message&) {},
            [](const sim::ipc_ns::order_refused_message&) {});

        std::printf("iex-deepplus: publishing to %s:%u\n", cfg.deepplus_feed.host.c_str(), (unsigned)cfg.deepplus_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_deepplus.read([&](std::string_view message) { deepplus_events.on_message(message); ++handled; }) || worked;
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("iex-deepplus: stopped — %llu events read, %llu messages published\n",
                    (unsigned long long)handled, (unsigned long long)(deepplus.next_seq() - 1));
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "iex-deepplus: %s\n", problem.what());
        return 1;
    }

    return 0;
}
