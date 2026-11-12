// Generated Odx Equities Open simulator — entry point.
// 
// One process, one thread per component; every link between them is a queue of framed
// Ipc messages, so a component hands bytes on instead of calling the next one:
// clients --(SoupBin Tcp)--> [gateway] --(ipc)--> [engine]
// [engine] --(ipc)--> [gateway] client responses
// [engine] --(ipc)--> [itch] Itch market data

#include "protocols/orderentry/Gateway.hpp"
#include "traders/Orders.hpp"
#include "traders/MakerBasic.hpp"
#include "traders/TakerBasic.hpp"
#include "traders/CaptureQuotes.hpp"
#include "protocols/marketdata/itch/PacketPublisher.hpp"
#include "protocols/replay/glimpse/ReplayServer.hpp"
#include "protocols/replay/glimpse/SoupBinClient.hpp"
#include "protocols/replay/itchtcp/ReplayServer.hpp"
#include "protocols/replay/itchtcp/SoupBinClient.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
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

    std::puts("odx-simulator configuration (runtime simulator.ini):");
    std::printf("  order-entry  %s:%u  MakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaymakerbasic_port);
    std::printf("  order-entry  %s:%u  TakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaytakerbasic_port);
    std::printf("  order-entry  %s:%u  Replay's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayreplay_port);
    std::printf("  feed Itch      %s:%u\n", cfg.itch_feed.host.c_str(), (unsigned)cfg.itch_feed.port);
    std::printf("  recording    dir=%s order-entry-log=%s market-data=%s\n", cfg.recording_directory.c_str(), "on", "on");

    // Market-data feed servers — segments tee to the recorder, Snap server, and real UDP multicast.
    sim::MarketDataLog itch_rec("recordings/marketdata/Itch.dat");
    net::UdpMulticastPublisher itch_mc(cfg.itch_feed.host, cfg.itch_feed.port);
    sim::itch::PacketPublisher itch("ODXITCH001", [&](const char* s, std::size_t n) { itch_rec(s, n); itch_mc(s, n); });
    sim::ItchSeconds itch_seconds;   // the second its seconds message last said

    // Replay services: each is served the feed's messages and hands them to its own clients.
    sim::MessageQueue to_glimpse("itch -> glimpse");
    sim::replay::glimpse::Server<std::function<void(const char*, std::size_t)>> glimpse("ODXGL00001");
    glimpse.allow("ALICE1", "alice00001");
    glimpse.allow("BOB001", "bob0000001");
    glimpse.allow("CAROL1", "carol00001");
    glimpse.allow("GUEST1", "guest00001");
    sim::MessageQueue to_itchtcp("itch -> itchtcp");
    sim::replay::itchtcp::Server<std::function<void(const char*, std::size_t)>> itchtcp("ODXITCH001");
    itchtcp.allow("ALICE1", "alice00001");
    itchtcp.allow("BOB001", "bob0000001");
    itchtcp.allow("CAROL1", "carol00001");
    itchtcp.allow("GUEST1", "guest00001");

    // The links between the components. One writer and one reader each: the engine fans
    // an event out by writing one queue per consumer, and takes one queue per producer.
    sim::IpcQueue to_sequencer_gatewaymakerbasic("gatewaymakerbasic -> sequencer");
    sim::IpcQueue to_sequencer_gatewaytakerbasic("gatewaytakerbasic -> sequencer");
    sim::IpcQueue to_sequencer_gatewayreplay("gatewayreplay -> sequencer");
    sim::IpcQueue from_day("day -> sequencer");
    sim::IpcQueue sequenced("sequencer -> engine");
    sim::IpcQueue to_clients_gatewaymakerbasic("engine -> gatewaymakerbasic");
    sim::IpcQueue to_clients_gatewaytakerbasic("engine -> gatewaytakerbasic");
    sim::IpcQueue to_clients_gatewayreplay("engine -> gatewayreplay");
    sim::IpcQueue to_itch("engine -> itch");

    // Counted from several threads: what crossed the links, and what the engine matched.
    std::atomic<std::uint64_t> ipc_msgs{0}, ipc_events{0}, trades{0};

    // Market data: each feed's events, translated by the transforms declared for it (OdxEquities.Ipc.Transforms.xml).
    sim::IpcEventServer itch_events(
        [&](const sim::ipc_ns::trade_message& t) {
            // trade_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
            if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(t, itch_second)) {
                itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
            }
            auto itch_order_executed = sim::to_itch_order_executed(t);
            itch.publish_message(reinterpret_cast<const char*>(&itch_order_executed), sizeof itch_order_executed);
            sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_executed), sizeof itch_order_executed);
            sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_executed), sizeof itch_order_executed);
            itch.flush();
        },
        [&](const sim::ipc_ns::order_rested_message& o) {
            // order_rested_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
            if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(o, itch_second)) {
                itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
            }
            auto itch_order_added = sim::to_itch_order_added(o);
            itch.publish_message(reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
            sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
            sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_added), sizeof itch_order_added);
            itch.flush();
        },
        [&](const sim::ipc_ns::order_canceled_message& c) {
            // order_canceled_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
            if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(c, itch_second)) {
                itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
            }
            auto itch_order_deleted = sim::to_itch_order_deleted(c);
            itch.publish_message(reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
            sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
            sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_deleted), sizeof itch_order_deleted);
            itch.flush();
        },
        [](const sim::ipc_ns::order_accepted_message&) {},
        [](const sim::ipc_ns::order_eliminated_message&) {},
        [](const sim::ipc_ns::order_refused_message&) {},
        [&](const sim::ipc_ns::phase_announced_message& s) {
            // phase_announced_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
            if ((static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'S' || static_cast<char>(s.announcement.get().value()) == 'Q' || static_cast<char>(s.announcement.get().value()) == 'M' || static_cast<char>(s.announcement.get().value()) == 'E' || static_cast<char>(s.announcement.get().value()) == 'C')) {
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_system_event = sim::to_itch_system_event(s);
                itch.publish_message(reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_system_event), sizeof itch_system_event);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'O') {
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                // A message for each tick of the table the instrument is listed under.
                for (const auto& tick : sim::ticks_of_ticktable(s.orderbook.get_trimmed().value())) {
                    auto itch_price_tick_size = sim::to_itch_price_tick_size(s, tick);
                    itch.publish_message(reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_price_tick_size), sizeof itch_price_tick_size);
                }
            }
            if (static_cast<char>(s.announcement.get().value()) == 'O') {
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_orderbook_directory = sim::to_itch_orderbook_directory(s);
                itch.publish_message(reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_orderbook_directory), sizeof itch_orderbook_directory);
            }
            if ((static_cast<char>(s.announcement.get().value()) == 'V' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(s, itch_second)) {
                    itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                    sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                }
                auto itch_trading_state = sim::to_itch_trading_state(s);
                itch.publish_message(reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_trading_state), sizeof itch_trading_state);
            }
            itch.flush();
        },
        [&](const sim::ipc_ns::order_replaced_message& w) {
            // order_replaced_message -> the feeds its transforms declare (OdxEquities.Ipc.Transforms.xml).
            if (odx::odxequities::pts::itch::v2_2::seconds_message itch_second{}; itch_seconds.due(w, itch_second)) {
                itch.publish_message(reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
                sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_second), sizeof itch_second);
            }
            auto itch_order_replaced = sim::to_itch_order_replaced(w);
            itch.publish_message(reinterpret_cast<const char*>(&itch_order_replaced), sizeof itch_order_replaced);
            sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&itch_order_replaced), sizeof itch_order_replaced);
            sim::write_blocking(to_itchtcp, reinterpret_cast<const char*>(&itch_order_replaced), sizeof itch_order_replaced);
            itch.flush();
        });

    // MakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaymakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaymakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaymakerbasic_emit)> gatewaymakerbasic(gatewaymakerbasic_emit, 1, 3);
    gatewaymakerbasic.allow("ALICE1", "alice00001");
    gatewaymakerbasic.allow("BOB001", "bob0000001");
    gatewaymakerbasic.allow("CAROL1", "carol00001");
    gatewaymakerbasic.allow("GUEST1", "guest00001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaymakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaymakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaymakerbasic.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaymakerbasic.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaymakerbasic.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewaymakerbasic.on_event(s); },
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewaymakerbasic.on_event(w); });

    // TakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaytakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaytakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaytakerbasic_emit)> gatewaytakerbasic(gatewaytakerbasic_emit, 2, 3);
    gatewaytakerbasic.allow("ALICE1", "alice00001");
    gatewaytakerbasic.allow("BOB001", "bob0000001");
    gatewaytakerbasic.allow("CAROL1", "carol00001");
    gatewaytakerbasic.allow("GUEST1", "guest00001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaytakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaytakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaytakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaytakerbasic.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaytakerbasic.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaytakerbasic.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewaytakerbasic.on_event(s); },
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewaytakerbasic.on_event(w); });

    // Replay's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewayreplay_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewayreplay, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewayreplay_emit)> gatewayreplay(gatewayreplay_emit, 3, 3);
    gatewayreplay.allow("ALICE1", "alice00001");
    gatewayreplay.allow("BOB001", "bob0000001");
    gatewayreplay.allow("CAROL1", "carol00001");
    gatewayreplay.allow("GUEST1", "guest00001");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewayreplay_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewayreplay.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewayreplay.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewayreplay.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayreplay.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewayreplay.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewayreplay.on_event(s); },
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewayreplay.on_event(w); });

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients_gatewaymakerbasic, b, n);
        sim::write_blocking(to_clients_gatewaytakerbasic, b, n);
        sim::write_blocking(to_clients_gatewayreplay, b, n);
        sim::write_blocking(to_itch, b, n);
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
    sim::traders::Trader<decltype(makerbasic_send)> makerbasic("MakerBasic", "ALICE1", "alice00001", makerbasic_send);
    auto makerbasic_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_makerbasic, b, n); };
    std::size_t makerbasic_client = gatewaymakerbasic.connect(makerbasic_reply);
    sim::traders::MakerBasic<std::remove_reference_t<decltype(makerbasic.orders())>> makerbasic_strategy(/*mid*/10000, /*spread*/20, /*size*/100, /*levels*/3, /*tick*/10);
    int makerbasic_ticks = 0;
    std::chrono::steady_clock::time_point makerbasic_paced{};   // when a served trader next orders
    sim::Turn makerbasic_turn;
    makerbasic.login();
    sim::MessageQueue to_takerbasic("gatewaytakerbasic -> takerbasic");
    sim::MessageQueue from_takerbasic("takerbasic -> gatewaytakerbasic");
    auto takerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_takerbasic, b, n); };
    sim::traders::Trader<decltype(takerbasic_send)> takerbasic("TakerBasic", "BOB001", "bob0000001", takerbasic_send);
    auto takerbasic_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_takerbasic, b, n); };
    std::size_t takerbasic_client = gatewaytakerbasic.connect(takerbasic_reply);
    sim::traders::TakerBasic<std::remove_reference_t<decltype(takerbasic.orders())>> takerbasic_strategy(/*mid*/10000, /*cross*/40, /*size*/50, /*period*/3);
    int takerbasic_ticks = 0;
    std::chrono::steady_clock::time_point takerbasic_paced{};   // when a served trader next orders
    sim::Turn takerbasic_turn;
    takerbasic.login();
    sim::MessageQueue to_replay("gatewayreplay -> replay");
    sim::MessageQueue from_replay("replay -> gatewayreplay");
    auto replay_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_replay, b, n); };
    sim::traders::Trader<decltype(replay_send)> replay("Replay", "CAROL1", "carol00001", replay_send);
    auto replay_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_replay, b, n); };
    std::size_t replay_client = gatewayreplay.connect(replay_reply);
    // Replay takes its orders from a capture of Odx.OdxEquities.Pts.Itch.v2.2: what that market showed,
    // posted here as this trader's own orders. A capture it cannot read leaves it quiet.
    sim::traders::Capture replay_capture("captures/Itch.pcap");
    if (!replay_capture.ok())
        std::printf("odx-simulator: trader Replay has no orders — captures/Itch.pcap %s\n", replay_capture.error().c_str());
    sim::traders::CaptureQuotes replay_read(replay_capture);
    sim::traders::CaptureMaker<std::remove_reference_t<decltype(replay.orders())>> replay_strategy(replay_read.quotes(), /*quotes*/4);
    int replay_ticks = 0;
    std::chrono::steady_clock::time_point replay_paced{};   // when a served trader next orders
    sim::Turn replay_turn;
    replay.login();

    // Each listening socket belongs to its gateway's thread; --serve is what opens them.
    std::unique_ptr<net::TcpServer> gatewaymakerbasic_order_entry;
    std::map<int, std::size_t> gatewaymakerbasic_sessions;
    std::unique_ptr<net::TcpServer> gatewaytakerbasic_order_entry;
    std::map<int, std::size_t> gatewaytakerbasic_sessions;
    std::unique_ptr<net::TcpServer> gatewayreplay_order_entry;
    std::map<int, std::size_t> gatewayreplay_sessions;
    std::unique_ptr<net::TcpServer> glimpse_server;
    std::map<int, std::size_t> glimpse_sessions;
    std::unique_ptr<net::TcpServer> itchtcp_server;
    std::map<int, std::size_t> itchtcp_sessions;
    if (serve) {
        std::signal(SIGPIPE, SIG_IGN);   // a client hanging up must not kill the venue
        std::signal(SIGINT, on_interrupt);
        gatewaymakerbasic_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaymakerbasic_port);
        gatewaytakerbasic_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaytakerbasic_port);
        gatewayreplay_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewayreplay_port);
        glimpse_server = std::make_unique<net::TcpServer>(cfg.glimpse_replay_host, cfg.glimpse_replay_port);
        itchtcp_server = std::make_unique<net::TcpServer>(cfg.itchtcp_replay_host, cfg.itchtcp_replay_port);
    }

    // One thread per component. Each owns its own state: the gateway its client sockets,
    // the engine its book, each feed its publisher — nothing is shared but the queues.
    sim::Component engine_thread("engine", [&] { return engine_component.step(sequenced); });
    sim::Component sequencer_thread("sequencer", [&] { return sequencer_component.step(to_sequencer_gatewaymakerbasic, to_sequencer_gatewaytakerbasic, to_sequencer_gatewayreplay, from_day); });
    sim::Component day_thread("day", [&] { return day_component.step(); });
    sim::Component itch_thread("itch", [&] {
        return to_itch.read([&](std::string_view message) { itch_events.on_message(message); });
    });
    sim::Component glimpse_thread("glimpse", [&] {
        bool worked = to_glimpse.read([&](std::string_view message) { glimpse.on_message(message.data(), message.size()); });
        if (glimpse_server) worked = glimpse_server->poll([&](int fd, std::string_view bytes) {
            auto session = glimpse_sessions.find(fd);
            if (session == glimpse_sessions.end())
                session = glimpse_sessions.emplace(fd, glimpse.connect([server = glimpse_server.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })).first;
            glimpse.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = glimpse_sessions.find(fd); gone != glimpse_sessions.end()) {
                glimpse.disconnect(gone->second);
                glimpse_sessions.erase(gone);
            }
        }) || worked;
        glimpse.tick();
        for (auto session = glimpse_sessions.begin(); session != glimpse_sessions.end(); ) {
            if (!glimpse.ended(session->second)) { ++session; continue; }
            if (glimpse_server) glimpse_server->close(session->first);
            glimpse.disconnect(session->second);
            session = glimpse_sessions.erase(session);
            worked = true;
        }
        return glimpse.pump() || worked;
    });
    sim::Component itchtcp_thread("itchtcp", [&] {
        bool worked = to_itchtcp.read([&](std::string_view message) { itchtcp.on_message(message.data(), message.size()); });
        if (itchtcp_server) worked = itchtcp_server->poll([&](int fd, std::string_view bytes) {
            auto session = itchtcp_sessions.find(fd);
            if (session == itchtcp_sessions.end())
                session = itchtcp_sessions.emplace(fd, itchtcp.connect([server = itchtcp_server.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })).first;
            itchtcp.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = itchtcp_sessions.find(fd); gone != itchtcp_sessions.end()) {
                itchtcp.disconnect(gone->second);
                itchtcp_sessions.erase(gone);
            }
        }) || worked;
        itchtcp.tick();
        for (auto session = itchtcp_sessions.begin(); session != itchtcp_sessions.end(); ) {
            if (!itchtcp.ended(session->second)) { ++session; continue; }
            if (itchtcp_server) itchtcp_server->close(session->first);
            itchtcp.disconnect(session->second);
            session = itchtcp_sessions.erase(session);
            worked = true;
        }
        return itchtcp.pump() || worked;
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
    sim::Component gatewayreplay_thread("gatewayreplay", [&] {
        bool worked = to_clients_gatewayreplay.read([&](std::string_view message) { gatewayreplay_events.on_message(message); });
        worked = from_replay.read([&](std::string_view bytes) { gatewayreplay.on_bytes(replay_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewayreplay_order_entry) worked = gatewayreplay_order_entry->poll([&](int fd, std::string_view bytes) {
            auto session = gatewayreplay_sessions.find(fd);
            if (session == gatewayreplay_sessions.end())
                session = gatewayreplay_sessions.emplace(fd, gatewayreplay.connect([server = gatewayreplay_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })).first;
            gatewayreplay.on_bytes(session->second, bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewayreplay_sessions.find(fd); gone != gatewayreplay_sessions.end()) {
                gatewayreplay.disconnect(gone->second);
                gatewayreplay_sessions.erase(gone);
            }
        }) || worked;
        gatewayreplay.tick();
        if (gatewayreplay.ended(replay_client)) {
            gatewayreplay.disconnect(replay_client);
            replay_client = gatewayreplay.connect(replay_reply);
            worked = true;
        }
        for (auto session = gatewayreplay_sessions.begin(); session != gatewayreplay_sessions.end(); ) {
            if (!gatewayreplay.ended(session->second)) { ++session; continue; }
            if (gatewayreplay_order_entry) gatewayreplay_order_entry->close(session->first);
            gatewayreplay.disconnect(session->second);
            session = gatewayreplay_sessions.erase(session);
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
    sim::Component replay_thread("replay", [&] {
        bool worked = to_replay.read([&](std::string_view bytes) { replay.on_bytes(bytes.data(), bytes.size()); });
        replay.tick();   // its session's heartbeats, and the venue gone silent
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

    engine_thread.start();
    sequencer_thread.start();
    day_thread.start();
    itch_thread.start();
    gatewaymakerbasic_thread.start();
    gatewaytakerbasic_thread.start();
    gatewayreplay_thread.start();
    glimpse_thread.start();
    itchtcp_thread.start();
    makerbasic_thread.start();
    takerbasic_thread.start();
    replay_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &from_day, &to_itch, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_glimpse, &to_itchtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &day_thread, &itch_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &glimpse_thread, &itchtcp_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            for (const auto& scheduled : decltype(day_component)::schedule) {
                if (!(tick == scheduled.tick)) continue;
                day_component.request(scheduled);
                settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &from_day, &to_itch, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_glimpse, &to_itchtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &day_thread, &itch_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &glimpse_thread, &itchtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            }
            makerbasic_turn.ask();
            settled = makerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &from_day, &to_itch, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_glimpse, &to_itchtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &day_thread, &itch_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &glimpse_thread, &itchtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            takerbasic_turn.ask();
            settled = takerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &from_day, &to_itch, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_glimpse, &to_itchtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &day_thread, &itch_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &glimpse_thread, &itchtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            replay_turn.ask();
            settled = replay_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &from_day, &to_itch, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_glimpse, &to_itchtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &day_thread, &itch_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &glimpse_thread, &itchtcp_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        for (const auto& scheduled : decltype(day_component)::schedule) {
            if (!(scheduled.tick >= 20)) continue;
            day_component.request(scheduled);
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &from_day, &to_itch, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_glimpse, &to_itchtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &day_thread, &itch_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &glimpse_thread, &itchtcp_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        if (!settled)
            std::puts("odx-simulator: the run did not settle between turns — see the counts below");
    }

    // --serve: live venue. Tcp order entry on the gateway thread; Udp multicast market data.
    if (serve) {
        std::puts("odx-simulator: serving — order entry on Tcp, market data on Udp multicast (Ctrl-C to stop)");
        while (serving.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::puts("odx-simulator: stopping");
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
    gatewaymakerbasic_thread.stop();
    to_clients_gatewaymakerbasic.close();
    from_makerbasic.close();
    gatewaytakerbasic_thread.stop();
    to_clients_gatewaytakerbasic.close();
    from_takerbasic.close();
    gatewayreplay_thread.stop();
    to_clients_gatewayreplay.close();
    from_replay.close();
    day_thread.stop();
    sequencer_thread.stop();
    to_sequencer_gatewaymakerbasic.close();
    to_sequencer_gatewaytakerbasic.close();
    to_sequencer_gatewayreplay.close();
    from_day.close();
    engine_thread.stop();
    sequenced.close();
    itch_thread.stop();
    to_itch.close();
    glimpse_thread.stop();
    to_glimpse.close();
    itchtcp_thread.stop();
    to_itchtcp.close();

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
            void silent() { over = true; }
        } watched;
        std::size_t session = 0;
        auto to_service = [&](const char* b, std::size_t n) { glimpse.on_bytes(session, b, n); };
        sim::replay::glimpse::soupbin::Client<decltype(to_service), watcher> reader(to_service, watched);
        session = glimpse.connect([&](const char* b, std::size_t n) { reader.on_bytes(b, n); });
        reader.login("ALICE1", "alice00001");
        glimpse.pump();
        std::printf("odx-simulator: Glimpse served %llu messages%s, resume from %llu\n",
                    (unsigned long long)watched.messages, watched.over ? " then ended the session" : "", (unsigned long long)watched.resume);
    }
    // ItchTcp self-check: a client logs in and is served the stream.
    {
        struct watcher {
            std::uint64_t messages = 0;
            std::uint64_t resume = 0;
            bool over = false;
            char refused = 0;
            void accepted(std::string_view, std::uint64_t sequence) { resume = sequence; }
            void rejected(sim::replay::itchtcp::soupbin::status reason) { refused = static_cast<char>(reason); }
            void message(std::string_view) { ++messages; }
            void ended() { over = true; }
            void silent() { over = true; }
        } watched;
        std::size_t session = 0;
        auto to_service = [&](const char* b, std::size_t n) { itchtcp.on_bytes(session, b, n); };
        sim::replay::itchtcp::soupbin::Client<decltype(to_service), watcher> reader(to_service, watched);
        session = itchtcp.connect([&](const char* b, std::size_t n) { reader.on_bytes(b, n); });
        reader.login("ALICE1", "alice00001", {}, 1);   // from the first message of the session
        itchtcp.pump();
        std::printf("odx-simulator: ItchTcp served %llu messages%s, resume from %llu\n",
                    (unsigned long long)watched.messages, watched.over ? " then ended the session" : "", (unsigned long long)watched.resume);
    }

    std::printf("odx-simulator: trader %s (%s) entered %llu, canceled %llu, system_event=%llu, order_accepted=%llu, order_replaced=%llu, order_executed=%llu, order_canceled=%llu, order_rejected=%llu\n",
                makerbasic.name().c_str(), makerbasic.logged_in() ? makerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)makerbasic.entered(), (unsigned long long)makerbasic.canceled(),
            (unsigned long long)makerbasic.answers().system_event_message,
            (unsigned long long)makerbasic.answers().order_accepted_message,
            (unsigned long long)makerbasic.answers().order_replaced_message,
            (unsigned long long)makerbasic.answers().order_executed_message,
            (unsigned long long)makerbasic.answers().order_canceled_message,
            (unsigned long long)makerbasic.answers().order_rejected_message);
    if (makerbasic.silences() > 0)
        std::printf("odx-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", makerbasic.name().c_str(),
                    (unsigned long long)makerbasic.silences(), makerbasic.logged_in() ? "logged in again" : "not logged in again");
    std::printf("odx-simulator: trader %s (%s) entered %llu, canceled %llu, system_event=%llu, order_accepted=%llu, order_replaced=%llu, order_executed=%llu, order_canceled=%llu, order_rejected=%llu\n",
                takerbasic.name().c_str(), takerbasic.logged_in() ? takerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)takerbasic.entered(), (unsigned long long)takerbasic.canceled(),
            (unsigned long long)takerbasic.answers().system_event_message,
            (unsigned long long)takerbasic.answers().order_accepted_message,
            (unsigned long long)takerbasic.answers().order_replaced_message,
            (unsigned long long)takerbasic.answers().order_executed_message,
            (unsigned long long)takerbasic.answers().order_canceled_message,
            (unsigned long long)takerbasic.answers().order_rejected_message);
    if (takerbasic.silences() > 0)
        std::printf("odx-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", takerbasic.name().c_str(),
                    (unsigned long long)takerbasic.silences(), takerbasic.logged_in() ? "logged in again" : "not logged in again");
    std::printf("odx-simulator: trader %s replayed %zu quotes from captures/Itch.pcap\n", replay.name().c_str(), replay_strategy.available());
    std::printf("odx-simulator: trader %s (%s) entered %llu, canceled %llu, system_event=%llu, order_accepted=%llu, order_replaced=%llu, order_executed=%llu, order_canceled=%llu, order_rejected=%llu\n",
                replay.name().c_str(), replay.logged_in() ? replay.session_name().c_str() : "not logged in",
                (unsigned long long)replay.entered(), (unsigned long long)replay.canceled(),
            (unsigned long long)replay.answers().system_event_message,
            (unsigned long long)replay.answers().order_accepted_message,
            (unsigned long long)replay.answers().order_replaced_message,
            (unsigned long long)replay.answers().order_executed_message,
            (unsigned long long)replay.answers().order_canceled_message,
            (unsigned long long)replay.answers().order_rejected_message);
    if (replay.silences() > 0)
        std::printf("odx-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", replay.name().c_str(),
                    (unsigned long long)replay.silences(), replay.logged_in() ? "logged in again" : "not logged in again");
    std::printf("odx-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
