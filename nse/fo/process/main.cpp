// Generated Nse Futures and Options simulator — entry point.
// 
// One process, one thread per component; every link between them is a queue of framed
// Ipc messages, so a component hands bytes on instead of calling the next one:
// clients --(Nse Nnf)--> [gateway] --(ipc)--> [engine]
// [engine] --(ipc)--> [gateway] client responses
// [engine] --(ipc)--> [mtbt] Mtbt market data

#include "protocols/orderentry/Gateway.hpp"
#include "traders/Orders.hpp"
#include "traders/Maker.hpp"
#include "traders/Taker.hpp"
#include "protocols/marketdata/mtbt/PacketPublisher.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
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

    std::puts("nse-simulator configuration (runtime simulator.ini):");
    std::printf("  order-entry  %s:%u\n", cfg.order_entry.host.c_str(), (unsigned)cfg.order_entry.port);
    std::printf("  feed Mtbt      %s:%u\n", cfg.mtbt_feed.host.c_str(), (unsigned)cfg.mtbt_feed.port);
    std::printf("  recording    dir=%s order-entry-log=%s market-data=%s\n", cfg.recording_directory.c_str(), "on", "on");

    // Market-data feed servers — segments tee to the recorder, Snap server, and real UDP multicast.
    sim::MarketDataLog mtbt_rec("recordings/marketdata/Mtbt.dat");
    net::UdpMulticastPublisher mtbt_mc(cfg.mtbt_feed.host, cfg.mtbt_feed.port);
    sim::mtbt::PacketPublisher mtbt("1", [&](const char* s, std::size_t n) { mtbt_rec(s, n); mtbt_mc(s, n); });

    // The links between the components. One writer and one reader each: the engine fans
    // an event out by writing one queue per consumer, and takes one queue per producer.
    sim::IpcQueue to_engine("gateway -> engine");
    sim::IpcQueue to_clients("engine -> gateway");
    sim::IpcQueue to_mtbt("engine -> mtbt");

    // Counted from several threads: what crossed the links, and what the engine matched.
    std::atomic<std::uint64_t> ipc_msgs{0}, ipc_events{0}, trades{0};

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

    // Order entry: each client's session reaches the engine as framed Ipc messages.
    auto gateway_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_engine, b, n);
    };
    sim::orderentry::Gateway<decltype(gateway_emit)> gateway(gateway_emit);
    gateway.allow("10001", "alice001");
    gateway.allow("10002", "bob00001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer client_events(
        [&](const sim::ipc_ns::trade_message& t) { gateway.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gateway.on_event(c); });

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients, b, n);
        sim::write_blocking(to_mtbt, b, n);
    };
    // The engine component, built the same way the per-component binaries build it:
    // what it matches is its own, and where its events go is whatever emit it was handed.
    sim::components::Engine<decltype(event_emit)> engine_component(event_emit, trades);

    // Traders: each trades through the venue's own order entry, as a client session like any
    // other — its bytes reach the gateway over a queue instead of a socket.
    sim::MessageQueue to_maker("gateway -> maker");
    sim::MessageQueue from_maker("maker -> gateway");
    auto maker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_maker, b, n); };
    sim::traders::Trader<decltype(maker_send)> maker("Maker", "10001", "alice001", maker_send);
    const std::size_t maker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_maker, b, n); });
    sim::traders::SimpleMaker<std::remove_reference_t<decltype(maker.orders())>> maker_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3);
    int maker_ticks = 0;
    sim::Turn maker_turn;
    maker.login();
    sim::MessageQueue to_taker("gateway -> taker");
    sim::MessageQueue from_taker("taker -> gateway");
    auto taker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_taker, b, n); };
    sim::traders::Trader<decltype(taker_send)> taker("Taker", "10002", "bob00001", taker_send);
    const std::size_t taker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_taker, b, n); });
    sim::traders::SimpleTaker<std::remove_reference_t<decltype(taker.orders())>> taker_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int taker_ticks = 0;
    sim::Turn taker_turn;
    taker.login();

    // The listening socket belongs to the gateway thread; --serve is what opens it.
    std::unique_ptr<net::TcpServer> order_entry;
    std::map<int, std::size_t> sessions;
    if (serve) {
        std::signal(SIGPIPE, SIG_IGN);   // a client hanging up must not kill the venue
        std::signal(SIGINT, on_interrupt);
        order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.order_entry.port);
    }

    // One thread per component. Each owns its own state: the gateway its client sockets,
    // the engine its book, each feed its publisher — nothing is shared but the queues.
    sim::Component engine_thread("engine", [&] {
        return engine_component.step(to_engine);
    });
    sim::Component mtbt_thread("mtbt", [&] {
        return to_mtbt.read([&](std::string_view message) { mtbt_events.on_message(message); });
    });
    sim::Component gateway_thread("gateway", [&] {
        bool worked = to_clients.read([&](std::string_view message) { client_events.on_message(message); });
        worked = from_maker.read([&](std::string_view bytes) { gateway.on_bytes(maker_client, bytes.data(), bytes.size()); }) || worked;
        worked = from_taker.read([&](std::string_view bytes) { gateway.on_bytes(taker_client, bytes.data(), bytes.size()); }) || worked;
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
    sim::Component maker_thread("maker", [&] {
        bool worked = to_maker.read([&](std::string_view bytes) { maker.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        if (serve) {
            if (maker.logged_in()) { maker_strategy.step(maker.orders()); ++maker_ticks; std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
            return true;
        }
        if (!maker_turn.asked()) return worked;
        if (maker.logged_in()) { maker_strategy.step(maker.orders()); ++maker_ticks; }
        maker_turn.taken();
        return true;
    });
    sim::Component taker_thread("taker", [&] {
        bool worked = to_taker.read([&](std::string_view bytes) { taker.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        if (serve) {
            if (taker.logged_in()) { taker_strategy.step(taker.orders()); ++taker_ticks; std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
            return true;
        }
        if (!taker_turn.asked()) return worked;
        if (taker.logged_in()) { taker_strategy.step(taker.orders()); ++taker_ticks; }
        taker_turn.taken();
        return true;
    });

    engine_thread.start();
    mtbt_thread.start();
    gateway_thread.start();
    maker_thread.start();
    taker_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_engine, &to_clients, &to_mtbt, &to_maker, &from_maker, &to_taker, &from_taker }, { &engine_thread, &gateway_thread, &mtbt_thread, &maker_thread, &taker_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            maker_turn.ask();
            settled = maker_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_mtbt, &to_maker, &from_maker, &to_taker, &from_taker }, { &engine_thread, &gateway_thread, &mtbt_thread, &maker_thread, &taker_thread }, std::chrono::milliseconds(5000)) && settled;
            taker_turn.ask();
            settled = taker_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_mtbt, &to_maker, &from_maker, &to_taker, &from_taker }, { &engine_thread, &gateway_thread, &mtbt_thread, &maker_thread, &taker_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        if (!settled)
            std::puts("nse-simulator: the run did not settle between turns — see the counts below");
    }

    // --serve: live venue. Tcp order entry on the gateway thread; Udp multicast market data.
    if (serve) {
        std::puts("nse-simulator: serving — order entry on Tcp, market data on Udp multicast (Ctrl-C to stop)");
        while (serving.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::puts("nse-simulator: stopping");
    }

    // Stop in producer order, so each component drains what is already on its queue.
    maker_thread.stop();
    taker_thread.stop();
    gateway_thread.stop();
    engine_thread.stop();
    mtbt_thread.stop();

    std::printf("nse-simulator: trader %s (%s) entered %llu, canceled %llu, order_confirmation=%llu, order_cancellation_confirmation_order_entry_body=%llu, trade_confirmation=%llu\n",
                maker.name().c_str(), maker.logged_in() ? "signed on" : "not signed on",
                (unsigned long long)maker.entered(), (unsigned long long)maker.canceled(),
            (unsigned long long)maker.answers().order_confirmation_message,
            (unsigned long long)maker.answers().order_cancellation_confirmation_order_entry_body,
            (unsigned long long)maker.answers().trade_confirmation_message);
    std::printf("nse-simulator: trader %s (%s) entered %llu, canceled %llu, order_confirmation=%llu, order_cancellation_confirmation_order_entry_body=%llu, trade_confirmation=%llu\n",
                taker.name().c_str(), taker.logged_in() ? "signed on" : "not signed on",
                (unsigned long long)taker.entered(), (unsigned long long)taker.canceled(),
            (unsigned long long)taker.answers().order_confirmation_message,
            (unsigned long long)taker.answers().order_cancellation_confirmation_order_entry_body,
            (unsigned long long)taker.answers().trade_confirmation_message);
    std::printf("nse-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
