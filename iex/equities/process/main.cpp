// Generated Investors Exchange Equities simulator — entry point.
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
    std::printf("  order-entry  %s:%u\n", cfg.order_entry.host.c_str(), (unsigned)cfg.order_entry.port);
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
    sim::IpcQueue to_engine("gateway -> engine");
    sim::IpcQueue to_clients("engine -> gateway");
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
        [](const sim::ipc_ns::order_canceled_message&) {});
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
        [](const sim::ipc_ns::order_canceled_message&) {});
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
        [](const sim::ipc_ns::order_canceled_message&) {});

    // Order entry: each client's session reaches the engine as framed Ipc messages.
    auto gateway_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_engine, b, n);
    };
    sim::orderentry::Gateway<decltype(gateway_emit)> gateway(gateway_emit);

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer client_events(
        [&](const sim::ipc_ns::trade_message& t) { gateway.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gateway.on_event(c); });

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients, b, n);
        sim::write_blocking(to_deep, b, n);
        sim::write_blocking(to_tops, b, n);
        sim::write_blocking(to_deepplus, b, n);
    };
    // The engine component, built the same way the per-component binaries build it:
    // what it matches is its own, and where its events go is whatever emit it was handed.
    sim::components::Engine<decltype(event_emit)> engine_component(event_emit, trades);

    // Traders: each trades through the venue's own order entry, as a client session like any
    // other — its bytes reach the gateway over a queue instead of a socket.
    sim::MessageQueue to_alice("gateway -> alice");
    sim::MessageQueue from_alice("alice -> gateway");
    auto alice_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_alice, b, n); };
    sim::traders::Trader<decltype(alice_send)> alice("Alice", "alice", "alice", alice_send);
    const std::size_t alice_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_alice, b, n); });
    sim::traders::MakerBasic<std::remove_reference_t<decltype(alice.orders())>> alice_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3, /*tick*/1);
    int alice_ticks = 0;
    std::chrono::steady_clock::time_point alice_paced{};   // when a served trader next orders
    sim::Turn alice_turn;
    alice.login();
    sim::MessageQueue to_bob("gateway -> bob");
    sim::MessageQueue from_bob("bob -> gateway");
    auto bob_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_bob, b, n); };
    sim::traders::Trader<decltype(bob_send)> bob("Bob", "bob", "bob", bob_send);
    const std::size_t bob_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_bob, b, n); });
    sim::traders::TakerBasic<std::remove_reference_t<decltype(bob.orders())>> bob_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int bob_ticks = 0;
    std::chrono::steady_clock::time_point bob_paced{};   // when a served trader next orders
    sim::Turn bob_turn;
    bob.login();

    // The listening socket belongs to the gateway thread; --serve is what opens it.
    std::unique_ptr<net::TcpServer> order_entry;
    std::map<int, std::size_t> sessions;
    std::unique_ptr<net::TcpServer> deep_snap_srv;
    std::unique_ptr<net::TcpServer> tops_snap_srv;
    std::unique_ptr<net::TcpServer> deepplus_snap_srv;
    if (serve) {
        std::signal(SIGPIPE, SIG_IGN);   // a client hanging up must not kill the venue
        std::signal(SIGINT, on_interrupt);
        order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.order_entry.port);
        deep_snap_srv = std::make_unique<net::TcpServer>("0.0.0.0", cfg.deep_snap_port);
        tops_snap_srv = std::make_unique<net::TcpServer>("0.0.0.0", cfg.tops_snap_port);
        deepplus_snap_srv = std::make_unique<net::TcpServer>("0.0.0.0", cfg.deepplus_snap_port);
    }

    // One thread per component. Each owns its own state: the gateway its client sockets,
    // the engine its book, each feed its publisher — nothing is shared but the queues.
    sim::Component engine_thread("engine", [&] {
        return engine_component.step(to_engine);
    });
    sim::Component deep_thread("deep", [&] {
        bool worked = to_deep.read([&](std::string_view message) { deep_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (deep_snap_srv) worked = deep_snap_srv->poll([&](int fd, std::string_view request) {
            deep_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); });
        }) || worked;
        return worked;
    });
    sim::Component tops_thread("tops", [&] {
        bool worked = to_tops.read([&](std::string_view message) { tops_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (tops_snap_srv) worked = tops_snap_srv->poll([&](int fd, std::string_view request) {
            tops_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); });
        }) || worked;
        return worked;
    });
    sim::Component deepplus_thread("deepplus", [&] {
        bool worked = to_deepplus.read([&](std::string_view message) { deepplus_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (deepplus_snap_srv) worked = deepplus_snap_srv->poll([&](int fd, std::string_view request) {
            deepplus_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); });
        }) || worked;
        return worked;
    });
    sim::Component gateway_thread("gateway", [&] {
        bool worked = to_clients.read([&](std::string_view message) { client_events.on_message(message); });
        worked = from_alice.read([&](std::string_view bytes) { gateway.on_bytes(alice_client, bytes.data(), bytes.size()); }) || worked;
        worked = from_bob.read([&](std::string_view bytes) { gateway.on_bytes(bob_client, bytes.data(), bytes.size()); }) || worked;
        if (order_entry) worked = order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = sessions.find(fd);
            if (session == sessions.end())
                session = sessions.emplace(fd, gateway.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
            gateway.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = sessions.find(fd); gone != sessions.end()) {
                gateway.disconnect(gone->second);
                sessions.erase(gone);
            }
        }) || worked;
        return worked;
    });
    sim::Component alice_thread("alice", [&] {
        bool worked = to_alice.read([&](std::string_view bytes) { alice.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (alice.logged_in() && std::chrono::steady_clock::now() >= alice_paced) {
                alice_strategy.step(alice.orders());
                ++alice_ticks;
                alice_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!alice_turn.asked()) return worked;
        if (alice.logged_in()) { alice_strategy.step(alice.orders()); ++alice_ticks; }
        alice_turn.taken();
        return true;
    });
    sim::Component bob_thread("bob", [&] {
        bool worked = to_bob.read([&](std::string_view bytes) { bob.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (bob.logged_in() && std::chrono::steady_clock::now() >= bob_paced) {
                bob_strategy.step(bob.orders());
                ++bob_ticks;
                bob_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!bob_turn.asked()) return worked;
        if (bob.logged_in()) { bob_strategy.step(bob.orders()); ++bob_ticks; }
        bob_turn.taken();
        return true;
    });

    engine_thread.start();
    deep_thread.start();
    tops_thread.start();
    deepplus_thread.start();
    gateway_thread.start();
    alice_thread.start();
    bob_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_engine, &to_clients, &to_deep, &to_tops, &to_deepplus, &to_alice, &from_alice, &to_bob, &from_bob }, { &engine_thread, &gateway_thread, &deep_thread, &tops_thread, &deepplus_thread, &alice_thread, &bob_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            alice_turn.ask();
            settled = alice_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_deep, &to_tops, &to_deepplus, &to_alice, &from_alice, &to_bob, &from_bob }, { &engine_thread, &gateway_thread, &deep_thread, &tops_thread, &deepplus_thread, &alice_thread, &bob_thread }, std::chrono::milliseconds(5000)) && settled;
            bob_turn.ask();
            settled = bob_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_deep, &to_tops, &to_deepplus, &to_alice, &from_alice, &to_bob, &from_bob }, { &engine_thread, &gateway_thread, &deep_thread, &tops_thread, &deepplus_thread, &alice_thread, &bob_thread }, std::chrono::milliseconds(5000)) && settled;
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
    alice_thread.stop();
    to_alice.close();
    bob_thread.stop();
    to_bob.close();
    gateway_thread.stop();
    to_clients.close();
    from_alice.close();
    from_bob.close();
    engine_thread.stop();
    to_engine.close();
    deep_thread.stop();
    to_deep.close();
    tops_thread.stop();
    to_tops.close();
    deepplus_thread.stop();
    to_deepplus.close();

    std::printf("iex-simulator: trader %s (%s) entered %llu, canceled %llu, ExecutionReport=%llu\n",
                alice.name().c_str(), alice.logged_in() ? alice.session_name().c_str() : "not logged in",
                (unsigned long long)alice.entered(), (unsigned long long)alice.canceled(),
            (unsigned long long)alice.answers().ExecutionReport);
    std::printf("iex-simulator: trader %s (%s) entered %llu, canceled %llu, ExecutionReport=%llu\n",
                bob.name().c_str(), bob.logged_in() ? bob.session_name().c_str() : "not logged in",
                (unsigned long long)bob.entered(), (unsigned long long)bob.canceled(),
            (unsigned long long)bob.answers().ExecutionReport);
    std::printf("iex-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
