// Generated Nasdaq Stock Market Open simulator — entry point.
// 
// One process, one thread per component; every link between them is a queue of framed
// Ipc messages, so a component hands bytes on instead of calling the next one:
// clients --(SoupBin Tcp)--> [gateway] --(ipc)--> [engine]
// [engine] --(ipc)--> [gateway] client responses
// [engine] --(ipc)--> [totalview] TotalView market data

#include "protocols/orderentry/Gateway.hpp"
#include "traders/Orders.hpp"
#include "traders/MakerBasic.hpp"
#include "traders/TakerBasic.hpp"
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
#include "sequencer/Sequencer.hpp"
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
    std::printf("  order-entry  %s:%u  MakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaymakerbasic_port);
    std::printf("  order-entry  %s:%u  TakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaytakerbasic_port);
    std::printf("  order-entry  %s:%u  Replay's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayreplay_port);
    std::printf("  order-entry  %s:%u  Requests's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayrequests_port);
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
    sim::IpcQueue to_sequencer_gatewaymakerbasic("gatewaymakerbasic -> sequencer");
    sim::IpcQueue to_sequencer_gatewaytakerbasic("gatewaytakerbasic -> sequencer");
    sim::IpcQueue to_sequencer_gatewayreplay("gatewayreplay -> sequencer");
    sim::IpcQueue to_sequencer_gatewayrequests("gatewayrequests -> sequencer");
    sim::IpcQueue sequenced("sequencer -> engine");
    sim::IpcQueue to_clients_gatewaymakerbasic("engine -> gatewaymakerbasic");
    sim::IpcQueue to_clients_gatewaytakerbasic("engine -> gatewaytakerbasic");
    sim::IpcQueue to_clients_gatewayreplay("engine -> gatewayreplay");
    sim::IpcQueue to_clients_gatewayrequests("engine -> gatewayrequests");
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
        },
        [](const sim::ipc_ns::order_accepted_message&) {},
        [](const sim::ipc_ns::order_eliminated_message&) {},
        [](const sim::ipc_ns::order_refused_message&) {});

    // MakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaymakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaymakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaymakerbasic_emit)> gatewaymakerbasic(gatewaymakerbasic_emit, 1, 4);
    gatewaymakerbasic.allow("ALICE1", "alice00001");
    gatewaymakerbasic.allow("BOB001", "bob0000001");
    gatewaymakerbasic.allow("CAROL1", "carol00001");
    gatewaymakerbasic.allow("DAVE01", "dave000001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaymakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaymakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaymakerbasic.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaymakerbasic.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaymakerbasic.on_event(j); });

    // TakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaytakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaytakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaytakerbasic_emit)> gatewaytakerbasic(gatewaytakerbasic_emit, 2, 4);
    gatewaytakerbasic.allow("ALICE1", "alice00001");
    gatewaytakerbasic.allow("BOB001", "bob0000001");
    gatewaytakerbasic.allow("CAROL1", "carol00001");
    gatewaytakerbasic.allow("DAVE01", "dave000001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaytakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaytakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaytakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaytakerbasic.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaytakerbasic.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaytakerbasic.on_event(j); });

    // Replay's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewayreplay_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewayreplay, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewayreplay_emit)> gatewayreplay(gatewayreplay_emit, 3, 4);
    gatewayreplay.allow("ALICE1", "alice00001");
    gatewayreplay.allow("BOB001", "bob0000001");
    gatewayreplay.allow("CAROL1", "carol00001");
    gatewayreplay.allow("DAVE01", "dave000001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewayreplay_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewayreplay.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewayreplay.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewayreplay.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayreplay.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewayreplay.on_event(j); });

    // Requests's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewayrequests_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewayrequests, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewayrequests_emit)> gatewayrequests(gatewayrequests_emit, 4, 4);
    gatewayrequests.allow("ALICE1", "alice00001");
    gatewayrequests.allow("BOB001", "bob0000001");
    gatewayrequests.allow("CAROL1", "carol00001");
    gatewayrequests.allow("DAVE01", "dave000001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewayrequests_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewayrequests.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewayrequests.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewayrequests.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayrequests.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewayrequests.on_event(j); });

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients_gatewaymakerbasic, b, n);
        sim::write_blocking(to_clients_gatewaytakerbasic, b, n);
        sim::write_blocking(to_clients_gatewayreplay, b, n);
        sim::write_blocking(to_clients_gatewayrequests, b, n);
        sim::write_blocking(to_totalview, b, n);
    };
    // The engine component, built the same way the per-component binaries build it:
    // what it matches is its own, and where its events go is whatever emit it was handed.
    sim::components::Engine<decltype(event_emit)> engine_component(event_emit, trades);
    // Each engine's sequencer: what is bound for the engine, from every producer, put in one
    // order, each message stamped with its place in it and the time, and handed to the engine.
    auto sequencer_emit = [&](const char* b, std::size_t n) { sim::write_blocking(sequenced, b, n); };
    sim::components::Sequencer<decltype(sequencer_emit)> sequencer_component(sequencer_emit);

    // Traders: each trades through the venue's own order entry, as a client session like any
    // other — its bytes reach the gateway over a queue instead of a socket.
    sim::MessageQueue to_makerbasic("gatewaymakerbasic -> makerbasic");
    sim::MessageQueue from_makerbasic("makerbasic -> gatewaymakerbasic");
    auto makerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_makerbasic, b, n); };
    sim::traders::Trader<decltype(makerbasic_send)> makerbasic("MakerBasic", "ALICE1", "alice00001", makerbasic_send);
    const std::size_t makerbasic_client = gatewaymakerbasic.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_makerbasic, b, n); });
    sim::traders::MakerBasic<std::remove_reference_t<decltype(makerbasic.orders())>> makerbasic_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3, /*tick*/1);
    int makerbasic_ticks = 0;
    std::chrono::steady_clock::time_point makerbasic_paced{};   // when a served trader next orders
    sim::Turn makerbasic_turn;
    makerbasic.login();
    sim::MessageQueue to_takerbasic("gatewaytakerbasic -> takerbasic");
    sim::MessageQueue from_takerbasic("takerbasic -> gatewaytakerbasic");
    auto takerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_takerbasic, b, n); };
    sim::traders::Trader<decltype(takerbasic_send)> takerbasic("TakerBasic", "BOB001", "bob0000001", takerbasic_send);
    const std::size_t takerbasic_client = gatewaytakerbasic.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_takerbasic, b, n); });
    sim::traders::TakerBasic<std::remove_reference_t<decltype(takerbasic.orders())>> takerbasic_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int takerbasic_ticks = 0;
    std::chrono::steady_clock::time_point takerbasic_paced{};   // when a served trader next orders
    sim::Turn takerbasic_turn;
    takerbasic.login();
    sim::MessageQueue to_replay("gatewayreplay -> replay");
    sim::MessageQueue from_replay("replay -> gatewayreplay");
    auto replay_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_replay, b, n); };
    sim::traders::Trader<decltype(replay_send)> replay("Replay", "CAROL1", "carol00001", replay_send);
    const std::size_t replay_client = gatewayreplay.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_replay, b, n); });
    // Replay takes its orders from a capture of Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026: what that market showed,
    // posted here as this trader's own orders. A capture it cannot read leaves it quiet.
    sim::traders::Capture replay_capture("captures/TotalView.pcap");
    if (!replay_capture.ok())
        std::printf("nasdaq-simulator: trader Replay has no orders — captures/TotalView.pcap %s\n", replay_capture.error().c_str());
    sim::traders::CaptureQuotes replay_read(replay_capture);
    sim::traders::CaptureMaker<std::remove_reference_t<decltype(replay.orders())>> replay_strategy(replay_read.quotes(), /*quotes*/4);
    int replay_ticks = 0;
    std::chrono::steady_clock::time_point replay_paced{};   // when a served trader next orders
    sim::Turn replay_turn;
    replay.login();
    sim::MessageQueue to_requests("gatewayrequests -> requests");
    sim::MessageQueue from_requests("requests -> gatewayrequests");
    auto requests_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_requests, b, n); };
    sim::traders::Trader<decltype(requests_send)> requests("Requests", "DAVE01", "dave000001", requests_send);
    const std::size_t requests_client = gatewayrequests.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_requests, b, n); });
    // Requests takes its orders from a capture of this venue's own order entry: the requests a
    // client sent, replayed here as this trader's own. A capture it cannot read leaves it quiet.
    sim::traders::Capture requests_capture("captures/Ouch.pcap");
    if (!requests_capture.ok())
        std::printf("nasdaq-simulator: trader Requests has no orders — captures/Ouch.pcap %s\n", requests_capture.error().c_str());
    sim::traders::CaptureRequests requests_read(requests_capture);
    sim::traders::CaptureReplay<std::remove_reference_t<decltype(requests.orders())>> requests_strategy(requests_read.requests(), /*quotes*/3);
    int requests_ticks = 0;
    std::chrono::steady_clock::time_point requests_paced{};   // when a served trader next orders
    sim::Turn requests_turn;
    requests.login();

    // Each listening socket belongs to its gateway's thread; --serve is what opens them.
    std::unique_ptr<net::TcpServer> gatewaymakerbasic_order_entry;
    std::map<int, std::size_t> gatewaymakerbasic_sessions;
    std::unique_ptr<net::TcpServer> gatewaytakerbasic_order_entry;
    std::map<int, std::size_t> gatewaytakerbasic_sessions;
    std::unique_ptr<net::TcpServer> gatewayreplay_order_entry;
    std::map<int, std::size_t> gatewayreplay_sessions;
    std::unique_ptr<net::TcpServer> gatewayrequests_order_entry;
    std::map<int, std::size_t> gatewayrequests_sessions;
    std::unique_ptr<net::TcpServer> glimpse_server;
    std::map<int, std::size_t> glimpse_sessions;
    std::unique_ptr<net::TcpServer> totalviewtcp_server;
    std::map<int, std::size_t> totalviewtcp_sessions;
    if (serve) {
        std::signal(SIGPIPE, SIG_IGN);   // a client hanging up must not kill the venue
        std::signal(SIGINT, on_interrupt);
        gatewaymakerbasic_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaymakerbasic_port);
        gatewaytakerbasic_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaytakerbasic_port);
        gatewayreplay_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewayreplay_port);
        gatewayrequests_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewayrequests_port);
        glimpse_server = std::make_unique<net::TcpServer>(cfg.glimpse_replay_port == 0 ? "0.0.0.0" : "127.0.0.1", cfg.glimpse_replay_port);
        totalviewtcp_server = std::make_unique<net::TcpServer>(cfg.totalviewtcp_replay_port == 0 ? "0.0.0.0" : "127.0.0.1", cfg.totalviewtcp_replay_port);
    }

    // One thread per component. Each owns its own state: the gateway its client sockets,
    // the engine its book, each feed its publisher — nothing is shared but the queues.
    sim::Component engine_thread("engine", [&] { return engine_component.step(sequenced); });
    sim::Component sequencer_thread("sequencer", [&] { return sequencer_component.step(to_sequencer_gatewaymakerbasic, to_sequencer_gatewaytakerbasic, to_sequencer_gatewayreplay, to_sequencer_gatewayrequests); });
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
    sim::Component gatewaymakerbasic_thread("gatewaymakerbasic", [&] {
        bool worked = to_clients_gatewaymakerbasic.read([&](std::string_view message) { gatewaymakerbasic_events.on_message(message); });
        worked = from_makerbasic.read([&](std::string_view bytes) { gatewaymakerbasic.on_bytes(makerbasic_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewaymakerbasic_order_entry) worked = gatewaymakerbasic_order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = gatewaymakerbasic_sessions.find(fd);
            if (session == gatewaymakerbasic_sessions.end())
                session = gatewaymakerbasic_sessions.emplace(fd, gatewaymakerbasic.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
            gatewaymakerbasic.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaymakerbasic_sessions.find(fd); gone != gatewaymakerbasic_sessions.end()) {
                gatewaymakerbasic.disconnect(gone->second);
                gatewaymakerbasic_sessions.erase(gone);
            }
        }) || worked;
        return worked;
    });
    sim::Component gatewaytakerbasic_thread("gatewaytakerbasic", [&] {
        bool worked = to_clients_gatewaytakerbasic.read([&](std::string_view message) { gatewaytakerbasic_events.on_message(message); });
        worked = from_takerbasic.read([&](std::string_view bytes) { gatewaytakerbasic.on_bytes(takerbasic_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewaytakerbasic_order_entry) worked = gatewaytakerbasic_order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = gatewaytakerbasic_sessions.find(fd);
            if (session == gatewaytakerbasic_sessions.end())
                session = gatewaytakerbasic_sessions.emplace(fd, gatewaytakerbasic.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
            gatewaytakerbasic.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaytakerbasic_sessions.find(fd); gone != gatewaytakerbasic_sessions.end()) {
                gatewaytakerbasic.disconnect(gone->second);
                gatewaytakerbasic_sessions.erase(gone);
            }
        }) || worked;
        return worked;
    });
    sim::Component gatewayreplay_thread("gatewayreplay", [&] {
        bool worked = to_clients_gatewayreplay.read([&](std::string_view message) { gatewayreplay_events.on_message(message); });
        worked = from_replay.read([&](std::string_view bytes) { gatewayreplay.on_bytes(replay_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewayreplay_order_entry) worked = gatewayreplay_order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = gatewayreplay_sessions.find(fd);
            if (session == gatewayreplay_sessions.end())
                session = gatewayreplay_sessions.emplace(fd, gatewayreplay.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
            gatewayreplay.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewayreplay_sessions.find(fd); gone != gatewayreplay_sessions.end()) {
                gatewayreplay.disconnect(gone->second);
                gatewayreplay_sessions.erase(gone);
            }
        }) || worked;
        return worked;
    });
    sim::Component gatewayrequests_thread("gatewayrequests", [&] {
        bool worked = to_clients_gatewayrequests.read([&](std::string_view message) { gatewayrequests_events.on_message(message); });
        worked = from_requests.read([&](std::string_view bytes) { gatewayrequests.on_bytes(requests_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewayrequests_order_entry) worked = gatewayrequests_order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = gatewayrequests_sessions.find(fd);
            if (session == gatewayrequests_sessions.end())
                session = gatewayrequests_sessions.emplace(fd, gatewayrequests.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
            gatewayrequests.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewayrequests_sessions.find(fd); gone != gatewayrequests_sessions.end()) {
                gatewayrequests.disconnect(gone->second);
                gatewayrequests_sessions.erase(gone);
            }
        }) || worked;
        return worked;
    });
    sim::Component makerbasic_thread("makerbasic", [&] {
        bool worked = to_makerbasic.read([&](std::string_view bytes) { makerbasic.on_bytes(bytes.data(), bytes.size()); });
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
    sim::Component replay_thread("replay", [&] {
        bool worked = to_replay.read([&](std::string_view bytes) { replay.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (replay.logged_in() && std::chrono::steady_clock::now() >= replay_paced) {
                replay_strategy.step(replay.orders());
                ++replay_ticks;
                replay_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!replay_turn.asked()) return worked;
        if (replay.logged_in()) { replay_strategy.step(replay.orders()); ++replay_ticks; }
        replay_turn.taken();
        return true;
    });
    sim::Component requests_thread("requests", [&] {
        bool worked = to_requests.read([&](std::string_view bytes) { requests.on_bytes(bytes.data(), bytes.size()); });
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (requests.logged_in() && std::chrono::steady_clock::now() >= requests_paced) {
                requests_strategy.step(requests.orders());
                ++requests_ticks;
                requests_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!requests_turn.asked()) return worked;
        if (requests.logged_in()) { requests_strategy.step(requests.orders()); ++requests_ticks; }
        requests_turn.taken();
        return true;
    });

    engine_thread.start();
    sequencer_thread.start();
    totalview_thread.start();
    gatewaymakerbasic_thread.start();
    gatewaytakerbasic_thread.start();
    gatewayreplay_thread.start();
    gatewayrequests_thread.start();
    glimpse_thread.start();
    totalviewtcp_thread.start();
    makerbasic_thread.start();
    takerbasic_thread.start();
    replay_thread.start();
    requests_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            makerbasic_turn.ask();
            settled = makerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            takerbasic_turn.ask();
            settled = takerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            replay_turn.ask();
            settled = replay_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            requests_turn.ask();
            settled = requests_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
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

    // Stop in producer order. Each link a component reads is closed once it has stopped: nothing
    // reads it again, so a component still writing to it - the gateway answering a trader that
    // has gone - stops waiting for room, and is free to stop in its turn.
    makerbasic_thread.stop();
    to_makerbasic.close();
    takerbasic_thread.stop();
    to_takerbasic.close();
    replay_thread.stop();
    to_replay.close();
    requests_thread.stop();
    to_requests.close();
    gatewaymakerbasic_thread.stop();
    to_clients_gatewaymakerbasic.close();
    from_makerbasic.close();
    gatewaytakerbasic_thread.stop();
    to_clients_gatewaytakerbasic.close();
    from_takerbasic.close();
    gatewayreplay_thread.stop();
    to_clients_gatewayreplay.close();
    from_replay.close();
    gatewayrequests_thread.stop();
    to_clients_gatewayrequests.close();
    from_requests.close();
    sequencer_thread.stop();
    to_sequencer_gatewaymakerbasic.close();
    to_sequencer_gatewaytakerbasic.close();
    to_sequencer_gatewayreplay.close();
    to_sequencer_gatewayrequests.close();
    engine_thread.stop();
    sequenced.close();
    totalview_thread.stop();
    to_totalview.close();
    glimpse_thread.stop();
    to_glimpse.close();
    totalviewtcp_thread.stop();
    to_totalviewtcp.close();

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

    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu, rejected_order=%llu\n",
                makerbasic.name().c_str(), makerbasic.logged_in() ? makerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)makerbasic.entered(), (unsigned long long)makerbasic.canceled(),
            (unsigned long long)makerbasic.answers().order_accepted_message,
            (unsigned long long)makerbasic.answers().order_executed_message,
            (unsigned long long)makerbasic.answers().canceled_message,
            (unsigned long long)makerbasic.answers().rejected_order_message);
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu, rejected_order=%llu\n",
                takerbasic.name().c_str(), takerbasic.logged_in() ? takerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)takerbasic.entered(), (unsigned long long)takerbasic.canceled(),
            (unsigned long long)takerbasic.answers().order_accepted_message,
            (unsigned long long)takerbasic.answers().order_executed_message,
            (unsigned long long)takerbasic.answers().canceled_message,
            (unsigned long long)takerbasic.answers().rejected_order_message);
    std::printf("nasdaq-simulator: trader %s replayed %zu quotes from captures/TotalView.pcap\n", replay.name().c_str(), replay_strategy.available());
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu, rejected_order=%llu\n",
                replay.name().c_str(), replay.logged_in() ? replay.session_name().c_str() : "not logged in",
                (unsigned long long)replay.entered(), (unsigned long long)replay.canceled(),
            (unsigned long long)replay.answers().order_accepted_message,
            (unsigned long long)replay.answers().order_executed_message,
            (unsigned long long)replay.answers().canceled_message,
            (unsigned long long)replay.answers().rejected_order_message);
    std::printf("nasdaq-simulator: trader %s replayed %zu quotes from captures/Ouch.pcap\n", requests.name().c_str(), requests_strategy.available());
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, order_accepted=%llu, order_executed=%llu, canceled=%llu, rejected_order=%llu\n",
                requests.name().c_str(), requests.logged_in() ? requests.session_name().c_str() : "not logged in",
                (unsigned long long)requests.entered(), (unsigned long long)requests.canceled(),
            (unsigned long long)requests.answers().order_accepted_message,
            (unsigned long long)requests.answers().order_executed_message,
            (unsigned long long)requests.answers().canceled_message,
            (unsigned long long)requests.answers().rejected_order_message);
    std::printf("nasdaq-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
