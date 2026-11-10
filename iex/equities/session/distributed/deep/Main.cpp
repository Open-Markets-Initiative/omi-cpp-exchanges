// Generated Investors Exchange Equities Session Deep feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are sockets, dialled where the configuration says each component is.

#include "protocols/marketdata/deep/PacketPublisher.hpp"
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
        sim::IpcSocket to_deep("engine -> deep", sim::IpcSocket::End::Reads, cfg.to_deep_link.host, cfg.to_deep_link.port);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog deep_rec("recordings/marketdata/Deep.dat");
        net::UdpMulticastPublisher deep_mc(cfg.deep_feed.host, cfg.deep_feed.port);
        sim::deep::PacketPublisher deep("", [&](const char* s, std::size_t n) { deep_rec(s, n); deep_mc(s, n); });

        // Market data: each feed's events, translated by the transforms declared for it (IexEquities.Ipc.Transforms.xml).
        sim::IpcEventServer deep_events(
            [&](const sim::ipc_ns::trade_message& t) {
                // trade_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                auto deep_trade = sim::to_deep_trade(t);
                deep.publish_message(reinterpret_cast<const char*>(&deep_trade), sizeof deep_trade);
                deep.flush();
            },
            [&](const sim::ipc_ns::order_rested_message& o) {
                // order_rested_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                if (static_cast<char>(o.side.get().value()) == 'B') {
                    auto deep_price_level_buy = sim::to_deep_price_level_buy(o);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_price_level_buy), sizeof deep_price_level_buy);
                }
                if (static_cast<char>(o.side.get().value()) == 'S') {
                    auto deep_price_level_sell = sim::to_deep_price_level_sell(o);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_price_level_sell), sizeof deep_price_level_sell);
                }
                deep.flush();
            },
            [](const sim::ipc_ns::order_canceled_message&) {},
            [](const sim::ipc_ns::order_refused_message&) {},
            [&](const sim::ipc_ns::phase_announced_message& s) {
                // phase_announced_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                if ((static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'S' || static_cast<char>(s.announcement.get().value()) == 'Q' || static_cast<char>(s.announcement.get().value()) == 'M' || static_cast<char>(s.announcement.get().value()) == 'E' || static_cast<char>(s.announcement.get().value()) == 'C')) {
                    auto deep_system_event = sim::to_deep_system_event(s);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_system_event), sizeof deep_system_event);
                }
                if ((static_cast<char>(s.announcement.get().value()) == 'H' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                    auto deep_trading_status = sim::to_deep_trading_status(s);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_trading_status), sizeof deep_trading_status);
                }
                deep.flush();
            },
            [&](const sim::ipc_ns::cross_completed_message& q) {
                // cross_completed_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                if (static_cast<char>(q.cross.get().value()) == 'O') {
                    auto deep_official_opening_price = sim::to_deep_official_opening_price(q);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_official_opening_price), sizeof deep_official_opening_price);
                }
                if (static_cast<char>(q.cross.get().value()) == 'C') {
                    auto deep_official_closing_price = sim::to_deep_official_closing_price(q);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_official_closing_price), sizeof deep_official_closing_price);
                }
                if (static_cast<char>(q.cross.get().value()) == 'O') {
                    auto deep_opening_process_complete = sim::to_deep_opening_process_complete(q);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_opening_process_complete), sizeof deep_opening_process_complete);
                }
                if (static_cast<char>(q.cross.get().value()) == 'C') {
                    auto deep_closing_process_complete = sim::to_deep_closing_process_complete(q);
                    deep.publish_message(reinterpret_cast<const char*>(&deep_closing_process_complete), sizeof deep_closing_process_complete);
                }
                deep.flush();
            },
            [&](const sim::ipc_ns::imbalance_indicated_message& i) {
                // imbalance_indicated_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                auto deep_auction_information = sim::to_deep_auction_information(i);
                deep.publish_message(reinterpret_cast<const char*>(&deep_auction_information), sizeof deep_auction_information);
                deep.flush();
            });

        std::printf("iex-deep: publishing to %s:%u\n", cfg.deep_feed.host.c_str(), (unsigned)cfg.deep_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_deep.read([&](std::string_view message) { deep_events.on_message(message); ++handled; }) || worked;
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_deep.gaps() > 0) std::printf("iex-deep: %llu gap(s) on engine -> deep — messages were lost\n", (unsigned long long)to_deep.gaps());
        std::printf("iex-deep: stopped — %llu events read, %llu messages published\n",
                    (unsigned long long)handled, (unsigned long long)(deep.next_seq() - 1));
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "iex-deep: %s\n", problem.what());
        return 1;
    }

    return 0;
}
