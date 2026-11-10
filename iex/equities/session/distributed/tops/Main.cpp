// Generated Investors Exchange Equities Session Tops feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are sockets, dialled where the configuration says each component is.

#include "protocols/marketdata/tops/PacketPublisher.hpp"
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
        sim::IpcSocket to_tops("engine -> tops", sim::IpcSocket::End::Reads, cfg.to_tops_link.host, cfg.to_tops_link.port);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog tops_rec("recordings/marketdata/Tops.dat");
        net::UdpMulticastPublisher tops_mc(cfg.tops_feed.host, cfg.tops_feed.port);
        sim::tops::PacketPublisher tops("", [&](const char* s, std::size_t n) { tops_rec(s, n); tops_mc(s, n); });

        // Market data: each feed's events, translated by the transforms declared for it (IexEquities.Ipc.Transforms.xml).
        sim::IpcEventServer tops_events(
            [](const sim::ipc_ns::trade_message&) {},
            [&](const sim::ipc_ns::order_rested_message& o) {
                // order_rested_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                if (static_cast<char>(o.side.get().value()) == 'B') {
                    auto tops_quote_bid = sim::to_tops_quote_bid(o);
                    tops.publish_message(reinterpret_cast<const char*>(&tops_quote_bid), sizeof tops_quote_bid);
                }
                if (static_cast<char>(o.side.get().value()) == 'S') {
                    auto tops_quote_ask = sim::to_tops_quote_ask(o);
                    tops.publish_message(reinterpret_cast<const char*>(&tops_quote_ask), sizeof tops_quote_ask);
                }
                tops.flush();
            },
            [](const sim::ipc_ns::order_canceled_message&) {},
            [](const sim::ipc_ns::order_refused_message&) {},
            [&](const sim::ipc_ns::phase_announced_message& s) {
                // phase_announced_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                if ((static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'S' || static_cast<char>(s.announcement.get().value()) == 'Q' || static_cast<char>(s.announcement.get().value()) == 'M' || static_cast<char>(s.announcement.get().value()) == 'E' || static_cast<char>(s.announcement.get().value()) == 'C')) {
                    auto tops_system_event = sim::to_tops_system_event(s);
                    tops.publish_message(reinterpret_cast<const char*>(&tops_system_event), sizeof tops_system_event);
                }
                if ((static_cast<char>(s.announcement.get().value()) == 'H' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                    auto tops_trading_status = sim::to_tops_trading_status(s);
                    tops.publish_message(reinterpret_cast<const char*>(&tops_trading_status), sizeof tops_trading_status);
                }
                tops.flush();
            },
            [&](const sim::ipc_ns::cross_completed_message& q) {
                // cross_completed_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                if (static_cast<char>(q.cross.get().value()) == 'O') {
                    auto tops_official_opening_price = sim::to_tops_official_opening_price(q);
                    tops.publish_message(reinterpret_cast<const char*>(&tops_official_opening_price), sizeof tops_official_opening_price);
                }
                if (static_cast<char>(q.cross.get().value()) == 'C') {
                    auto tops_official_closing_price = sim::to_tops_official_closing_price(q);
                    tops.publish_message(reinterpret_cast<const char*>(&tops_official_closing_price), sizeof tops_official_closing_price);
                }
                tops.flush();
            },
            [&](const sim::ipc_ns::imbalance_indicated_message& i) {
                // imbalance_indicated_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
                auto tops_auction_information = sim::to_tops_auction_information(i);
                tops.publish_message(reinterpret_cast<const char*>(&tops_auction_information), sizeof tops_auction_information);
                tops.flush();
            });

        std::printf("iex-tops: publishing to %s:%u\n", cfg.tops_feed.host.c_str(), (unsigned)cfg.tops_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_tops.read([&](std::string_view message) { tops_events.on_message(message); ++handled; }) || worked;
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_tops.gaps() > 0) std::printf("iex-tops: %llu gap(s) on engine -> tops — messages were lost\n", (unsigned long long)to_tops.gaps());
        std::printf("iex-tops: stopped — %llu events read, %llu messages published\n",
                    (unsigned long long)handled, (unsigned long long)(tops.next_seq() - 1));
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "iex-tops: %s\n", problem.what());
        return 1;
    }

    return 0;
}
