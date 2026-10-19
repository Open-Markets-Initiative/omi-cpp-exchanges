// Generated Nse Futures and Options Mtbt feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are sockets, dialled where the configuration says each component is.

#include "protocols/marketdata/mtbt/PacketPublisher.hpp"
#include "protocols/ipc/IpcBus.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
#include "run/SocketLink.hpp"
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
        sim::IpcSocket to_mtbt("engine -> mtbt", sim::IpcSocket::End::Reads, cfg.to_mtbt_link.host, cfg.to_mtbt_link.port);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog mtbt_rec("recordings/marketdata/Mtbt.dat");
        net::UdpMulticastPublisher mtbt_mc(cfg.mtbt_feed.host, cfg.mtbt_feed.port);
        sim::mtbt::PacketPublisher mtbt("1", [&](const char* s, std::size_t n) { mtbt_rec(s, n); mtbt_mc(s, n); });

        // Market data: each feed's events, translated by the transforms declared for it (NseFo.Ipc.Transforms.xml).
        sim::IpcEventServer mtbt_events(
            [&](const sim::ipc_ns::trade_message& t) {
                // trade_message -> the feeds its transforms declare (NseFo.Ipc.Transforms.xml).
                auto mtbt_trade = sim::to_mtbt_trade(t);
                mtbt.publish_message(reinterpret_cast<const char*>(&mtbt_trade), sizeof mtbt_trade);
                mtbt.flush();
            },
            [&](const sim::ipc_ns::order_rested_message& o) {
                // order_rested_message -> the feeds its transforms declare (NseFo.Ipc.Transforms.xml).
                auto mtbt_new_order = sim::to_mtbt_new_order(o);
                mtbt.publish_message(reinterpret_cast<const char*>(&mtbt_new_order), sizeof mtbt_new_order);
                mtbt.flush();
            },
            [&](const sim::ipc_ns::order_canceled_message& c) {
                // order_canceled_message -> the feeds its transforms declare (NseFo.Ipc.Transforms.xml).
                auto mtbt_order_cancellation = sim::to_mtbt_order_cancellation(c);
                mtbt.publish_message(reinterpret_cast<const char*>(&mtbt_order_cancellation), sizeof mtbt_order_cancellation);
                mtbt.flush();
            });

        std::printf("nse-mtbt: publishing to %s:%u\n", cfg.mtbt_feed.host.c_str(), (unsigned)cfg.mtbt_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_mtbt.read([&](std::string_view message) { mtbt_events.on_message(message); ++handled; }) || worked;
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_mtbt.gaps() > 0) std::printf("nse-mtbt: %llu gap(s) on engine -> mtbt — messages were lost\n", (unsigned long long)to_mtbt.gaps());
        std::printf("nse-mtbt: stopped — %llu events published\n", (unsigned long long)handled);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nse-mtbt: %s\n", problem.what());
        return 1;
    }

    return 0;
}
