// Generated Investors Exchange Equities Session simulator — entry point.
// 
// One process, one thread per component; every link between them is a queue of framed
// Ipc messages, so a component hands bytes on instead of calling the next one:
// clients --(Fix)--> [gateway] --(ipc)--> [engine]
// [engine] --(ipc)--> [gateway] client responses
// [engine] --(ipc)--> [deep] Deep market data
// [engine] --(ipc)--> [tops] Tops market data
// [engine] --(ipc)--> [deepplus] DeepPlus market data

#include "protocols/orderentry/Gateway.hpp"
#include "traders/Orders.hpp"
#include "traders/MakerBasic.hpp"
#include "traders/TakerBasic.hpp"
#include "protocols/marketdata/tops/PacketPublisher.hpp"
#include "protocols/marketdata/deep/PacketPublisher.hpp"
#include "protocols/marketdata/deepplus/PacketPublisher.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
#include "snap/SnapServer.hpp"
#include "run/Components.hpp"
#include "engine/Engine.hpp"
#include "sequencer/Sequencer.hpp"
#include "day/Day.hpp"
#include "MarketDataLog.hpp"
#include "SimulatorConfiguration.hpp"
#include "net/Network.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <csignal>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>

namespace {
// --serve runs until interrupted; the handler only asks, main.cpp does the stopping.
std::atomic<bool> serving{true};
void on_interrupt(int) { serving.store(false, std::memory_order_release); }
}

