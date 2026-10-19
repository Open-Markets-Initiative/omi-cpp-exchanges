// Generated Nasdaq Stock Market simulator — entry point.
// 
// One process, one thread per component; every link between them is a queue of framed
// Ipc messages, so a component hands bytes on instead of calling the next one:
// clients --(SoupBin Tcp)--> [gateway] --(ipc)--> [engine]
// [engine] --(ipc)--> [gateway] client responses
// [engine] --(ipc)--> [totalview] TotalView market data

#include "protocols/orderentry/Gateway.hpp"
#include "traders/Orders.hpp"
#include "traders/Maker.hpp"
#include "traders/Taker.hpp"
#include "traders/CaptureQuotes.hpp"
#include "traders/CaptureRequests.hpp"
#include "protocols/marketdata/totalview/PacketPublisher.hpp"
#include "protocols/replay/glimpse/ReplayServer.hpp"
#include "protocols/replay/glimpse/SoupBinClient.hpp"
#include "protocols/replay/totalviewtcp/ReplayServer.hpp"
#include "protocols/replay/totalviewtcp/SoupBinClient.hpp"
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

    std::puts("nasdaq-simulator configuration (runtime simulator.ini):");
    std::printf("  order-entry  %s:%u\n", cfg.order_entry.host.c_str(), (unsigned)cfg.order_entry.port);
    std::printf("  feed TotalView %s:%u\n", cfg.totalview_feed.host.c_str(), (unsigned)cfg.totalview_feed.port);
    std::printf("  recording    dir=%s order-entry-log=%s market-data=%s\n", cfg.recording_directory.c_str(), "on", "on");

    // Market-data feed servers — segments tee to the recorder, Snap server, and real UDP multicast.
    sim::MarketDataLog totalview_rec("recordings/marketdata/TotalView.dat");
    net::UdpMulticastPublisher totalview_mc(cfg.totalview_feed.host, cfg.totalview_feed.port);
    sim::totalview::PacketPublisher totalview("NSMTV00001", [&](const char* s, std::size_t n) { totalview_rec(s, n); totalview_mc(s, n); });

    // Replay services: each is served the feed's messages and hands them to its own clients.
    sim::MessageQueue to_glimpse("totalview -> glimpse");
    sim::replay::glimpse::Server<std::function<void(const char*, std::size_t)>> glimpse("NSMGL00001");
    glimpse.allow("ALICE1", "alice00001");
    glimpse.allow("BOB001", "bob0000001");
    glimpse.allow("CAROL1", "carol00001");
    glimpse.allow("DAVE01", "dave000001");
    sim::MessageQueue to_totalviewtcp("totalview -> totalviewtcp");
    sim::replay::totalviewtcp::Server<std::function<void(const char*, std::size_t)>> totalviewtcp("NSMTV00001");
    totalviewtcp.allow("ALICE1", "alice00001");
    totalviewtcp.allow("BOB001", "bob0000001");
    totalviewtcp.allow("CAROL1", "carol00001");
    totalviewtcp.allow("DAVE01", "dave000001");

    // The links between the components. One writer and one reader each: the engine fans
    // an event out by writing one queue per consumer, and takes one queue per producer.
    sim::IpcQueue to_engine("gateway -> engine");
    sim::IpcQueue to_clients("engine -> gateway");
    sim::IpcQueue to_totalview("engine -> totalview");

    // Counted from several threads: what crossed the links, and what the engine matched.
    std::atomic<std::uint64_t> ipc_msgs{0}, ipc_events{0}, trades{0};

    // Market data: each feed's events, translated by the transforms declared for it (NsmEquities.Ipc.Transforms.xml).
    sim::IpcEventServer totalview_events(
        [&](const sim::ipc_ns::trade_message& t) {
            // trade_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            auto totalview_order_executed = sim::to_totalview_order_executed(t);
            totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
            sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
            sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
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
        });

    // Order entry: each client's session reaches the engine as framed Ipc messages.
    auto gateway_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_engine, b, n);
    };
    sim::orderentry::Gateway<decltype(gateway_emit)> gateway(gateway_emit);
    gateway.allow("ALICE1", "alice00001");
    gateway.allow("BOB001", "bob0000001");
    gateway.allow("CAROL1", "carol00001");
    gateway.allow("DAVE01", "dave000001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer client_events(
        [&](const sim::ipc_ns::trade_message& t) { gateway.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gateway.on_event(c); });

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients, b, n);
        sim::write_blocking(to_totalview, b, n);
    };
    // The engine component, built the same way the per-component binaries build it:
    // what it matches is its own, and where its events go is whatever emit it was handed.
    sim::components::Engine<decltype(event_emit)> engine_component(event_emit, trades);

    // Traders: each trades through the venue's own order entry, as a client session like any
    // other — its bytes reach the gateway over a queue instead of a socket.
    sim::MessageQueue to_maker("gateway -> maker");
    sim::MessageQueue from_maker("maker -> gateway");
    auto maker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_maker, b, n); };
    sim::traders::Trader<decltype(maker_send)> maker("Maker", "ALICE1", "alice00001", maker_send);
    const std::size_t maker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_maker, b, n); });
    sim::traders::SimpleMaker<std::remove_reference_t<decltype(maker.orders())>> maker_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3);
    int maker_ticks = 0;
    sim::Turn maker_turn;
    maker.login();
    sim::MessageQueue to_taker("gateway -> taker");
    sim::MessageQueue from_taker("taker -> gateway");
    auto taker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_taker, b, n); };
    sim::traders::Trader<decltype(taker_send)> taker("Taker", "BOB001", "bob0000001", taker_send);
    const std::size_t taker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_taker, b, n); });
    sim::traders::SimpleTaker<std::remove_reference_t<decltype(taker.orders())>> taker_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int taker_ticks = 0;
    sim::Turn taker_turn;
    taker.login();
    sim::MessageQueue to_replay("gateway -> replay");
    sim::MessageQueue from_replay("replay -> gateway");
    auto replay_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_replay, b, n); };
    sim::traders::Trader<decltype(replay_send)> replay("Replay", "CAROL1", "carol00001", replay_send);
    const std::size_t replay_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_replay, b, n); });
    // Replay takes its orders from a capture of Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026: what that market showed,
    // posted here as this trader's own orders. A capture it cannot read leaves it quiet.
    sim::traders::Capture replay_capture("captures/TotalView.pcap");
    if (!replay_capture.ok())
        std::printf("nasdaq-simulator: trader Replay has no orders — captures/TotalView.pcap %s\n", replay_capture.error().c_str());
    sim::traders::CaptureQuotes replay_read(replay_capture);
    sim::traders::CaptureMaker<std::remove_reference_t<decltype(replay.orders())>> replay_strategy(replay_read.quotes(), /*quotes*/4);
    int replay_ticks = 0;
    sim::Turn replay_turn;
    replay.login();
    sim::MessageQueue to_requests("gateway -> requests");
    sim::MessageQueue from_requests("requests -> gateway");
    auto requests_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_requests, b, n); };
    sim::traders::Trader<decltype(requests_send)> requests("Requests", "DAVE01", "dave000001", requests_send);
    const std::size_t requests_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_requests, b, n); });
    // Requests takes its orders from a capture of this venue's own order entry: the requests a
    // client sent, replayed here as this trader's own. A capture it cannot read leaves it quiet.
    sim::traders::Capture requests_capture("captures/Ouch.pcap");
    if (!requests_capture.ok())
        std::printf("nasdaq-simulator: trader Requests has no orders — captures/Ouch.pcap %s\n", requests_capture.error().c_str());
    sim::traders::CaptureRequests requests_read(requests_capture);
    sim::traders::CaptureReplay<std::remove_reference_t<decltype(requests.orders())>> requests_strategy(requests_read.requests(), /*quotes*/3);
    int requests_ticks = 0;
    sim::Turn requests_turn;
    requests.login();

    // The listening socket belongs to the gateway thread; --serve is what opens it.
    std::unique_ptr<net::TcpServer> order_entry;
    std::map<int, std::size_t> sessions;
    std::unique_ptr<net::TcpServer> glimpse_server;
    std::map<int, std::size_t> glimpse_sessions;
    std::unique_ptr<net::TcpServer> totalviewtcp_server;
    std::map<int, std::size_t> totalviewtcp_sessions;
    if (serve) {
        std::signal(SIGPIPE, SIG_IGN);   // a client hanging up must not kill the venue
        std::signal(SIGINT, on_interrupt);
        order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.order_entry.port);
        glimpse_server = std::make_unique<net::TcpServer>(cfg.glimpse_replay_port == 0 ? "0.0.0.0" : "127.0.0.1", cfg.glimpse_replay_port);
        totalviewtcp_server = std::make_unique<net::TcpServer>(cfg.totalviewtcp_replay_port == 0 ? "0.0.0.0" : "127.0.0.1", cfg.totalviewtcp_replay_port);
    }

    // One thread per component. Each owns its own state: the gateway its client sockets,
    // the engine its book, each feed its publisher — nothing is shared but the queues.
    sim::Component engine_thread("engine", [&] {
        return engine_component.step(to_engine);
    });
    sim::Component totalview_thread("totalview", [&] {
        return to_totalview.read([&](std::string_view message) { totalview_events.on_message(message); });
    });
    sim::Component glimpse_thread("glimpse", [&] {
        bool worked = to_glimpse.read([&](std::string_view message) { glimpse.on_message(message.data(), message.size()); });
        if (glimpse_server) worked = glimpse_server->poll([&](int fd, std::string_view bytes) {
            auto session = glimpse_sessions.find(fd);
            if (session == glimpse_sessions.end())
                session = glimpse_sessions.emplace(fd, glimpse.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
            glimpse.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = glimpse_sessions.find(fd); gone != glimpse_sessions.end()) {
                glimpse.disconnect(gone->second);
                glimpse_sessions.erase(gone);
            }
        }) || worked;
        return glimpse.pump() || worked;
    });
    sim::Component totalviewtcp_thread("totalviewtcp", [&] {
        bool worked = to_totalviewtcp.read([&](std::string_view message) { totalviewtcp.on_message(message.data(), message.size()); });
        if (totalviewtcp_server) worked = totalviewtcp_server->poll([&](int fd, std::string_view bytes) {
            auto session = totalviewtcp_sessions.find(fd);
            if (session == totalviewtcp_sessions.end())
                session = totalviewtcp_sessions.emplace(fd, totalviewtcp.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
            totalviewtcp.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = totalviewtcp_sessions.find(fd); gone != totalviewtcp_sessions.end()) {
                totalviewtcp.disconnect(gone->second);
                totalviewtcp_sessions.erase(gone);
            }
        }) || worked;
        return totalviewtcp.pump() || worked;
    });
    sim::Component gateway_thread("gateway", [&] {
        bool worked = to_clients.read([&](std::string_view message) { client_events.on_message(message); });
        worked = from_maker.read([&](std::string_view bytes) { gateway.on_bytes(maker_client, bytes.data(), bytes.size()); }) || worked;
        worked = from_taker.read([&](std::string_view bytes) { gateway.on_bytes(taker_client, bytes.data(), bytes.size()); }) || worked;
        worked = from_replay.read([&](std::string_view bytes) { gateway.on_bytes(replay_client, bytes.data(), bytes.size()); }) || worked;
        worked = from_requests.read([&](std::string_view bytes) { gateway.on_bytes(requests_client, bytes.data(), bytes.size()); }) || worked;
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
    sim::Component replay_thread("replay", [&] {
        bool worked = to_replay.read([&](std::string_view bytes) { replay.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        if (serve) {
            if (replay.logged_in()) { replay_strategy.step(replay.orders()); ++replay_ticks; std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
            return true;
        }
        if (!replay_turn.asked()) return worked;
        if (replay.logged_in()) { replay_strategy.step(replay.orders()); ++replay_ticks; }
        replay_turn.taken();
        return true;
    });
    sim::Component requests_thread("requests", [&] {
        bool worked = to_requests.read([&](std::string_view bytes) { requests.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        if (serve) {
            if (requests.logged_in()) { requests_strategy.step(requests.orders()); ++requests_ticks; std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
            return true;
        }
        if (!requests_turn.asked()) return worked;
        if (requests.logged_in()) { requests_strategy.step(requests.orders()); ++requests_ticks; }
        requests_turn.taken();
        return true;
    });

    engine_thread.start();
    totalview_thread.start();
    gateway_thread.start();
    glimpse_thread.start();
    totalviewtcp_thread.start();
    maker_thread.start();
    taker_thread.start();
    replay_thread.start();
    requests_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_engine, &to_clients, &to_totalview, &to_maker, &from_maker, &to_taker, &from_taker, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &gateway_thread, &totalview_thread, &maker_thread, &taker_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            maker_turn.ask();
            settled = maker_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_totalview, &to_maker, &from_maker, &to_taker, &from_taker, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &gateway_thread, &totalview_thread, &maker_thread, &taker_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            taker_turn.ask();
            settled = taker_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_totalview, &to_maker, &from_maker, &to_taker, &from_taker, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &gateway_thread, &totalview_thread, &maker_thread, &taker_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            replay_turn.ask();
            settled = replay_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_totalview, &to_maker, &from_maker, &to_taker, &from_taker, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &gateway_thread, &totalview_thread, &maker_thread, &taker_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            requests_turn.ask();
            settled = requests_turn.wait() && settled;
            settled = sim::quiesce({ &to_engine, &to_clients, &to_totalview, &to_maker, &from_maker, &to_taker, &from_taker, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &gateway_thread, &totalview_thread, &maker_thread, &taker_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        if (!settled)
            std::puts("nasdaq-simulator: the run did not settle between turns — see the counts below");
    }

    // --serve: live venue. Tcp order entry on the gateway thread; Udp multicast market data.
    if (serve) {
        std::puts("nasdaq-simulator: serving — order entry on Tcp, market data on Udp multicast (Ctrl-C to stop)");
        while (serving.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::puts("nasdaq-simulator: stopping");
    }

    // Stop in producer order, so each component drains what is already on its queue.
    maker_thread.stop();
    taker_thread.stop();
    replay_thread.stop();
    requests_thread.stop();
    gateway_thread.stop();
    engine_thread.stop();
    totalview_thread.stop();
    glimpse_thread.stop();
    totalviewtcp_thread.stop();

    // Glimpse self-check: a client logs in and is served the book as it stands.
    {
        struct watcher {
            std::uint64_t messages = 0;
            std::uint64_t resume = 0;
            bool over = false;
            char refused = 0;
            void accepted(std::string_view, std::uint64_t sequence) { resume = sequence; }
            void rejected(sim::replay::glimpse::soupbin::status reason) { refused = static_cast<char>(reason); }
            void message(std::string_view) { ++messages; }
            void ended() { over = true; }
        } watched;
        std::size_t session = 0;
        auto to_service = [&](const char* b, std::size_t n) { glimpse.on_bytes(session, b, n); };
        sim::replay::glimpse::soupbin::Client<decltype(to_service), watcher> reader(to_service, watched);
        session = glimpse.connect([&](const char* b, std::size_t n) { reader.on_bytes(b, n); });
        reader.login("ALICE1", "alice00001");
        glimpse.pump();
        std::printf("nasdaq-simulator: Glimpse served %llu messages%s, resume from %llu\n",
                    (unsigned long long)watched.messages, watched.over ? " then ended the session" : "", (unsigned long long)watched.resume);
    }
    // TotalViewTcp self-check: a client logs in and is served the stream.
    {
        struct watcher {
            std::uint64_t messages = 0;
            std::uint64_t resume = 0;
            bool over = false;
            char refused = 0;
            void accepted(std::string_view, std::uint64_t sequence) { resume = sequence; }
            void rejected(sim::replay::totalviewtcp::soupbin::status reason) { refused = static_cast<char>(reason); }
            void message(std::string_view) { ++messages; }
            void ended() { over = true; }
        } watched;
        std::size_t session = 0;
        auto to_service = [&](const char* b, std::size_t n) { totalviewtcp.on_bytes(session, b, n); };
        sim::replay::totalviewtcp::soupbin::Client<decltype(to_service), watcher> reader(to_service, watched);
        session = totalviewtcp.connect([&](const char* b, std::size_t n) { reader.on_bytes(b, n); });
        reader.login("ALICE1", "alice00001", {}, 1);   // from the first message of the session
        totalviewtcp.pump();
        std::printf("nasdaq-simulator: TotalViewTcp served %llu messages%s, resume from %llu\n",
                    (unsigned long long)watched.messages, watched.over ? " then ended the session" : "", (unsigned long long)watched.resume);
    }

    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu\n",
                maker.name().c_str(), maker.logged_in() ? maker.session_name().c_str() : "not logged in",
                (unsigned long long)maker.entered(), (unsigned long long)maker.canceled(),
            (unsigned long long)maker.answers().order_accepted_message,
            (unsigned long long)maker.answers().order_executed_message,
            (unsigned long long)maker.answers().canceled_message);
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu\n",
                taker.name().c_str(), taker.logged_in() ? taker.session_name().c_str() : "not logged in",
                (unsigned long long)taker.entered(), (unsigned long long)taker.canceled(),
            (unsigned long long)taker.answers().order_accepted_message,
            (unsigned long long)taker.answers().order_executed_message,
            (unsigned long long)taker.answers().canceled_message);
    std::printf("nasdaq-simulator: trader %s replayed %zu quotes from captures/TotalView.pcap\n", replay.name().c_str(), replay_strategy.available());
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu\n",
                replay.name().c_str(), replay.logged_in() ? replay.session_name().c_str() : "not logged in",
                (unsigned long long)replay.entered(), (unsigned long long)replay.canceled(),
            (unsigned long long)replay.answers().order_accepted_message,
            (unsigned long long)replay.answers().order_executed_message,
            (unsigned long long)replay.answers().canceled_message);
    std::printf("nasdaq-simulator: trader %s replayed %zu quotes from captures/Ouch.pcap\n", requests.name().c_str(), requests_strategy.available());
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu\n",
                requests.name().c_str(), requests.logged_in() ? requests.session_name().c_str() : "not logged in",
                (unsigned long long)requests.entered(), (unsigned long long)requests.canceled(),
            (unsigned long long)requests.answers().order_accepted_message,
            (unsigned long long)requests.answers().order_executed_message,
            (unsigned long long)requests.answers().canceled_message);
    std::printf("nasdaq-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
