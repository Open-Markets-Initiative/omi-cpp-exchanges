// Generated Nasdaq Stock Market Day TotalView feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are rings in shared memory that the venue's launcher created.

#include "protocols/marketdata/totalview/PacketPublisher.hpp"
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
        sim::IpcRing to_totalview("engine -> totalview", sim::IpcRing::End::Reads);
        sim::MessageRing to_glimpse("totalview -> glimpse", sim::MessageRing::End::Writes);
        sim::MessageRing to_totalviewtcp("totalview -> totalviewtcp", sim::MessageRing::End::Writes);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog totalview_rec("recordings/marketdata/TotalView.dat");
        net::UdpMulticastPublisher totalview_mc(cfg.totalview_feed.host, cfg.totalview_feed.port);
        sim::totalview::PacketPublisher totalview("NSMTV00001", [&](const char* s, std::size_t n) { totalview_rec(s, n); totalview_mc(s, n); });

        // Market data: each feed's events, translated by the transforms declared for it (NsmEquities.Ipc.Transforms.xml).
        sim::IpcEventServer totalview_events(
            [&](const sim::ipc_ns::trade_message& t) {
                // trade_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
                if (static_cast<char>(t.auction.get().value()) == 'N') {
                    auto totalview_order_executed = sim::to_totalview_order_executed(t);
                    totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                    sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                }
                if (static_cast<char>(t.auction.get().value()) != 'N') {
                    auto totalview_crossed_resting = sim::to_totalview_crossed_resting(t);
                    totalview.publish_message(reinterpret_cast<const char*>(&totalview_crossed_resting), sizeof totalview_crossed_resting);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_crossed_resting), sizeof totalview_crossed_resting);
                    sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_crossed_resting), sizeof totalview_crossed_resting);
                }
                if (static_cast<char>(t.auction.get().value()) != 'N') {
                    auto totalview_crossed_aggressor = sim::to_totalview_crossed_aggressor(t);
                    totalview.publish_message(reinterpret_cast<const char*>(&totalview_crossed_aggressor), sizeof totalview_crossed_aggressor);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_crossed_aggressor), sizeof totalview_crossed_aggressor);
                    sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_crossed_aggressor), sizeof totalview_crossed_aggressor);
                }
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
            },
            [](const sim::ipc_ns::order_accepted_message&) {},
            [](const sim::ipc_ns::order_eliminated_message&) {},
            [](const sim::ipc_ns::order_refused_message&) {},
            [&](const sim::ipc_ns::phase_announced_message& s) {
                // phase_announced_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
                if ((static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'S' || static_cast<char>(s.announcement.get().value()) == 'Q' || static_cast<char>(s.announcement.get().value()) == 'M' || static_cast<char>(s.announcement.get().value()) == 'E' || static_cast<char>(s.announcement.get().value()) == 'C')) {
                    auto totalview_system_event = sim::to_totalview_system_event(s);
                    totalview.publish_message(reinterpret_cast<const char*>(&totalview_system_event), sizeof totalview_system_event);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_system_event), sizeof totalview_system_event);
                    sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_system_event), sizeof totalview_system_event);
                }
                if ((static_cast<char>(s.announcement.get().value()) == 'H' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                    auto totalview_stock_trading_action = sim::to_totalview_stock_trading_action(s);
                    totalview.publish_message(reinterpret_cast<const char*>(&totalview_stock_trading_action), sizeof totalview_stock_trading_action);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_stock_trading_action), sizeof totalview_stock_trading_action);
                    sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_stock_trading_action), sizeof totalview_stock_trading_action);
                }
                totalview.flush();
            },
            [&](const sim::ipc_ns::cross_completed_message& q) {
                // cross_completed_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
                auto totalview_cross_trade = sim::to_totalview_cross_trade(q);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_cross_trade), sizeof totalview_cross_trade);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_cross_trade), sizeof totalview_cross_trade);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_cross_trade), sizeof totalview_cross_trade);
                totalview.flush();
            },
            [&](const sim::ipc_ns::imbalance_indicated_message& i) {
                // imbalance_indicated_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
                if (static_cast<char>(i.imbalance_direction.get().value()) != 'O') {
                    auto totalview_noii = sim::to_totalview_noii(i);
                    totalview.publish_message(reinterpret_cast<const char*>(&totalview_noii), sizeof totalview_noii);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_noii), sizeof totalview_noii);
                    sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_noii), sizeof totalview_noii);
                }
                if (static_cast<char>(i.imbalance_direction.get().value()) == 'O') {
                    auto totalview_noii_insufficient = sim::to_totalview_noii_insufficient(i);
                    totalview.publish_message(reinterpret_cast<const char*>(&totalview_noii_insufficient), sizeof totalview_noii_insufficient);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_noii_insufficient), sizeof totalview_noii_insufficient);
                    sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_noii_insufficient), sizeof totalview_noii_insufficient);
                }
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

        std::printf("nasdaq-totalview: stopped — %llu events read, %llu messages published\n",
                    (unsigned long long)handled, (unsigned long long)(totalview.next_seq() - 1));
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-totalview: %s\n", problem.what());
        return 1;
    }

    return 0;
}