int main(int argc, char** argv) {
    // Runtime endpoints: simulator.ini (or --config <file>) decides IPs/ports at process start.
    sim::RuntimeConfig cfg = sim::load_config(argc, argv);
    bool serve = false;
    for (int i = 1; i < argc; ++i) if (std::string_view(argv[i]) == "--serve") serve = true;

    std::puts("iex-simulator configuration (runtime simulator.ini):");
    std::printf("  order-entry  %s:%u  MakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaymakerbasic_port);
    std::printf("  order-entry  %s:%u  TakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaytakerbasic_port);
    std::printf("  feed Tops      %s:%u\n", cfg.tops_feed.host.c_str(), (unsigned)cfg.tops_feed.port);
    std::printf("  feed Deep      %s:%u\n", cfg.deep_feed.host.c_str(), (unsigned)cfg.deep_feed.port);
    std::printf("  feed DeepPlus  %s:%u\n", cfg.deepplus_feed.host.c_str(), (unsigned)cfg.deepplus_feed.port);
    std::printf("  snap Deep      port %u\n", (unsigned)cfg.deep_snap_port);
    std::printf("  snap Tops      port %u\n", (unsigned)cfg.tops_snap_port);
    std::printf("  snap DeepPlus  port %u\n", (unsigned)cfg.deepplus_snap_port);
    std::printf("  recording    dir=%s fix-log=%s market-data=%s\n", cfg.recording_directory.c_str(), "on", "on");

    // Market-data feed servers — segments tee to the recorder, Snap server, and real UDP multicast.
    sim::MarketDataLog tops_rec("recordings/marketdata/Tops.dat");
    snap::SnapServer tops_snap;
    net::UdpMulticastPublisher tops_mc(cfg.tops_feed.host, cfg.tops_feed.port);
    sim::tops::PacketPublisher tops("", [&](const char* s, std::size_t n) { tops_rec(s, n); tops_snap.on_segment(s, n); tops_mc(s, n); });
    sim::MarketDataLog deep_rec("recordings/marketdata/Deep.dat");
    snap::SnapServer deep_snap;
    net::UdpMulticastPublisher deep_mc(cfg.deep_feed.host, cfg.deep_feed.port);
    sim::deep::PacketPublisher deep("", [&](const char* s, std::size_t n) { deep_rec(s, n); deep_snap.on_segment(s, n); deep_mc(s, n); });
    sim::MarketDataLog deepplus_rec("recordings/marketdata/DeepPlus.dat");
    snap::SnapServer deepplus_snap;
    net::UdpMulticastPublisher deepplus_mc(cfg.deepplus_feed.host, cfg.deepplus_feed.port);
    sim::deepplus::PacketPublisher deepplus("", [&](const char* s, std::size_t n) { deepplus_rec(s, n); deepplus_snap.on_segment(s, n); deepplus_mc(s, n); });

    // The links between the components. One writer and one reader each: the engine fans
    // an event out by writing one queue per consumer, and takes one queue per producer.
    sim::IpcQueue to_sequencer_gatewaymakerbasic("gatewaymakerbasic -> sequencer");
    sim::IpcQueue to_sequencer_gatewaytakerbasic("gatewaytakerbasic -> sequencer");
    sim::IpcQueue from_day("day -> sequencer");
    sim::IpcQueue sequenced("sequencer -> engine");
    sim::IpcQueue to_clients_gatewaymakerbasic("engine -> gatewaymakerbasic");
    sim::IpcQueue to_clients_gatewaytakerbasic("engine -> gatewaytakerbasic");
    sim::IpcQueue to_deep("engine -> deep");
    sim::IpcQueue to_tops("engine -> tops");
    sim::IpcQueue to_deepplus("engine -> deepplus");

    // Counted from several threads: what crossed the links, and what the engine matched.
    std::atomic<std::uint64_t> ipc_msgs{0}, ipc_events{0}, trades{0};

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
        [](const sim::ipc_ns::order_refused_message&) {},
        [&](const sim::ipc_ns::phase_announced_message& s) {
            // phase_announced_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
            if ((static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'S' || static_cast<char>(s.announcement.get().value()) == 'Q' || static_cast<char>(s.announcement.get().value()) == 'M' || static_cast<char>(s.announcement.get().value()) == 'E' || static_cast<char>(s.announcement.get().value()) == 'C')) {
                auto deepplus_system_event = sim::to_deepplus_system_event(s);
                deepplus.publish_message(reinterpret_cast<const char*>(&deepplus_system_event), sizeof deepplus_system_event);
            }
            if ((static_cast<char>(s.announcement.get().value()) == 'H' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                auto deepplus_trading_status = sim::to_deepplus_trading_status(s);
                deepplus.publish_message(reinterpret_cast<const char*>(&deepplus_trading_status), sizeof deepplus_trading_status);
            }
            deepplus.flush();
        },
        [&](const sim::ipc_ns::cross_completed_message& q) {
            // cross_completed_message -> the feeds its transforms declare (IexEquities.Ipc.Transforms.xml).
            if (static_cast<char>(q.cross.get().value()) == 'O') {
                auto deepplus_opening_process_complete = sim::to_deepplus_opening_process_complete(q);
                deepplus.publish_message(reinterpret_cast<const char*>(&deepplus_opening_process_complete), sizeof deepplus_opening_process_complete);
            }
            if (static_cast<char>(q.cross.get().value()) == 'C') {
                auto deepplus_closing_process_complete = sim::to_deepplus_closing_process_complete(q);
                deepplus.publish_message(reinterpret_cast<const char*>(&deepplus_closing_process_complete), sizeof deepplus_closing_process_complete);
            }
            deepplus.flush();
        },
        [](const sim::ipc_ns::imbalance_indicated_message&) {});

    // MakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaymakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaymakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaymakerbasic_emit)> gatewaymakerbasic(gatewaymakerbasic_emit, 1, 2);

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaymakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaymakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakerbasic.on_event(c); },
        [](const sim::ipc_ns::order_refused_message&) {},
        [](const sim::ipc_ns::phase_announced_message&) {},
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {});

    // TakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaytakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaytakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaytakerbasic_emit)> gatewaytakerbasic(gatewaytakerbasic_emit, 2, 2);

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaytakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaytakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaytakerbasic.on_event(c); },
        [](const sim::ipc_ns::order_refused_message&) {},
        [](const sim::ipc_ns::phase_announced_message&) {},
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {});

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients_gatewaymakerbasic, b, n);
        sim::write_blocking(to_clients_gatewaytakerbasic, b, n);
        sim::write_blocking(to_deep, b, n);
        sim::write_blocking(to_tops, b, n);
        sim::write_blocking(to_deepplus, b, n);
    };
    // The engine component, built the same way the per-component binaries build it:
    // what it matches is its own, and where its events go is whatever emit it was handed.
    sim::components::Engine<decltype(event_emit)> engine_component(event_emit, trades);
    // The trading day: it asks the engine for each change of phase over the Ipc, on a link of
    // its own. A served venue asks on the clock, at the seconds its simulator.ini gives; the
    // startup run asks between its turns.
    auto day_emit = [&](const char* b, std::size_t n) { sim::write_blocking(from_day, b, n); };
    sim::components::Day<decltype(day_emit)> day_component(day_emit);
    if (serve) day_component.serve(cfg.day_at);
    // Each engine's sequencer: what is bound for the engine, from every producer, put in one
    // order, each message stamped with its place in it and the time, and handed to the engine.
    auto sequencer_emit = [&](const char* b, std::size_t n) { sim::write_blocking(sequenced, b, n); };
    sim::components::Sequencer<decltype(sequencer_emit)> sequencer_component(sequencer_emit);

    // Traders: each trades through the venue's own order entry, as a client session like any
    // other — its bytes reach the gateway over a queue instead of a socket.
    sim::MessageQueue to_makerbasic("gatewaymakerbasic -> makerbasic");
    sim::MessageQueue from_makerbasic("makerbasic -> gatewaymakerbasic");
    auto makerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_makerbasic, b, n); };
    sim::traders::Trader<decltype(makerbasic_send)> makerbasic("MakerBasic", "alice", "alice", makerbasic_send);
    auto makerbasic_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_makerbasic, b, n); };
    std::size_t makerbasic_client = gatewaymakerbasic.connect(makerbasic_reply);
    sim::traders::MakerBasic<std::remove_reference_t<decltype(makerbasic.orders())>> makerbasic_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3, /*tick*/1);
    int makerbasic_ticks = 0;
    std::chrono::steady_clock::time_point makerbasic_paced{};   // when a served trader next orders
    sim::Turn makerbasic_turn;
    makerbasic.login();
    sim::MessageQueue to_takerbasic("gatewaytakerbasic -> takerbasic");
    sim::MessageQueue from_takerbasic("takerbasic -> gatewaytakerbasic");
    auto takerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_takerbasic, b, n); };
    sim::traders::Trader<decltype(takerbasic_send)> takerbasic("TakerBasic", "bob", "bob", takerbasic_send);
    auto takerbasic_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_takerbasic, b, n); };
    std::size_t takerbasic_client = gatewaytakerbasic.connect(takerbasic_reply);
    sim::traders::TakerBasic<std::remove_reference_t<decltype(takerbasic.orders())>> takerbasic_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int takerbasic_ticks = 0;
    std::chrono::steady_clock::time_point takerbasic_paced{};   // when a served trader next orders
    sim::Turn takerbasic_turn;
    takerbasic.login();

    // Each listening socket belongs to its gateway's thread; --serve is what opens them.
    std::unique_ptr<net::TcpServer> gatewaymakerbasic_order_entry;
    std::map<int, std::size_t> gatewaymakerbasic_sessions;
    std::unique_ptr<net::TcpServer> gatewaytakerbasic_order_entry;
    std::map<int, std::size_t> gatewaytakerbasic_sessions;
    std::unique_ptr<net::TcpServer> deep_snap_srv;
    std::unique_ptr<net::TcpServer> tops_snap_srv;
    std::unique_ptr<net::TcpServer> deepplus_snap_srv;
    if (serve) {
        std::signal(SIGPIPE, SIG_IGN);   // a client hanging up must not kill the venue
        std::signal(SIGINT, on_interrupt);
        gatewaymakerbasic_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaymakerbasic_port);
        gatewaytakerbasic_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaytakerbasic_port);
        deep_snap_srv = std::make_unique<net::TcpServer>("0.0.0.0", cfg.deep_snap_port);
        tops_snap_srv = std::make_unique<net::TcpServer>("0.0.0.0", cfg.tops_snap_port);
        deepplus_snap_srv = std::make_unique<net::TcpServer>("0.0.0.0", cfg.deepplus_snap_port);
    }

    // One thread per component. Each owns its own state: the gateway its client sockets,
    // the engine its book, each feed its publisher — nothing is shared but the queues.
    sim::Component engine_thread("engine", [&] { return engine_component.step(sequenced); });
    sim::Component sequencer_thread("sequencer", [&] { return sequencer_component.step(to_sequencer_gatewaymakerbasic, to_sequencer_gatewaytakerbasic, from_day); });
    sim::Component day_thread("day", [&] { return day_component.step(); });
    sim::Component deep_thread("deep", [&] {
        bool worked = to_deep.read([&](std::string_view message) { deep_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (deep_snap_srv) worked = deep_snap_srv->poll([&](int fd, std::string_view request) {
            deep_snap.on_message(request, [&](const char* b, std::size_t n) { deep_snap_srv->send(fd, b, n); });
        }) || worked;
        return worked;
    });
    sim::Component tops_thread("tops", [&] {
        bool worked = to_tops.read([&](std::string_view message) { tops_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (tops_snap_srv) worked = tops_snap_srv->poll([&](int fd, std::string_view request) {
            tops_snap.on_message(request, [&](const char* b, std::size_t n) { tops_snap_srv->send(fd, b, n); });
        }) || worked;
        return worked;
    });
    sim::Component deepplus_thread("deepplus", [&] {
        bool worked = to_deepplus.read([&](std::string_view message) { deepplus_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (deepplus_snap_srv) worked = deepplus_snap_srv->poll([&](int fd, std::string_view request) {
            deepplus_snap.on_message(request, [&](const char* b, std::size_t n) { deepplus_snap_srv->send(fd, b, n); });
        }) || worked;
        return worked;
    });
    sim::Component gatewaymakerbasic_thread("gatewaymakerbasic", [&] {
        bool worked = to_clients_gatewaymakerbasic.read([&](std::string_view message) { gatewaymakerbasic_events.on_message(message); });
        worked = from_makerbasic.read([&](std::string_view bytes) { gatewaymakerbasic.on_bytes(makerbasic_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewaymakerbasic_order_entry) worked = gatewaymakerbasic_order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = gatewaymakerbasic_sessions.find(fd);
            if (session == gatewaymakerbasic_sessions.end())
                session = gatewaymakerbasic_sessions.emplace(fd, gatewaymakerbasic.connect([server = gatewaymakerbasic_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })).first;
            gatewaymakerbasic.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaymakerbasic_sessions.find(fd); gone != gatewaymakerbasic_sessions.end()) {
                gatewaymakerbasic.disconnect(gone->second);
                gatewaymakerbasic_sessions.erase(gone);
            }
        }) || worked;
        gatewaymakerbasic.tick();
        if (gatewaymakerbasic.ended(makerbasic_client)) {
            gatewaymakerbasic.disconnect(makerbasic_client);
            makerbasic_client = gatewaymakerbasic.connect(makerbasic_reply);
            worked = true;
        }
        for (auto session = gatewaymakerbasic_sessions.begin(); session != gatewaymakerbasic_sessions.end(); ) {
            if (!gatewaymakerbasic.ended(session->second)) { ++session; continue; }
            if (gatewaymakerbasic_order_entry) gatewaymakerbasic_order_entry->close(session->first);
            gatewaymakerbasic.disconnect(session->second);
            session = gatewaymakerbasic_sessions.erase(session);
            worked = true;
        }
        return worked;
    });
    sim::Component gatewaytakerbasic_thread("gatewaytakerbasic", [&] {
        bool worked = to_clients_gatewaytakerbasic.read([&](std::string_view message) { gatewaytakerbasic_events.on_message(message); });
        worked = from_takerbasic.read([&](std::string_view bytes) { gatewaytakerbasic.on_bytes(takerbasic_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewaytakerbasic_order_entry) worked = gatewaytakerbasic_order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = gatewaytakerbasic_sessions.find(fd);
            if (session == gatewaytakerbasic_sessions.end())
                session = gatewaytakerbasic_sessions.emplace(fd, gatewaytakerbasic.connect([server = gatewaytakerbasic_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })).first;
            gatewaytakerbasic.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaytakerbasic_sessions.find(fd); gone != gatewaytakerbasic_sessions.end()) {
                gatewaytakerbasic.disconnect(gone->second);
                gatewaytakerbasic_sessions.erase(gone);
            }
        }) || worked;
        gatewaytakerbasic.tick();
        if (gatewaytakerbasic.ended(takerbasic_client)) {
            gatewaytakerbasic.disconnect(takerbasic_client);
            takerbasic_client = gatewaytakerbasic.connect(takerbasic_reply);
            worked = true;
        }
        for (auto session = gatewaytakerbasic_sessions.begin(); session != gatewaytakerbasic_sessions.end(); ) {
            if (!gatewaytakerbasic.ended(session->second)) { ++session; continue; }
            if (gatewaytakerbasic_order_entry) gatewaytakerbasic_order_entry->close(session->first);
            gatewaytakerbasic.disconnect(session->second);
            session = gatewaytakerbasic_sessions.erase(session);
            worked = true;
        }
        return worked;
    });
    sim::Component makerbasic_thread("makerbasic", [&] {
        bool worked = to_makerbasic.read([&](std::string_view bytes) { makerbasic.on_bytes(bytes.data(), bytes.size()); });
        makerbasic.tick();   // its session's heartbeats, and the venue gone silent
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (makerbasic.logged_in() && std::chrono::steady_clock::now() >= makerbasic_paced) {
                makerbasic_strategy.step(makerbasic.orders());
                ++makerbasic_ticks;
                makerbasic_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!makerbasic_turn.asked()) return worked;
        if (makerbasic.logged_in()) { makerbasic_strategy.step(makerbasic.orders()); ++makerbasic_ticks; }
        makerbasic_turn.taken();
        return true;
    });
    sim::Component takerbasic_thread("takerbasic", [&] {
        bool worked = to_takerbasic.read([&](std::string_view bytes) { takerbasic.on_bytes(bytes.data(), bytes.size()); });
        takerbasic.tick();   // its session's heartbeats, and the venue gone silent
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (takerbasic.logged_in() && std::chrono::steady_clock::now() >= takerbasic_paced) {
                takerbasic_strategy.step(takerbasic.orders());
                ++takerbasic_ticks;
                takerbasic_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!takerbasic_turn.asked()) return worked;
        if (takerbasic.logged_in()) { takerbasic_strategy.step(takerbasic.orders()); ++takerbasic_ticks; }
        takerbasic_turn.taken();
        return true;
    });

    engine_thread.start();
    sequencer_thread.start();
    day_thread.start();
    deep_thread.start();
    tops_thread.start();
    deepplus_thread.start();
    gatewaymakerbasic_thread.start();
    gatewaytakerbasic_thread.start();
    makerbasic_thread.start();
    takerbasic_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &from_day, &to_deep, &to_tops, &to_deepplus, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &day_thread, &deep_thread, &tops_thread, &deepplus_thread, &makerbasic_thread, &takerbasic_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            for (const auto& scheduled : decltype(day_component)::schedule) {
                if (!(tick == scheduled.tick)) continue;
                day_component.request(scheduled);
                settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &from_day, &to_deep, &to_tops, &to_deepplus, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &day_thread, &deep_thread, &tops_thread, &deepplus_thread, &makerbasic_thread, &takerbasic_thread }, std::chrono::milliseconds(5000)) && settled;
            }
            day_component.indicate();
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &from_day, &to_deep, &to_tops, &to_deepplus, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &day_thread, &deep_thread, &tops_thread, &deepplus_thread, &makerbasic_thread, &takerbasic_thread }, std::chrono::milliseconds(5000)) && settled;
            makerbasic_turn.ask();
            settled = makerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &from_day, &to_deep, &to_tops, &to_deepplus, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &day_thread, &deep_thread, &tops_thread, &deepplus_thread, &makerbasic_thread, &takerbasic_thread }, std::chrono::milliseconds(5000)) && settled;
            takerbasic_turn.ask();
            settled = takerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &from_day, &to_deep, &to_tops, &to_deepplus, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &day_thread, &deep_thread, &tops_thread, &deepplus_thread, &makerbasic_thread, &takerbasic_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        for (const auto& scheduled : decltype(day_component)::schedule) {
            if (!(scheduled.tick >= 20)) continue;
            day_component.request(scheduled);
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &from_day, &to_deep, &to_tops, &to_deepplus, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &day_thread, &deep_thread, &tops_thread, &deepplus_thread, &makerbasic_thread, &takerbasic_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        if (!settled)
            std::puts("iex-simulator: the run did not settle between turns — see the counts below");
    }

    // --serve: live venue. Tcp order entry on the gateway thread; Udp multicast market data.
    if (serve) {
        std::puts("iex-simulator: serving — order entry on Tcp, market data on Udp multicast (Ctrl-C to stop)");
        while (serving.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::puts("iex-simulator: stopping");
    }

    // Stop in producer order. Each link a component reads is closed once it has stopped: nothing
    // reads it again, so a component still writing to it - the gateway answering a trader that
    // has gone - stops waiting for room, and is free to stop in its turn.
    makerbasic_thread.stop();
    to_makerbasic.close();
    takerbasic_thread.stop();
    to_takerbasic.close();
    gatewaymakerbasic_thread.stop();
    to_clients_gatewaymakerbasic.close();
    from_makerbasic.close();
    gatewaytakerbasic_thread.stop();
    to_clients_gatewaytakerbasic.close();
    from_takerbasic.close();
    day_thread.stop();
    sequencer_thread.stop();
    to_sequencer_gatewaymakerbasic.close();
    to_sequencer_gatewaytakerbasic.close();
    from_day.close();
    engine_thread.stop();
    sequenced.close();
    deep_thread.stop();
    to_deep.close();
    tops_thread.stop();
    to_tops.close();
    deepplus_thread.stop();
    to_deepplus.close();

    std::printf("iex-simulator: trader %s (%s) entered %llu, canceled %llu, ExecutionReport=%llu\n",
                makerbasic.name().c_str(), makerbasic.logged_in() ? makerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)makerbasic.entered(), (unsigned long long)makerbasic.canceled(),
            (unsigned long long)makerbasic.answers().ExecutionReport);
    std::printf("iex-simulator: trader %s (%s) entered %llu, canceled %llu, ExecutionReport=%llu\n",
                takerbasic.name().c_str(), takerbasic.logged_in() ? takerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)takerbasic.entered(), (unsigned long long)takerbasic.canceled(),
            (unsigned long long)takerbasic.answers().ExecutionReport);
    std::printf("iex-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
