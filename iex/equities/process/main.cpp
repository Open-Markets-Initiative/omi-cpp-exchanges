// Generated Investors Exchange Equities simulator — entry point.
//
// One process, one thread per component; every link between them is a queue of framed
// messages, so a component hands bytes on instead of calling the next one:
//   [traders] --(FIX)--> [gateway] --(ipc)--> [engine]
//   [engine] --(ipc)--> [gateway] execution reports, and one queue per market-data feed

#include "FixWire.hpp"
#include "FixSession.hpp"
#include "FixGateway.hpp"
#include "FixOrders.hpp"
#include "FixStrategies.hpp"
#include "MatchingEngine.hpp"
#include "engine/Engine.hpp"
#include "FeedServer.hpp"
#include "FixLog.hpp"
#include "MarketDataLog.hpp"
#include "SnapServer.hpp"
#include "SimulatorConfiguration.hpp"

#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include "protocols/ipc/IpcBus.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
#include "run/Components.hpp"
#include "protocols/marketdata/tops/PacketPublisher.hpp"
#include "protocols/marketdata/deep/PacketPublisher.hpp"
#include "protocols/marketdata/deepplus/PacketPublisher.hpp"
#include "net/Network.hpp"

using namespace iexiexequities::order_entry;

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

    // FIX record log (bracketed header + raw message). Empty path = disabled.
    sim::FixLog fixlog("recordings/fix/session.log");
    // Numbered as each trader sends and as the gateway answers, from their own threads.
    std::atomic<std::uint32_t> out_seq{0};
    std::uint32_t in_seq = 0;

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

    // Order entry: the gateway decodes FIX and reaches the engine as framed Ipc messages.
    auto gateway_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_engine, b, n);
    };
    sim::IpcEngineClient ipc_client(gateway_emit);
    Gateway gateway(ipc_client, [](char, std::string_view) {});

    // The engine's trades, answered to the client as ExecutionReports (35=8) on the
    // gateway's own thread, and logged inbound ('<').
    sim::IpcEventServer client_events(
        [&](const sim::ipc_ns::trade_message& t) {
            ExecutionReport er{};
            { auto r = std::to_chars(er.cl_ord_id, er.cl_ord_id + sizeof er.cl_ord_id - 1, t.aggressor_order_id.get().value()); *r.ptr = '\0'; }
            er.ord_status = '2';  er.exec_type = 'F';   // Filled / Trade
            er.price = static_cast<double>(t.price.get().value());  er.order_qty = static_cast<double>(t.quantity.get().value());
            char erbody[512];
            std::size_t ern = encode(er, erbody);
            fixlog.record('<', ++in_seq, '8', std::string_view(erbody, ern));
        },
        [](const sim::ipc_ns::order_rested_message&) {},
        [](const sim::ipc_ns::order_canceled_message&) {});

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

    // Traders: each logs what it sends, then frames it onto its own link into the gateway.
    sim::MessageQueue from_alice("alice -> gateway");
    Orders alice_orders([&](char type, std::string_view body) {
        fixlog.record('>', ++out_seq, type, body);   // '>' sent, '<' received
        char message[sim::MessageQueue::slot_bytes];
        const std::size_t length = std::min(body.size(), sizeof message - 3);
        const std::uint16_t framed = static_cast<std::uint16_t>(1 + length);
        std::memcpy(message, &framed, 2);
        message[2] = type;
        std::memcpy(message + 3, body.data(), length);
        sim::write_blocking(from_alice, message, 3 + length);
    });
    SimpleMaker<decltype(alice_orders)> alice_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3);
    int alice_ticks = 0;
    sim::Turn alice_turn;
    sim::MessageQueue from_bob("bob -> gateway");
    Orders bob_orders([&](char type, std::string_view body) {
        fixlog.record('>', ++out_seq, type, body);   // '>' sent, '<' received
        char message[sim::MessageQueue::slot_bytes];
        const std::size_t length = std::min(body.size(), sizeof message - 3);
        const std::uint16_t framed = static_cast<std::uint16_t>(1 + length);
        std::memcpy(message, &framed, 2);
        message[2] = type;
        std::memcpy(message + 3, body.data(), length);
        sim::write_blocking(from_bob, message, 3 + length);
    });
    SimpleTaker<decltype(bob_orders)> bob_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int bob_ticks = 0;
    sim::Turn bob_turn;

    // The listening sockets belong to the threads that serve them; --serve opens them.
    std::unique_ptr<net::TcpServer> order_entry;
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

    // One thread per component. Each owns its own state: the traders their strategies, the
    // gateway its sessions, the engine its book, each feed its market data.
    sim::Component engine_thread("engine", [&] {
        return engine_component.step(to_engine);
    });
    sim::Component tops_thread("tops", [&] {
        bool worked = to_tops.read([&](std::string_view message) { tops_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (tops_snap_srv) worked = tops_snap_srv->poll([&](int fd, std::string_view request) {
            tops_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); });
        }) || worked;
        return worked;
    });
    sim::Component deep_thread("deep", [&] {
        bool worked = to_deep.read([&](std::string_view message) { deep_events.on_message(message); });
        // Snapshot requests are served by the feed that retains the segments.
        if (deep_snap_srv) worked = deep_snap_srv->poll([&](int fd, std::string_view request) {
            deep_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); });
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
        worked = from_alice.read([&](std::string_view message) { gateway.on_app(message[2], message.substr(3)); }) || worked;
        worked = from_bob.read([&](std::string_view message) { gateway.on_app(message[2], message.substr(3)); }) || worked;
        // External order entry over Tcp -> the gateway (decode ignores the framing tags).
        if (order_entry) worked = order_entry->poll([&](int, std::string_view msg) {
            char type = 'D';
            auto at = msg.find("\x01" "35=");
            if (at != std::string_view::npos && at + 4 < msg.size()) type = msg[at + 4];
            else if (msg.size() > 3 && msg.compare(0, 3, "35=") == 0) type = msg[3];
            gateway.on_app(type, msg);
        }) || worked;
        return worked;
    });
    sim::Component alice_thread("alice", [&] {
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        if (serve) {
            alice_strategy.step(alice_orders);
            ++alice_ticks;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));   // 20 Hz order flow
            return true;
        }
        if (!alice_turn.asked()) return false;
        alice_strategy.step(alice_orders);
        ++alice_ticks;
        alice_turn.taken();
        return true;
    });
    sim::Component bob_thread("bob", [&] {
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        if (serve) {
            bob_strategy.step(bob_orders);
            ++bob_ticks;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));   // 20 Hz order flow
            return true;
        }
        if (!bob_turn.asked()) return false;
        bob_strategy.step(bob_orders);
        ++bob_ticks;
        bob_turn.taken();
        return true;
    });

    engine_thread.start();
    tops_thread.start();
    deep_thread.start();
    deepplus_thread.start();
    gateway_thread.start();
    alice_thread.start();
    bob_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        bool settled = sim::quiesce({ &to_engine, &to_clients, &to_deep, &to_tops, &to_deepplus, &from_alice, &from_bob }, { &gateway_thread, &engine_thread, &tops_thread, &deep_thread, &deepplus_thread, &alice_thread, &bob_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            alice_turn.ask();
            settled = alice_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_deep, &to_tops, &to_deepplus, &from_alice, &from_bob }, { &gateway_thread, &engine_thread, &tops_thread, &deep_thread, &deepplus_thread, &alice_thread, &bob_thread }, std::chrono::milliseconds(5000)) && settled;
            bob_turn.ask();
            settled = bob_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_deep, &to_tops, &to_deepplus, &from_alice, &from_bob }, { &gateway_thread, &engine_thread, &tops_thread, &deep_thread, &deepplus_thread, &alice_thread, &bob_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        if (!settled)
            std::puts("iex-simulator: the run did not settle between turns — see the counts below");
    }

    // --serve: live venue. Tcp order entry + snapshot servers; Udp multicast market data.
    if (serve) {
        std::puts("iex-simulator: serving — order-entry + snapshot Tcp, market-data multicast (Ctrl-C to stop)");
        while (serving.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::puts("iex-simulator: stopping");
    }

    // Stop in producer order, so each component drains what is already on its queue.
    alice_thread.stop();
    bob_thread.stop();
    gateway_thread.stop();
    engine_thread.stop();
    tops_thread.stop();
    deep_thread.stop();
    deepplus_thread.stop();

    std::printf("iex-simulator: ran %d ticks (alice+bob), %llu trades matched; MD segments recorded\n",
                alice_ticks + bob_ticks, (unsigned long long)trades.load());
    std::printf("iex-simulator: ipc bus carried %llu order + %llu event messages\n",
                (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());

    // Snap (replay) self-check: SnapshotRequest -> Start + Data* + End.
    {
        std::uint64_t served = 0, data_msgs = 0;
        char req[3 + 40 + 4 + 4 + 8] = {};
        std::uint16_t rlen = static_cast<std::uint16_t>(sizeof req - 2);
        std::memcpy(req, &rlen, 2);
        req[2] = snap::MessageType::SnapshotRequest;   // token/channel/session zeroed; min seq 0
        deep_snap.on_message(std::string_view(req, sizeof req), [&](const char* b, std::size_t n) {
            ++served;
            if (n >= 3 && b[2] == snap::MessageType::SnapshotData) ++data_msgs;
        });
        std::printf("iex-simulator: snap replay served %llu messages (%llu segments) from Deep\n",
                    (unsigned long long)served, (unsigned long long)data_msgs);
    }
    return 0;
}
