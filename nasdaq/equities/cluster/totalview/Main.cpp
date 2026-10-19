// Generated Nasdaq Stock Market TotalView feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are sockets, dialled where the configuration says each component is.

#include "protocols/marketdata/totalview/PacketPublisher.hpp"
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
        sim::IpcSocket to_totalview("engine -> totalview", sim::IpcSocket::End::Reads, cfg.to_totalview_link.host, cfg.to_totalview_link.port);
        sim::MessageSocket to_glimpse("totalview -> glimpse", sim::MessageSocket::End::Writes, cfg.to_glimpse_link.host, cfg.to_glimpse_link.port);
        sim::MessageSocket to_totalviewtcp("totalview -> totalviewtcp", sim::MessageSocket::End::Writes, cfg.to_totalviewtcp_link.host, cfg.to_totalviewtcp_link.port);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog totalview_rec("recordings/marketdata/TotalView.dat");
        net::UdpMulticastPublisher totalview_mc(cfg.totalview_feed.host, cfg.totalview_feed.port);
        sim::totalview::PacketPublisher totalview("NSMTV00001", [&](const char* s, std::size_t n) { totalview_rec(s, n); totalview_mc(s, n); });

        // Market data: each feed's events, translated by the transforms declared for it (NsmEquities.Ipc.Transforms.xml).
        sim::IpcEventServer totalview_events(
            [&](const sim::ipc_ns::trade_message& t) {
                // trade_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
                auto totalview_order_executed = sim::to_totalview_order_executed(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                totalview.flush();
            },
            [&](const sim::ipc_ns::order_rested_message& o) {
                // order_rested_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
                auto totalview_add_order = sim::to_totalview_add_order(o);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_add_order), sizeof totalview_add_order);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_add_order), sizeof totalview_add_order);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_add_order), sizeof totalview_add_order);
                totalview.flush();
            },
            [&](const sim::ipc_ns::order_canceled_message& c) {
                // order_canceled_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
                auto totalview_order_delete = sim::to_totalview_order_delete(c);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_delete), sizeof totalview_order_delete);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_delete), sizeof totalview_order_delete);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_delete), sizeof totalview_order_delete);
                totalview.flush();
            });

        std::printf("nasdaq-totalview: publishing to %s:%u\n", cfg.totalview_feed.host.c_str(), (unsigned)cfg.totalview_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_totalview.read([&](std::string_view message) { totalview_events.on_message(message); ++handled; }) || worked;
            to_glimpse.deliver();
            to_totalviewtcp.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_totalview.gaps() > 0) std::printf("nasdaq-totalview: %llu gap(s) on engine -> totalview — messages were lost\n", (unsigned long long)to_totalview.gaps());
        std::printf("nasdaq-totalview: stopped — %llu events published\n", (unsigned long long)handled);
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-totalview: %s\n", problem.what());
        return 1;
    }

    return 0;
}
