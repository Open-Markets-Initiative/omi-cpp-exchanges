// Generated Cme Globex simulator — entry point.
// 
// One process, one thread per component; every link between them is a queue of framed
// Ipc messages, so a component hands bytes on instead of calling the next one:
// clients --(Cme iLink3)--> [gateway] --(ipc)--> [engine]
// [engine] --(ipc)--> [gateway] client responses

#include "protocols/orderentry/Gateway.hpp"
#include "traders/Orders.hpp"
#include "traders/Maker.hpp"
#include "traders/Taker.hpp"
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

    std::puts("cme-simulator configuration (runtime simulator.ini):");
    std::printf("  order-entry  %s:%u\n", cfg.order_entry.host.c_str(), (unsigned)cfg.order_entry.port);
    std::printf("  recording    dir=%s order-entry-log=%s market-data=%s\n", cfg.recording_directory.c_str(), "on", "on");

    // Market-data feed servers — segments tee to the recorder, Snap server, and real UDP multicast.

    // The links between the components. One writer and one reader each: the engine fans
    // an event out by writing one queue per consumer, and takes one queue per producer.
    sim::IpcQueue to_engine("gateway -> engine");
    sim::IpcQueue to_clients("engine -> gateway");

    // Counted from several threads: what crossed the links, and what the engine matched.
    std::atomic<std::uint64_t> ipc_msgs{0}, ipc_events{0}, trades{0};

    // No feed declares transforms, so the engine's events have no market-data consumer.

    // Order entry: each client's session reaches the engine as framed Ipc messages.
    auto gateway_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_engine, b, n);
    };
    sim::orderentry::Gateway<decltype(gateway_emit)> gateway(gateway_emit);
    gateway.allow("ABC", "TESTF");
    gateway.allow("DEF", "TESTF");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer client_events(
        [&](const sim::ipc_ns::trade_message& t) { gateway.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gateway.on_event(c); });

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients, b, n);
    };
    // The engine component, built the same way the per-component binaries build it:
    // what it matches is its own, and where its events go is whatever emit it was handed.
    sim::components::Engine<decltype(event_emit)> engine_component(event_emit, trades);

    // Traders: each trades through the venue's own order entry, as a client session like any
    // other — its bytes reach the gateway over a queue instead of a socket.
    sim::MessageQueue to_maker("gateway -> maker");
    sim::MessageQueue from_maker("maker -> gateway");
    auto maker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_maker, b, n); };
    sim::traders::Trader<decltype(maker_send)> maker("Maker", "ABC", "TESTF", maker_send);
    maker.orders().trade(sim::instruments[0]);
    const std::size_t maker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_maker, b, n); });
    sim::traders::SimpleMaker<std::remove_reference_t<decltype(maker.orders())>> maker_strategy(/*mid*/5000000000000, /*spread*/250000000, /*size*/10, /*levels*/3);
    int maker_ticks = 0;
    std::chrono::steady_clock::time_point maker_paced{};   // when a served trader next orders
    sim::Turn maker_turn;
    maker.login();
    sim::MessageQueue to_taker("gateway -> taker");
    sim::MessageQueue from_taker("taker -> gateway");
    auto taker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_taker, b, n); };
    sim::traders::Trader<decltype(taker_send)> taker("Taker", "DEF", "TESTF", taker_send);
    taker.orders().trade(sim::instruments[0]);
    const std::size_t taker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_taker, b, n); });
    sim::traders::SimpleTaker<std::remove_reference_t<decltype(taker.orders())>> taker_strategy(/*mid*/5000000000000, /*cross*/500000000, /*size*/5, /*period*/3);
    int taker_ticks = 0;
    std::chrono::steady_clock::time_point taker_paced{};   // when a served trader next orders
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
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (maker.logged_in() && std::chrono::steady_clock::now() >= maker_paced) {
                maker_strategy.step(maker.orders());
                ++maker_ticks;
                maker_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!maker_turn.asked()) return worked;
        if (maker.logged_in()) { maker_strategy.step(maker.orders()); ++maker_ticks; }
        maker_turn.taken();
        return true;
    });
    sim::Component taker_thread("taker", [&] {
        bool worked = to_taker.read([&](std::string_view bytes) { taker.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (taker.logged_in() && std::chrono::steady_clock::now() >= taker_paced) {
                taker_strategy.step(taker.orders());
                ++taker_ticks;
                taker_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!taker_turn.asked()) return worked;
        if (taker.logged_in()) { taker_strategy.step(taker.orders()); ++taker_ticks; }
        taker_turn.taken();
        return true;
    });

    engine_thread.start();
    gateway_thread.start();
    maker_thread.start();
    taker_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_engine, &to_clients, &to_maker, &from_maker, &to_taker, &from_taker }, { &engine_thread, &gateway_thread, &maker_thread, &taker_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            maker_turn.ask();
            settled = maker_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_maker, &from_maker, &to_taker, &from_taker }, { &engine_thread, &gateway_thread, &maker_thread, &taker_thread }, std::chrono::milliseconds(5000)) && settled;
            taker_turn.ask();
            settled = taker_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_maker, &from_maker, &to_taker, &from_taker }, { &engine_thread, &gateway_thread, &maker_thread, &taker_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        if (!settled)
            std::puts("cme-simulator: the run did not settle between turns — see the counts below");
    }

    // --serve: live venue. Tcp order entry on the gateway thread; Udp multicast market data.
    if (serve) {
        std::puts("cme-simulator: serving — order entry on Tcp, market data on Udp multicast (Ctrl-C to stop)");
        while (serving.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::puts("cme-simulator: stopping");
    }

    // Stop in producer order. Each link a component reads is closed once it has stopped: nothing
    // reads it again, so a component still writing to it - the gateway answering a trader that
    // has gone - stops waiting for room, and is free to stop in its turn.
    maker_thread.stop();
    to_maker.close();
    taker_thread.stop();
    to_taker.close();
    gateway_thread.stop();
    to_clients.close();
    from_maker.close();
    from_taker.close();
    engine_thread.stop();
    to_engine.close();

    std::printf("cme-simulator: trader %s (%s) entered %llu, canceled %llu, execution_report_new=%llu, execution_report_cancel=%llu, execution_report_trade_outright=%llu\n",
                maker.name().c_str(), maker.logged_in() ? "established" : "not established",
                (unsigned long long)maker.entered(), (unsigned long long)maker.canceled(),
            (unsigned long long)maker.answers().execution_report_new,
            (unsigned long long)maker.answers().execution_report_cancel,
            (unsigned long long)maker.answers().execution_report_trade_outright);
    std::printf("cme-simulator: trader %s (%s) entered %llu, canceled %llu, execution_report_new=%llu, execution_report_cancel=%llu, execution_report_trade_outright=%llu\n",
                taker.name().c_str(), taker.logged_in() ? "established" : "not established",
                (unsigned long long)taker.entered(), (unsigned long long)taker.canceled(),
            (unsigned long long)taker.answers().execution_report_new,
            (unsigned long long)taker.answers().execution_report_cancel,
            (unsigned long long)taker.answers().execution_report_trade_outright);
    std::printf("cme-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
