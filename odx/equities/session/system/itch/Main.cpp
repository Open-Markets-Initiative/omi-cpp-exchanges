// Generated Odx Equities Session Itch feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are rings in shared memory that the venue's launcher created.

#include "protocols/marketdata/itch/PacketPublisher.hpp"
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
        sim::IpcRing to_itch("engine -> itch", sim::IpcRing::End::Reads);
        sim::MessageRing to_glimpse("itch -> glimpse", sim::MessageRing::End::Writes);
        sim::MessageRing to_itchtcp("itch -> itchtcp", sim::MessageRing::End::Writes);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog itch_rec("recordings/marketdata/Itch.dat");
        net::UdpMulticastPublisher itch_mc(cfg.itch_feed.host, cfg.itch_feed.port);
        sim::itch::PacketPublisher itch("ODXITCHS01", [&](const char* s, std::size_t n) { itch_rec(s, n); itch_mc(s, n); });
        sim::ItchSeconds itch_seconds;   // the second its seconds message last said

        // Market data: each feed's events, translated by the transforms declared for it (OdxEquities.Ipc.Transforms.xml).
        sim::IpcEventServer itch_events(
            [&](const sim::ipc_ns::trade_message& t) {
                // trade_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(t, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_order_executed = sim::to_itch_order_executed(t);
                itch.publish_message(reinterpret_cast<const char*>(&itch_order_executed), sizeof itch_order_executed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_executed), sizeof itch_order_executed);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_executed), sizeof itch_order_executed);
                itch.flush();
            },
            [&](const sim::ipc_ns::order_rested_message& o) {
                // order_rested_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(o, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_order_added = sim::to_itch_order_added(o);
                itch.publish_message(reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
                itch.flush();
            },
            [&](const sim::ipc_ns::order_canceled_message& c) {
                // order_canceled_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(c, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_order_deleted = sim::to_itch_order_deleted(c);
                itch.publish_message(reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
                itch.flush();
            },
            [](const sim::ipc_ns::order_accepted_message&) {},
            [](const sim::ipc_ns::order_eliminated_message&) {},
            [](const sim::ipc_ns::order_refused_message&) {},
            [&](const sim::ipc_ns::phase_announced_message& s) {
                // phase_announced_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
                if ((static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'S' || static_cast<char>(s.announcement.get().value()) == 'Q' || static_cast<char>(s.announcement.get().value()) == 'M' || static_cast<char>(s.announcement.get().value()) == 'E' || static_cast<char>(s.announcement.get().value()) == 'C')) {
                    if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_system_event = sim::to_itch_system_event(s);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
                }
                if (static_cast<char>(s.announcement.get().value()) == 'O') {
                    if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    // A message for each tick of the table the instrument is listed under.
                    for (const auto& tick : sim::ticks_of_ticktable(s.orderbook.get_trimmed().value())) {
                        auto itch_price_tick_size = sim::to_itch_price_tick_size(s, tick);
                        itch.publish_message(reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                        sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                    }
                }
                if (static_cast<char>(s.announcement.get().value()) == 'O') {
                    if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_orderbook_directory = sim::to_itch_orderbook_directory(s);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
                }
                if ((static_cast<char>(s.announcement.get().value()) == 'V' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                    if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_trading_state = sim::to_itch_trading_state(s);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
                }
                itch.flush();
            },
            [&](const sim::ipc_ns::order_replaced_message& w) {
                // order_replaced_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(w, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_order_replaced = sim::to_itch_order_replaced(w);
                itch.publish_message(reinterpret_cast<const char*>(&itch_order_replaced), sizeof itch_order_replaced);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_replaced), sizeof itch_order_replaced);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_replaced), sizeof itch_order_replaced);
                itch.flush();
            });

        std::printf("odx-itch: publishing to %s:%u\n", cfg.itch_feed.host.c_str(), (unsigned)cfg.itch_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_itch.read([&](std::string_view message) { itch_events.on_message(message); ++handled; }) || worked;
            to_glimpse.deliver();
            to_itchtcp.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("odx-itch: stopped — %llu events read, %llu messages published\n",
                    (unsigned long long)handled, (unsigned long long)(itch.next_seq() - 1));
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-itch: %s\n", problem.what());
        return 1;
    }

    return 0;
}
