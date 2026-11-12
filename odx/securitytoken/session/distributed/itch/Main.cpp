// Generated Odx Security Token Session Itch feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are sockets, dialled where the configuration says each component is.

#include "protocols/marketdata/itch/PacketPublisher.hpp"
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
        sim::IpcSocket to_itch("engine -> itch", sim::IpcSocket::End::Reads, cfg.to_itch_link.host, cfg.to_itch_link.port);
        sim::MessageSocket to_itchtcp("itch -> itchtcp", sim::MessageSocket::End::Writes, cfg.to_itchtcp_link.host, cfg.to_itchtcp_link.port);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog itch_rec("recordings/marketdata/Itch.dat");
        sim::itch::PacketPublisher itch("ODXSTDAY01", [&](const char* s, std::size_t n) { itch_rec(s, n); });
        sim::ItchSeconds itch_seconds;   // the second its seconds message last said

        // Market data: each feed's events, translated by the transforms declared for it (OdxSecurityToken.Ipc.Transforms.xml).
        sim::IpcEventServer itch_events(
            [&](const sim::ipc_ns::trade_message& t) {
                // trade_message -> the feeds its transforms declare (OdxSecurityToken.Ipc.Transforms.xml).
                if (static_cast<char>(t.auction.get().value()) == 'N') {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(t, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_order_executed_with_price = sim::to_itch_order_executed_with_price(t);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_order_executed_with_price), sizeof itch_order_executed_with_price);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_executed_with_price), sizeof itch_order_executed_with_price);
                }
                if (static_cast<char>(t.auction.get().value()) != 'N') {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(t, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_crossed_sell = sim::to_itch_crossed_sell(t);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_crossed_sell), sizeof itch_crossed_sell);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_crossed_sell), sizeof itch_crossed_sell);
                }
                if (static_cast<char>(t.auction.get().value()) != 'N') {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(t, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_crossed_buy = sim::to_itch_crossed_buy(t);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_crossed_buy), sizeof itch_crossed_buy);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_crossed_buy), sizeof itch_crossed_buy);
                }
                itch.flush();
            },
            [&](const sim::ipc_ns::order_rested_message& o) {
                // order_rested_message -> the feeds its transforms declare (OdxSecurityToken.Ipc.Transforms.xml).
                if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(o, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_order_added = sim::to_itch_order_added(o);
                itch.publish_message(reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
                itch.flush();
            },
            [&](const sim::ipc_ns::order_canceled_message& c) {
                // order_canceled_message -> the feeds its transforms declare (OdxSecurityToken.Ipc.Transforms.xml).
                if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(c, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_order_deleted = sim::to_itch_order_deleted(c);
                itch.publish_message(reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
                itch.flush();
            },
            [](const sim::ipc_ns::order_accepted_message&) {},
            [&](const sim::ipc_ns::order_eliminated_message& e) {
                // order_eliminated_message -> the feeds its transforms declare (OdxSecurityToken.Ipc.Transforms.xml).
                if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(e, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_order_expired = sim::to_itch_order_expired(e);
                itch.publish_message(reinterpret_cast<const char*>(&itch_order_expired), sizeof itch_order_expired);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_expired), sizeof itch_order_expired);
                itch.flush();
            },
            [](const sim::ipc_ns::order_refused_message&) {},
            [&](const sim::ipc_ns::phase_announced_message& s) {
                // phase_announced_message -> the feeds its transforms declare (OdxSecurityToken.Ipc.Transforms.xml).
                if ((static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'C')) {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_system_event = sim::to_itch_system_event(s);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
                }
                if (static_cast<char>(s.announcement.get().value()) == 'O') {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    // A message for each tick of the table the instrument is listed under.
                    for (const auto& tick : sim::ticks_of_ticktable(s.orderbook.get_trimmed().value())) {
                        auto itch_price_tick_size = sim::to_itch_price_tick_size(s, tick);
                        itch.publish_message(reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                    }
                }
                if (static_cast<char>(s.announcement.get().value()) == 'O') {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_orderbook_directory = sim::to_itch_orderbook_directory(s);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
                }
                if ((static_cast<char>(s.announcement.get().value()) == 'V' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_trading_state = sim::to_itch_trading_state(s);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
                }
                itch.flush();
            },
            [&](const sim::ipc_ns::cross_completed_message& q) {
                // cross_completed_message -> the feeds its transforms declare (OdxSecurityToken.Ipc.Transforms.xml).
                if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(q, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_equilibrium_crossed = sim::to_itch_equilibrium_crossed(q);
                itch.publish_message(reinterpret_cast<const char*>(&itch_equilibrium_crossed), sizeof itch_equilibrium_crossed);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_equilibrium_crossed), sizeof itch_equilibrium_crossed);
                itch.flush();
            },
            [&](const sim::ipc_ns::imbalance_indicated_message& i) {
                // imbalance_indicated_message -> the feeds its transforms declare (OdxSecurityToken.Ipc.Transforms.xml).
                if (static_cast<char>(i.imbalance_direction.get().value()) != 'O') {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(i, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_equilibrium_price = sim::to_itch_equilibrium_price(i);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_equilibrium_price), sizeof itch_equilibrium_price);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_equilibrium_price), sizeof itch_equilibrium_price);
                }
                if (static_cast<char>(i.imbalance_direction.get().value()) == 'O') {
                    if (odx::odxsecuritytoken::pts::itch::v1_2::seconds_message itch_second{}; itch_seconds.due(i, itch_second)) {
                        itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                        sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    }
                    auto itch_equilibrium_none = sim::to_itch_equilibrium_none(i);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_equilibrium_none), sizeof itch_equilibrium_none);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_equilibrium_none), sizeof itch_equilibrium_none);
                }
                itch.flush();
            });

        std::printf("odx-itch: publishing to %s:%u\n", cfg.itch_feed.host.c_str(), (unsigned)cfg.itch_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_itch.read([&](std::string_view message) { itch_events.on_message(message); ++handled; }) || worked;
            to_itchtcp.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_itch.gaps() > 0) std::printf("odx-itch: %llu gap(s) on engine -> itch — messages were lost\n", (unsigned long long)to_itch.gaps());
        std::printf("odx-itch: stopped — %llu events read, %llu messages published\n",
                    (unsigned long long)handled, (unsigned long long)(itch.next_seq() - 1));
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-itch: %s\n", problem.what());
        return 1;
    }

    return 0;
}
