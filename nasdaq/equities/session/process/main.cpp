// Generated Nasdaq Stock Market Session simulator — entry point.
// 
// One process, one thread per component; every link between them is a queue of framed
// Ipc messages, so a component hands bytes on instead of calling the next one:
// clients --(SoupBin Tcp)--> [gateway] --(ipc)--> [engine]
// clients --(Fix)--> [Fix gateway] --(ipc)--> [engine]
// [engine] --(ipc)--> [gateway] client responses
// [engine] --(ipc)--> [totalview] TotalView market data

#include "protocols/orderentry/Gateway.hpp"
#include "protocols/orderentry/fix/Gateway.hpp"
#include "traders/Orders.hpp"
#include "traders/fix/Orders.hpp"
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

    std::puts("nasdaq-simulator configuration (runtime simulator.ini):");
    std::printf("  order-entry  %s:%u  MakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaymakerbasic_port);
    std::printf("  order-entry  %s:%u  TakerBasic's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaytakerbasic_port);
    std::printf("  order-entry  %s:%u  Replay's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayreplay_port);
    std::printf("  order-entry  %s:%u  Requests's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayrequests_port);
    std::printf("  order-entry  %s:%u  MakerSecond's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaymakersecond_port);
    std::printf("  order-entry  %s:%u  TakerSecond's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaytakersecond_port);
    std::printf("  order-entry  %s:%u  FixMaker's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayfixmaker_port);
    std::printf("  order-entry  %s:%u  FixTaker's gateway\n", cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayfixtaker_port);
    std::printf("  feed TotalView %s:%u\n", cfg.totalview_feed.host.c_str(), (unsigned)cfg.totalview_feed.port);
    std::printf("  recording    dir=%s order-entry-log=%s market-data=%s\n", cfg.recording_directory.c_str(), "on", "on");

    // Market-data feed servers — segments tee to the recorder, Snap server, and real UDP multicast.
    sim::MarketDataLog totalview_rec("recordings/marketdata/TotalView.dat");
    net::UdpMulticastPublisher totalview_mc(cfg.totalview_feed.host, cfg.totalview_feed.port);
    sim::totalview::PacketPublisher totalview("NSMTV00001", [&](const char* s, std::size_t n) { totalview_rec(s, n); totalview_mc(s, n); });

    // Replay services: each is served the feed's messages and hands them to its own clients.
    sim::MessageQueue to_glimpse("totalview -> glimpse");
    sim::replay::glimpse::Server<std::function<void(const char*, std::size_t)>> glimpse("NSMGL00001");
    glimpse.allow("MAKER1", "maker10000");
    glimpse.allow("TAKER1", "taker10000");
    glimpse.allow("REPLAY", "replay0000");
    glimpse.allow("ASKER1", "asker10000");
    glimpse.allow("MAKER2", "maker20000");
    glimpse.allow("TAKER2", "taker20000");
    glimpse.allow("FIXMKR", "fixmkr0000");
    glimpse.allow("FIXTKR", "fixtkr0000");
    glimpse.allow("RISKST", "riskst0000");
    glimpse.allow("RSTRCT", "rstrct0000");
    glimpse.allow("RATEDP", "ratedp0000");
    glimpse.allow("KILLSW", "killsw0000");
    glimpse.allow("GROSSX", "grossx0000");
    glimpse.allow("FATFNG", "fatfng0000");
    glimpse.allow("OUCETC", "oucetc0000");
    glimpse.allow("HALTCX", "haltcx0000");
    glimpse.allow("MELOBY", "meloby0000");
    glimpse.allow("MELOSL", "melosl0000");
    glimpse.allow("MELOPG", "melopg0000");
    glimpse.allow("FIXETC", "fixetc0000");
    glimpse.allow("FIXOPT", "fixopt0000");
    glimpse.allow("FIXRSK", "fixrsk0000");
    sim::MessageQueue to_totalviewtcp("totalview -> totalviewtcp");
    sim::replay::totalviewtcp::Server<std::function<void(const char*, std::size_t)>> totalviewtcp("NSMTV00001");
    totalviewtcp.allow("MAKER1", "maker10000");
    totalviewtcp.allow("TAKER1", "taker10000");
    totalviewtcp.allow("REPLAY", "replay0000");
    totalviewtcp.allow("ASKER1", "asker10000");
    totalviewtcp.allow("MAKER2", "maker20000");
    totalviewtcp.allow("TAKER2", "taker20000");
    totalviewtcp.allow("FIXMKR", "fixmkr0000");
    totalviewtcp.allow("FIXTKR", "fixtkr0000");
    totalviewtcp.allow("RISKST", "riskst0000");
    totalviewtcp.allow("RSTRCT", "rstrct0000");
    totalviewtcp.allow("RATEDP", "ratedp0000");
    totalviewtcp.allow("KILLSW", "killsw0000");
    totalviewtcp.allow("GROSSX", "grossx0000");
    totalviewtcp.allow("FATFNG", "fatfng0000");
    totalviewtcp.allow("OUCETC", "oucetc0000");
    totalviewtcp.allow("HALTCX", "haltcx0000");
    totalviewtcp.allow("MELOBY", "meloby0000");
    totalviewtcp.allow("MELOSL", "melosl0000");
    totalviewtcp.allow("MELOPG", "melopg0000");
    totalviewtcp.allow("FIXETC", "fixetc0000");
    totalviewtcp.allow("FIXOPT", "fixopt0000");
    totalviewtcp.allow("FIXRSK", "fixrsk0000");

    // The links between the components. One writer and one reader each: the engine fans
    // an event out by writing one queue per consumer, and takes one queue per producer.
    sim::IpcQueue to_sequencer_gatewaymakerbasic("gatewaymakerbasic -> sequencer");
    sim::IpcQueue to_sequencer_gatewaytakerbasic("gatewaytakerbasic -> sequencer");
    sim::IpcQueue to_sequencer_gatewayreplay("gatewayreplay -> sequencer");
    sim::IpcQueue to_sequencer_gatewayrequests("gatewayrequests -> sequencer");
    sim::IpcQueue to_sequencer_gatewaymakersecond("gatewaymakersecond -> sequencer");
    sim::IpcQueue to_sequencer_gatewaytakersecond("gatewaytakersecond -> sequencer");
    sim::IpcQueue to_sequencer_gatewayfixmaker("gatewayfixmaker -> sequencer");
    sim::IpcQueue to_sequencer_gatewayfixtaker("gatewayfixtaker -> sequencer");
    sim::IpcQueue from_day("day -> sequencer");
    sim::IpcQueue sequenced("sequencer -> engine");
    sim::IpcQueue to_clients_gatewaymakerbasic("engine -> gatewaymakerbasic");
    sim::IpcQueue to_clients_gatewaytakerbasic("engine -> gatewaytakerbasic");
    sim::IpcQueue to_clients_gatewayreplay("engine -> gatewayreplay");
    sim::IpcQueue to_clients_gatewayrequests("engine -> gatewayrequests");
    sim::IpcQueue to_clients_gatewaymakersecond("engine -> gatewaymakersecond");
    sim::IpcQueue to_clients_gatewaytakersecond("engine -> gatewaytakersecond");
    sim::IpcQueue to_clients_gatewayfixmaker("engine -> gatewayfixmaker");
    sim::IpcQueue to_clients_gatewayfixtaker("engine -> gatewayfixtaker");
    sim::IpcQueue to_totalview("engine -> totalview");

    // Counted from several threads: what crossed the links, and what the engine matched.
    std::atomic<std::uint64_t> ipc_msgs{0}, ipc_events{0}, trades{0};

    // Market data: each feed's events, translated by the transforms declared for it (NsmEquities.Ipc.Transforms.xml).
    sim::IpcEventServer totalview_events(
        [&](const sim::ipc_ns::trade_message& t) {
            // trade_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(t.auction.get().value()) == 'N' && static_cast<char>(t.resting_display.get().value()) != 'N') {
                auto totalview_order_executed = sim::to_totalview_order_executed(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_executed), sizeof totalview_order_executed);
            }
            if (static_cast<char>(t.resting_display.get().value()) == 'Y' && static_cast<char>(t.auction.get().value()) != 'N') {
                auto totalview_crossed_resting = sim::to_totalview_crossed_resting(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_crossed_resting), sizeof totalview_crossed_resting);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_crossed_resting), sizeof totalview_crossed_resting);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_crossed_resting), sizeof totalview_crossed_resting);
            }
            if (static_cast<char>(t.aggressor_display.get().value()) == 'Y' && static_cast<char>(t.auction.get().value()) != 'N') {
                auto totalview_crossed_aggressor = sim::to_totalview_crossed_aggressor(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_crossed_aggressor), sizeof totalview_crossed_aggressor);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_crossed_aggressor), sizeof totalview_crossed_aggressor);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_crossed_aggressor), sizeof totalview_crossed_aggressor);
            }
            if (static_cast<char>(t.auction.get().value()) == 'N' && static_cast<char>(t.aggressor_display.get().value()) != 'N') {
                auto totalview_discretion_taken = sim::to_totalview_discretion_taken(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_discretion_taken), sizeof totalview_discretion_taken);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_discretion_taken), sizeof totalview_discretion_taken);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_discretion_taken), sizeof totalview_discretion_taken);
            }
            if (static_cast<char>(t.auction.get().value()) == 'N' && static_cast<char>(t.resting_display.get().value()) != 'Y') {
                auto totalview_hidden_executed = sim::to_totalview_hidden_executed(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_hidden_executed), sizeof totalview_hidden_executed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_hidden_executed), sizeof totalview_hidden_executed);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_hidden_executed), sizeof totalview_hidden_executed);
            }
            if (static_cast<char>(t.auction.get().value()) == 'A') {
                auto totalview_extended_executed = sim::to_totalview_extended_executed(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_extended_executed), sizeof totalview_extended_executed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_extended_executed), sizeof totalview_extended_executed);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_extended_executed), sizeof totalview_extended_executed);
            }
            if (static_cast<char>(t.auction.get().value()) == 'E') {
                auto totalview_extended_life_executed = sim::to_totalview_extended_life_executed(t);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_extended_life_executed), sizeof totalview_extended_life_executed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_extended_life_executed), sizeof totalview_extended_life_executed);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_extended_life_executed), sizeof totalview_extended_life_executed);
            }
            totalview.flush();
        },
        [&](const sim::ipc_ns::order_rested_message& o) {
            // order_rested_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(o.display.get().value()) == 'Y' && static_cast<char>(o.repriced.get().value()) != 'Y') {
                auto totalview_add_order = sim::to_totalview_add_order(o);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_add_order), sizeof totalview_add_order);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_add_order), sizeof totalview_add_order);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_add_order), sizeof totalview_add_order);
            }
            if (static_cast<char>(o.display.get().value()) == 'A' && static_cast<char>(o.repriced.get().value()) != 'Y') {
                auto totalview_add_order_attributed = sim::to_totalview_add_order_attributed(o);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_add_order_attributed), sizeof totalview_add_order_attributed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_add_order_attributed), sizeof totalview_add_order_attributed);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_add_order_attributed), sizeof totalview_add_order_attributed);
            }
            if (static_cast<char>(o.repriced.get().value()) == 'Y' && static_cast<char>(o.display.get().value()) != 'N') {
                auto totalview_order_repriced = sim::to_totalview_order_repriced(o);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_repriced), sizeof totalview_order_repriced);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_repriced), sizeof totalview_order_repriced);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_repriced), sizeof totalview_order_repriced);
            }
            totalview.flush();
        },
        [&](const sim::ipc_ns::order_canceled_message& c) {
            // order_canceled_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(c.display.get().value()) == 'Y') {
                auto totalview_order_delete = sim::to_totalview_order_delete(c);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_delete), sizeof totalview_order_delete);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_delete), sizeof totalview_order_delete);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_delete), sizeof totalview_order_delete);
            }
            totalview.flush();
        },
        [](const sim::ipc_ns::order_accepted_message&) {},
        [&](const sim::ipc_ns::order_eliminated_message& e) {
            // order_eliminated_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(e.display.get().value()) == 'Y') {
                auto totalview_order_expired = sim::to_totalview_order_expired(e);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_expired), sizeof totalview_order_expired);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_expired), sizeof totalview_order_expired);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_expired), sizeof totalview_order_expired);
            }
            totalview.flush();
        },
        [](const sim::ipc_ns::order_refused_message&) {},
        [&](const sim::ipc_ns::phase_announced_message& s) {
            // phase_announced_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(s.announcement.get().value()) != 'C' && (static_cast<char>(s.announcement.get().value()) == 'O' || static_cast<char>(s.announcement.get().value()) == 'S' || static_cast<char>(s.announcement.get().value()) == 'Q' || static_cast<char>(s.announcement.get().value()) == 'M' || static_cast<char>(s.announcement.get().value()) == 'E' || static_cast<char>(s.announcement.get().value()) == 'C') && s.symbol.get_trimmed().value() == "ZVZZT") {
                auto totalview_system_event = sim::to_totalview_system_event(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_system_event), sizeof totalview_system_event);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_system_event), sizeof totalview_system_event);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_system_event), sizeof totalview_system_event);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'C' && s.symbol.get_trimmed().value() == "ZXZZT") {
                auto totalview_end_of_messages = sim::to_totalview_end_of_messages(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_end_of_messages), sizeof totalview_end_of_messages);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_end_of_messages), sizeof totalview_end_of_messages);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_end_of_messages), sizeof totalview_end_of_messages);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'H' && s.release.get().value() != 0) {
                auto totalview_ipo_quoting_period_update = sim::to_totalview_ipo_quoting_period_update(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_ipo_quoting_period_update), sizeof totalview_ipo_quoting_period_update);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_ipo_quoting_period_update), sizeof totalview_ipo_quoting_period_update);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_ipo_quoting_period_update), sizeof totalview_ipo_quoting_period_update);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'W') {
                auto totalview_ipo_postponed = sim::to_totalview_ipo_postponed(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_ipo_postponed), sizeof totalview_ipo_postponed);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_ipo_postponed), sizeof totalview_ipo_postponed);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_ipo_postponed), sizeof totalview_ipo_postponed);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'W' && s.release.get().value() != 0) {
                auto totalview_ipo_postponed_release = sim::to_totalview_ipo_postponed_release(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_ipo_postponed_release), sizeof totalview_ipo_postponed_release);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_ipo_postponed_release), sizeof totalview_ipo_postponed_release);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_ipo_postponed_release), sizeof totalview_ipo_postponed_release);
            }
            if (s.reason.get_trimmed().value() != std::string_view{"OPER"} && s.reason.get_trimmed().value() != std::string_view{"LUDP"} && (static_cast<char>(s.announcement.get().value()) == 'H' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                auto totalview_stock_trading_action = sim::to_totalview_stock_trading_action(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_stock_trading_action), sizeof totalview_stock_trading_action);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_stock_trading_action), sizeof totalview_stock_trading_action);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_stock_trading_action), sizeof totalview_stock_trading_action);
            }
            if (s.reason.get_trimmed().value() == std::string_view{"OPER"} && (static_cast<char>(s.announcement.get().value()) == 'H' || static_cast<char>(s.announcement.get().value()) == 'T')) {
                auto totalview_operational_halt = sim::to_totalview_operational_halt(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_operational_halt), sizeof totalview_operational_halt);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_operational_halt), sizeof totalview_operational_halt);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_operational_halt), sizeof totalview_operational_halt);
            }
            if (s.reason.get_trimmed().value() == std::string_view{"LUDP"}) {
                auto totalview_luld_pause = sim::to_totalview_luld_pause(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_luld_pause), sizeof totalview_luld_pause);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_luld_pause), sizeof totalview_luld_pause);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_luld_pause), sizeof totalview_luld_pause);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'q' && s.reason.get_trimmed().value() != std::string_view{"OPER"}) {
                auto totalview_quotation = sim::to_totalview_quotation(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_quotation), sizeof totalview_quotation);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_quotation), sizeof totalview_quotation);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_quotation), sizeof totalview_quotation);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'q' && s.reason.get_trimmed().value() != std::string_view{"IPOQ"}) {
                auto totalview_quotation_collar = sim::to_totalview_quotation_collar(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_quotation_collar), sizeof totalview_quotation_collar);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_quotation_collar), sizeof totalview_quotation_collar);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_quotation_collar), sizeof totalview_quotation_collar);
            }
            if (s.reason.get_trimmed().value() == std::string_view{"LUDP"}) {
                auto totalview_luld_collar = sim::to_totalview_luld_collar(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_luld_collar), sizeof totalview_luld_collar);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_luld_collar), sizeof totalview_luld_collar);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_luld_collar), sizeof totalview_luld_collar);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'X') {
                auto totalview_luld_collar_extended = sim::to_totalview_luld_collar_extended(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_luld_collar_extended), sizeof totalview_luld_collar_extended);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_luld_collar_extended), sizeof totalview_luld_collar_extended);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_luld_collar_extended), sizeof totalview_luld_collar_extended);
            }
            if (s.reason.get_trimmed().value() == std::string_view{"MWC1"} && s.symbol.get_trimmed().value() == "ZVZZT") {
                auto totalview_mwcb_status_1 = sim::to_totalview_mwcb_status_1(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_mwcb_status_1), sizeof totalview_mwcb_status_1);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_mwcb_status_1), sizeof totalview_mwcb_status_1);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_mwcb_status_1), sizeof totalview_mwcb_status_1);
            }
            if (s.reason.get_trimmed().value() == std::string_view{"MWC2"} && s.symbol.get_trimmed().value() == "ZVZZT") {
                auto totalview_mwcb_status_2 = sim::to_totalview_mwcb_status_2(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_mwcb_status_2), sizeof totalview_mwcb_status_2);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_mwcb_status_2), sizeof totalview_mwcb_status_2);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_mwcb_status_2), sizeof totalview_mwcb_status_2);
            }
            if (s.reason.get_trimmed().value() == std::string_view{"MWC3"} && s.symbol.get_trimmed().value() == "ZVZZT") {
                auto totalview_mwcb_status_3 = sim::to_totalview_mwcb_status_3(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_mwcb_status_3), sizeof totalview_mwcb_status_3);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_mwcb_status_3), sizeof totalview_mwcb_status_3);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_mwcb_status_3), sizeof totalview_mwcb_status_3);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'S' && s.symbol.get_trimmed().value() == "ZVZZT") {
                auto totalview_mwcb_decline_level = sim::to_totalview_mwcb_decline_level(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_mwcb_decline_level), sizeof totalview_mwcb_decline_level);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_mwcb_decline_level), sizeof totalview_mwcb_decline_level);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_mwcb_decline_level), sizeof totalview_mwcb_decline_level);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'O') {
                auto totalview_stock_directory = sim::to_totalview_stock_directory(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_stock_directory), sizeof totalview_stock_directory);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_stock_directory), sizeof totalview_stock_directory);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_stock_directory), sizeof totalview_stock_directory);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'O') {
                auto totalview_trading_action_spin = sim::to_totalview_trading_action_spin(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_trading_action_spin), sizeof totalview_trading_action_spin);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_trading_action_spin), sizeof totalview_trading_action_spin);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_trading_action_spin), sizeof totalview_trading_action_spin);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'O') {
                auto totalview_reg_sho_spin = sim::to_totalview_reg_sho_spin(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_reg_sho_spin), sizeof totalview_reg_sho_spin);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_reg_sho_spin), sizeof totalview_reg_sho_spin);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_reg_sho_spin), sizeof totalview_reg_sho_spin);
            }
            if (static_cast<char>(s.announcement.get().value()) == 'O') {
                auto totalview_market_participant_spin = sim::to_totalview_market_participant_spin(s);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_market_participant_spin), sizeof totalview_market_participant_spin);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_market_participant_spin), sizeof totalview_market_participant_spin);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_market_participant_spin), sizeof totalview_market_participant_spin);
            }
            totalview.flush();
            if (static_cast<char>(s.announcement.get().value()) == 'C' && s.symbol.get_trimmed().value() == "ZXZZT") totalview.end_session();
        },
        [&](const sim::ipc_ns::cross_completed_message& q) {
            // cross_completed_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(q.cross.get().value()) != 'I') {
                auto totalview_cross_trade = sim::to_totalview_cross_trade(q);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_cross_trade), sizeof totalview_cross_trade);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_cross_trade), sizeof totalview_cross_trade);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_cross_trade), sizeof totalview_cross_trade);
            }
            if (static_cast<char>(q.cross.get().value()) == 'I' && q.quantity.get().value() != 0) {
                auto totalview_halt_cross_trade = sim::to_totalview_halt_cross_trade(q);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_halt_cross_trade), sizeof totalview_halt_cross_trade);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_halt_cross_trade), sizeof totalview_halt_cross_trade);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_halt_cross_trade), sizeof totalview_halt_cross_trade);
            }
            totalview.flush();
        },
        [&](const sim::ipc_ns::imbalance_indicated_message& i) {
            // imbalance_indicated_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(i.imbalance_direction.get().value()) != 'O') {
                auto totalview_noii = sim::to_totalview_noii(i);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_noii), sizeof totalview_noii);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_noii), sizeof totalview_noii);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_noii), sizeof totalview_noii);
            }
            if (static_cast<char>(i.imbalance_direction.get().value()) == 'O') {
                auto totalview_noii_insufficient = sim::to_totalview_noii_insufficient(i);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_noii_insufficient), sizeof totalview_noii_insufficient);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_noii_insufficient), sizeof totalview_noii_insufficient);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_noii_insufficient), sizeof totalview_noii_insufficient);
            }
            totalview.flush();
        },
        [&](const sim::ipc_ns::order_replaced_message& w) {
            // order_replaced_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(w.display.get().value()) == 'Y' && w.quantity.get().value() != 0) {
                auto totalview_order_replace = sim::to_totalview_order_replace(w);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_replace), sizeof totalview_order_replace);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_replace), sizeof totalview_order_replace);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_replace), sizeof totalview_order_replace);
            }
            if (w.quantity.get().value() == 0 && static_cast<char>(w.display.get().value()) != 'N') {
                auto totalview_order_replaced_away = sim::to_totalview_order_replaced_away(w);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_replaced_away), sizeof totalview_order_replaced_away);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_replaced_away), sizeof totalview_order_replaced_away);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_replaced_away), sizeof totalview_order_replaced_away);
            }
            totalview.flush();
        },
        [&](const sim::ipc_ns::order_reduced_message& y) {
            // order_reduced_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            if (static_cast<char>(y.display.get().value()) == 'Y') {
                auto totalview_order_cancel = sim::to_totalview_order_cancel(y);
                totalview.publish_message(reinterpret_cast<const char*>(&totalview_order_cancel), sizeof totalview_order_cancel);
                sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_order_cancel), sizeof totalview_order_cancel);
                sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_order_cancel), sizeof totalview_order_cancel);
            }
            totalview.flush();
        },
        [](const sim::ipc_ns::cancel_pending_message&) {},
        [](const sim::ipc_ns::reduce_refused_message&) {},
        [&](const sim::ipc_ns::trade_broken_message& x) {
            // trade_broken_message -> the feeds its transforms declare (NsmEquities.Ipc.Transforms.xml).
            auto totalview_broken_trade = sim::to_totalview_broken_trade(x);
            totalview.publish_message(reinterpret_cast<const char*>(&totalview_broken_trade), sizeof totalview_broken_trade);
            sim::write_blocking(to_glimpse, reinterpret_cast<const char*>(&totalview_broken_trade), sizeof totalview_broken_trade);
            sim::write_blocking(to_totalviewtcp, reinterpret_cast<const char*>(&totalview_broken_trade), sizeof totalview_broken_trade);
            totalview.flush();
        });

    // MakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaymakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaymakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaymakerbasic_emit)> gatewaymakerbasic(gatewaymakerbasic_emit, 1, 8);
    gatewaymakerbasic.allow("MAKER1", "maker10000");
    gatewaymakerbasic.allow("TAKER1", "taker10000");
    gatewaymakerbasic.allow("REPLAY", "replay0000");
    gatewaymakerbasic.allow("ASKER1", "asker10000");
    gatewaymakerbasic.allow("MAKER2", "maker20000");
    gatewaymakerbasic.allow("TAKER2", "taker20000");
    gatewaymakerbasic.allow("FIXMKR", "fixmkr0000");
    gatewaymakerbasic.allow("FIXTKR", "fixtkr0000");
    gatewaymakerbasic.allow("RISKST", "riskst0000");
    gatewaymakerbasic.allow("RSTRCT", "rstrct0000");
    gatewaymakerbasic.allow("RATEDP", "ratedp0000");
    gatewaymakerbasic.allow("KILLSW", "killsw0000");
    gatewaymakerbasic.allow("GROSSX", "grossx0000");
    gatewaymakerbasic.allow("FATFNG", "fatfng0000");
    gatewaymakerbasic.allow("OUCETC", "oucetc0000");
    gatewaymakerbasic.allow("HALTCX", "haltcx0000");
    gatewaymakerbasic.allow("MELOBY", "meloby0000");
    gatewaymakerbasic.allow("MELOSL", "melosl0000");
    gatewaymakerbasic.allow("MELOPG", "melopg0000");
    gatewaymakerbasic.allow("FIXETC", "fixetc0000");
    gatewaymakerbasic.allow("FIXOPT", "fixopt0000");
    gatewaymakerbasic.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaymakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaymakerbasic.on_event(t); },
        [&](const sim::ipc_ns::order_rested_message& o) { gatewaymakerbasic.on_event(o); },
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaymakerbasic.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaymakerbasic.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaymakerbasic.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewaymakerbasic.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewaymakerbasic.on_event(w); },
        [&](const sim::ipc_ns::order_reduced_message& y) { gatewaymakerbasic.on_event(y); },
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewaymakerbasic.on_event(g); },
        [&](const sim::ipc_ns::reduce_refused_message& z) { gatewaymakerbasic.on_event(z); },
        [&](const sim::ipc_ns::trade_broken_message& x) { gatewaymakerbasic.on_event(x); });

    // TakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaytakerbasic_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaytakerbasic, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaytakerbasic_emit)> gatewaytakerbasic(gatewaytakerbasic_emit, 2, 8);
    gatewaytakerbasic.allow("MAKER1", "maker10000");
    gatewaytakerbasic.allow("TAKER1", "taker10000");
    gatewaytakerbasic.allow("REPLAY", "replay0000");
    gatewaytakerbasic.allow("ASKER1", "asker10000");
    gatewaytakerbasic.allow("MAKER2", "maker20000");
    gatewaytakerbasic.allow("TAKER2", "taker20000");
    gatewaytakerbasic.allow("FIXMKR", "fixmkr0000");
    gatewaytakerbasic.allow("FIXTKR", "fixtkr0000");
    gatewaytakerbasic.allow("RISKST", "riskst0000");
    gatewaytakerbasic.allow("RSTRCT", "rstrct0000");
    gatewaytakerbasic.allow("RATEDP", "ratedp0000");
    gatewaytakerbasic.allow("KILLSW", "killsw0000");
    gatewaytakerbasic.allow("GROSSX", "grossx0000");
    gatewaytakerbasic.allow("FATFNG", "fatfng0000");
    gatewaytakerbasic.allow("OUCETC", "oucetc0000");
    gatewaytakerbasic.allow("HALTCX", "haltcx0000");
    gatewaytakerbasic.allow("MELOBY", "meloby0000");
    gatewaytakerbasic.allow("MELOSL", "melosl0000");
    gatewaytakerbasic.allow("MELOPG", "melopg0000");
    gatewaytakerbasic.allow("FIXETC", "fixetc0000");
    gatewaytakerbasic.allow("FIXOPT", "fixopt0000");
    gatewaytakerbasic.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaytakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaytakerbasic.on_event(t); },
        [&](const sim::ipc_ns::order_rested_message& o) { gatewaytakerbasic.on_event(o); },
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaytakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaytakerbasic.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaytakerbasic.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaytakerbasic.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewaytakerbasic.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewaytakerbasic.on_event(w); },
        [&](const sim::ipc_ns::order_reduced_message& y) { gatewaytakerbasic.on_event(y); },
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewaytakerbasic.on_event(g); },
        [&](const sim::ipc_ns::reduce_refused_message& z) { gatewaytakerbasic.on_event(z); },
        [&](const sim::ipc_ns::trade_broken_message& x) { gatewaytakerbasic.on_event(x); });

    // Replay's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewayreplay_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewayreplay, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewayreplay_emit)> gatewayreplay(gatewayreplay_emit, 3, 8);
    gatewayreplay.allow("MAKER1", "maker10000");
    gatewayreplay.allow("TAKER1", "taker10000");
    gatewayreplay.allow("REPLAY", "replay0000");
    gatewayreplay.allow("ASKER1", "asker10000");
    gatewayreplay.allow("MAKER2", "maker20000");
    gatewayreplay.allow("TAKER2", "taker20000");
    gatewayreplay.allow("FIXMKR", "fixmkr0000");
    gatewayreplay.allow("FIXTKR", "fixtkr0000");
    gatewayreplay.allow("RISKST", "riskst0000");
    gatewayreplay.allow("RSTRCT", "rstrct0000");
    gatewayreplay.allow("RATEDP", "ratedp0000");
    gatewayreplay.allow("KILLSW", "killsw0000");
    gatewayreplay.allow("GROSSX", "grossx0000");
    gatewayreplay.allow("FATFNG", "fatfng0000");
    gatewayreplay.allow("OUCETC", "oucetc0000");
    gatewayreplay.allow("HALTCX", "haltcx0000");
    gatewayreplay.allow("MELOBY", "meloby0000");
    gatewayreplay.allow("MELOSL", "melosl0000");
    gatewayreplay.allow("MELOPG", "melopg0000");
    gatewayreplay.allow("FIXETC", "fixetc0000");
    gatewayreplay.allow("FIXOPT", "fixopt0000");
    gatewayreplay.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewayreplay_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewayreplay.on_event(t); },
        [&](const sim::ipc_ns::order_rested_message& o) { gatewayreplay.on_event(o); },
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewayreplay.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewayreplay.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayreplay.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewayreplay.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewayreplay.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewayreplay.on_event(w); },
        [&](const sim::ipc_ns::order_reduced_message& y) { gatewayreplay.on_event(y); },
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewayreplay.on_event(g); },
        [&](const sim::ipc_ns::reduce_refused_message& z) { gatewayreplay.on_event(z); },
        [&](const sim::ipc_ns::trade_broken_message& x) { gatewayreplay.on_event(x); });

    // Requests's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewayrequests_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewayrequests, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewayrequests_emit)> gatewayrequests(gatewayrequests_emit, 4, 8);
    gatewayrequests.allow("MAKER1", "maker10000");
    gatewayrequests.allow("TAKER1", "taker10000");
    gatewayrequests.allow("REPLAY", "replay0000");
    gatewayrequests.allow("ASKER1", "asker10000");
    gatewayrequests.allow("MAKER2", "maker20000");
    gatewayrequests.allow("TAKER2", "taker20000");
    gatewayrequests.allow("FIXMKR", "fixmkr0000");
    gatewayrequests.allow("FIXTKR", "fixtkr0000");
    gatewayrequests.allow("RISKST", "riskst0000");
    gatewayrequests.allow("RSTRCT", "rstrct0000");
    gatewayrequests.allow("RATEDP", "ratedp0000");
    gatewayrequests.allow("KILLSW", "killsw0000");
    gatewayrequests.allow("GROSSX", "grossx0000");
    gatewayrequests.allow("FATFNG", "fatfng0000");
    gatewayrequests.allow("OUCETC", "oucetc0000");
    gatewayrequests.allow("HALTCX", "haltcx0000");
    gatewayrequests.allow("MELOBY", "meloby0000");
    gatewayrequests.allow("MELOSL", "melosl0000");
    gatewayrequests.allow("MELOPG", "melopg0000");
    gatewayrequests.allow("FIXETC", "fixetc0000");
    gatewayrequests.allow("FIXOPT", "fixopt0000");
    gatewayrequests.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewayrequests_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewayrequests.on_event(t); },
        [&](const sim::ipc_ns::order_rested_message& o) { gatewayrequests.on_event(o); },
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewayrequests.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewayrequests.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayrequests.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewayrequests.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewayrequests.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewayrequests.on_event(w); },
        [&](const sim::ipc_ns::order_reduced_message& y) { gatewayrequests.on_event(y); },
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewayrequests.on_event(g); },
        [&](const sim::ipc_ns::reduce_refused_message& z) { gatewayrequests.on_event(z); },
        [&](const sim::ipc_ns::trade_broken_message& x) { gatewayrequests.on_event(x); });

    // MakerSecond's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaymakersecond_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaymakersecond, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaymakersecond_emit)> gatewaymakersecond(gatewaymakersecond_emit, 5, 8);
    gatewaymakersecond.allow("MAKER1", "maker10000");
    gatewaymakersecond.allow("TAKER1", "taker10000");
    gatewaymakersecond.allow("REPLAY", "replay0000");
    gatewaymakersecond.allow("ASKER1", "asker10000");
    gatewaymakersecond.allow("MAKER2", "maker20000");
    gatewaymakersecond.allow("TAKER2", "taker20000");
    gatewaymakersecond.allow("FIXMKR", "fixmkr0000");
    gatewaymakersecond.allow("FIXTKR", "fixtkr0000");
    gatewaymakersecond.allow("RISKST", "riskst0000");
    gatewaymakersecond.allow("RSTRCT", "rstrct0000");
    gatewaymakersecond.allow("RATEDP", "ratedp0000");
    gatewaymakersecond.allow("KILLSW", "killsw0000");
    gatewaymakersecond.allow("GROSSX", "grossx0000");
    gatewaymakersecond.allow("FATFNG", "fatfng0000");
    gatewaymakersecond.allow("OUCETC", "oucetc0000");
    gatewaymakersecond.allow("HALTCX", "haltcx0000");
    gatewaymakersecond.allow("MELOBY", "meloby0000");
    gatewaymakersecond.allow("MELOSL", "melosl0000");
    gatewaymakersecond.allow("MELOPG", "melopg0000");
    gatewaymakersecond.allow("FIXETC", "fixetc0000");
    gatewaymakersecond.allow("FIXOPT", "fixopt0000");
    gatewaymakersecond.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaymakersecond_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaymakersecond.on_event(t); },
        [&](const sim::ipc_ns::order_rested_message& o) { gatewaymakersecond.on_event(o); },
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakersecond.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaymakersecond.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaymakersecond.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaymakersecond.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewaymakersecond.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewaymakersecond.on_event(w); },
        [&](const sim::ipc_ns::order_reduced_message& y) { gatewaymakersecond.on_event(y); },
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewaymakersecond.on_event(g); },
        [&](const sim::ipc_ns::reduce_refused_message& z) { gatewaymakersecond.on_event(z); },
        [&](const sim::ipc_ns::trade_broken_message& x) { gatewaymakersecond.on_event(x); });

    // TakerSecond's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewaytakersecond_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewaytakersecond, b, n);
    };
    sim::orderentry::Gateway<decltype(gatewaytakersecond_emit)> gatewaytakersecond(gatewaytakersecond_emit, 6, 8);
    gatewaytakersecond.allow("MAKER1", "maker10000");
    gatewaytakersecond.allow("TAKER1", "taker10000");
    gatewaytakersecond.allow("REPLAY", "replay0000");
    gatewaytakersecond.allow("ASKER1", "asker10000");
    gatewaytakersecond.allow("MAKER2", "maker20000");
    gatewaytakersecond.allow("TAKER2", "taker20000");
    gatewaytakersecond.allow("FIXMKR", "fixmkr0000");
    gatewaytakersecond.allow("FIXTKR", "fixtkr0000");
    gatewaytakersecond.allow("RISKST", "riskst0000");
    gatewaytakersecond.allow("RSTRCT", "rstrct0000");
    gatewaytakersecond.allow("RATEDP", "ratedp0000");
    gatewaytakersecond.allow("KILLSW", "killsw0000");
    gatewaytakersecond.allow("GROSSX", "grossx0000");
    gatewaytakersecond.allow("FATFNG", "fatfng0000");
    gatewaytakersecond.allow("OUCETC", "oucetc0000");
    gatewaytakersecond.allow("HALTCX", "haltcx0000");
    gatewaytakersecond.allow("MELOBY", "meloby0000");
    gatewaytakersecond.allow("MELOSL", "melosl0000");
    gatewaytakersecond.allow("MELOPG", "melopg0000");
    gatewaytakersecond.allow("FIXETC", "fixetc0000");
    gatewaytakersecond.allow("FIXOPT", "fixopt0000");
    gatewaytakersecond.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewaytakersecond_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaytakersecond.on_event(t); },
        [&](const sim::ipc_ns::order_rested_message& o) { gatewaytakersecond.on_event(o); },
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaytakersecond.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaytakersecond.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaytakersecond.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewaytakersecond.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewaytakersecond.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewaytakersecond.on_event(w); },
        [&](const sim::ipc_ns::order_reduced_message& y) { gatewaytakersecond.on_event(y); },
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewaytakersecond.on_event(g); },
        [&](const sim::ipc_ns::reduce_refused_message& z) { gatewaytakersecond.on_event(z); },
        [&](const sim::ipc_ns::trade_broken_message& x) { gatewaytakersecond.on_event(x); });

    // FixMaker's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewayfixmaker_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewayfixmaker, b, n);
    };
    sim::orderentry::fix::Gateway<decltype(gatewayfixmaker_emit)> gatewayfixmaker(gatewayfixmaker_emit, 7, 8);
    gatewayfixmaker.allow("MAKER1", "maker10000");
    gatewayfixmaker.allow("TAKER1", "taker10000");
    gatewayfixmaker.allow("REPLAY", "replay0000");
    gatewayfixmaker.allow("ASKER1", "asker10000");
    gatewayfixmaker.allow("MAKER2", "maker20000");
    gatewayfixmaker.allow("TAKER2", "taker20000");
    gatewayfixmaker.allow("FIXMKR", "fixmkr0000");
    gatewayfixmaker.allow("FIXTKR", "fixtkr0000");
    gatewayfixmaker.allow("RISKST", "riskst0000");
    gatewayfixmaker.allow("RSTRCT", "rstrct0000");
    gatewayfixmaker.allow("RATEDP", "ratedp0000");
    gatewayfixmaker.allow("KILLSW", "killsw0000");
    gatewayfixmaker.allow("GROSSX", "grossx0000");
    gatewayfixmaker.allow("FATFNG", "fatfng0000");
    gatewayfixmaker.allow("OUCETC", "oucetc0000");
    gatewayfixmaker.allow("HALTCX", "haltcx0000");
    gatewayfixmaker.allow("MELOBY", "meloby0000");
    gatewayfixmaker.allow("MELOSL", "melosl0000");
    gatewayfixmaker.allow("MELOPG", "melopg0000");
    gatewayfixmaker.allow("FIXETC", "fixetc0000");
    gatewayfixmaker.allow("FIXOPT", "fixopt0000");
    gatewayfixmaker.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewayfixmaker_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewayfixmaker.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewayfixmaker.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewayfixmaker.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayfixmaker.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewayfixmaker.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewayfixmaker.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewayfixmaker.on_event(w); },
        [](const sim::ipc_ns::order_reduced_message&) {},
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewayfixmaker.on_event(g); },
        [](const sim::ipc_ns::reduce_refused_message&) {},
        [](const sim::ipc_ns::trade_broken_message&) {});

    // FixTaker's gateway: each client's session reaches the engine as framed Ipc messages.
    auto gatewayfixtaker_emit = [&](const char* b, std::size_t n) {
        ipc_msgs.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_sequencer_gatewayfixtaker, b, n);
    };
    sim::orderentry::fix::Gateway<decltype(gatewayfixtaker_emit)> gatewayfixtaker(gatewayfixtaker_emit, 8, 8);
    gatewayfixtaker.allow("MAKER1", "maker10000");
    gatewayfixtaker.allow("TAKER1", "taker10000");
    gatewayfixtaker.allow("REPLAY", "replay0000");
    gatewayfixtaker.allow("ASKER1", "asker10000");
    gatewayfixtaker.allow("MAKER2", "maker20000");
    gatewayfixtaker.allow("TAKER2", "taker20000");
    gatewayfixtaker.allow("FIXMKR", "fixmkr0000");
    gatewayfixtaker.allow("FIXTKR", "fixtkr0000");
    gatewayfixtaker.allow("RISKST", "riskst0000");
    gatewayfixtaker.allow("RSTRCT", "rstrct0000");
    gatewayfixtaker.allow("RATEDP", "ratedp0000");
    gatewayfixtaker.allow("KILLSW", "killsw0000");
    gatewayfixtaker.allow("GROSSX", "grossx0000");
    gatewayfixtaker.allow("FATFNG", "fatfng0000");
    gatewayfixtaker.allow("OUCETC", "oucetc0000");
    gatewayfixtaker.allow("HALTCX", "haltcx0000");
    gatewayfixtaker.allow("MELOBY", "meloby0000");
    gatewayfixtaker.allow("MELOSL", "melosl0000");
    gatewayfixtaker.allow("MELOPG", "melopg0000");
    gatewayfixtaker.allow("FIXETC", "fixetc0000");
    gatewayfixtaker.allow("FIXOPT", "fixopt0000");
    gatewayfixtaker.allow("FIXRSK", "fixrsk0000");

    // The engine events the gateway answers, on the gateway's own thread.
    sim::IpcEventServer gatewayfixtaker_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewayfixtaker.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewayfixtaker.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewayfixtaker.on_event(a); },
        [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayfixtaker.on_event(e); },
        [&](const sim::ipc_ns::order_refused_message& j) { gatewayfixtaker.on_event(j); },
        [&](const sim::ipc_ns::phase_announced_message& s) { gatewayfixtaker.on_event(s); },
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {},
        [&](const sim::ipc_ns::order_replaced_message& w) { gatewayfixtaker.on_event(w); },
        [](const sim::ipc_ns::order_reduced_message&) {},
        [&](const sim::ipc_ns::cancel_pending_message& g) { gatewayfixtaker.on_event(g); },
        [](const sim::ipc_ns::reduce_refused_message&) {},
        [](const sim::ipc_ns::trade_broken_message&) {});

    // The engine's events, framed onto every consumer's queue.
    auto event_emit = [&](const char* b, std::size_t n) {
        ipc_events.fetch_add(1, std::memory_order_relaxed);
        sim::write_blocking(to_clients_gatewaymakerbasic, b, n);
        sim::write_blocking(to_clients_gatewaytakerbasic, b, n);
        sim::write_blocking(to_clients_gatewayreplay, b, n);
        sim::write_blocking(to_clients_gatewayrequests, b, n);
        sim::write_blocking(to_clients_gatewaymakersecond, b, n);
        sim::write_blocking(to_clients_gatewaytakersecond, b, n);
        sim::write_blocking(to_clients_gatewayfixmaker, b, n);
        sim::write_blocking(to_clients_gatewayfixtaker, b, n);
        sim::write_blocking(to_totalview, b, n);
    };
    // The engine component, built the same way the per-component binaries build it:
    // what it matches is its own, and where its events go is whatever emit it was handed.
    sim::components::Engine<decltype(event_emit), sim::instruments> engine_component(event_emit, trades);
    engine_component.dump(cfg.book_dump);
    // The trading day: it asks the engine for each change of phase over the Ipc, on a link of
    // its own. A served venue asks on the clock, at the seconds its simulator.ini gives; the
    // startup run asks between its turns.
    auto day_emit = [&](const char* b, std::size_t n) { sim::write_blocking(from_day, b, n); };
    sim::components::Day<decltype(day_emit)> day_component(day_emit);
    day_component.books(cfg.book_every == "phase");   // whether it asks for the engine's book after each change of phase
    { char* rest = nullptr; const long long each = std::strtoll(cfg.book_every.c_str(), &rest, 10); if (rest != cfg.book_every.c_str() && *rest == '\0') day_component.books_each(each); }   // or every so many milliseconds
    day_component.every(cfg.clock_tick_milliseconds);   // how often the venue's time is sent its engine
    if (serve) day_component.serve(cfg.day_at);
    if (serve) day_component.supervise(cfg.supervisor_port);   // a supervisor's requests, where the settings give a port
    // Each engine's sequencer: what is bound for the engine, from every producer, put in one
    // order, each message stamped with its place in it and the time, and handed to the engine.
    auto sequencer_emit = [&](const char* b, std::size_t n) { sim::write_blocking(sequenced, b, n); };
    sim::components::Sequencer<decltype(sequencer_emit)> sequencer_component(sequencer_emit);

    // Traders: each trades through the venue's own order entry, as a client session like any
    // other — its bytes reach the gateway over a queue instead of a socket.
    sim::MessageQueue to_makerbasic("gatewaymakerbasic -> makerbasic");
    sim::MessageQueue from_makerbasic("makerbasic -> gatewaymakerbasic");
    auto makerbasic_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_makerbasic, b, n); };
    sim::traders::Trader<decltype(makerbasic_send)> makerbasic("MakerBasic", "MAKER1", "maker10000", makerbasic_send);
    makerbasic.orders().trade(sim::instruments[0]);
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
    sim::traders::Trader<decltype(takerbasic_send)> takerbasic("TakerBasic", "TAKER1", "taker10000", takerbasic_send);
    takerbasic.orders().trade(sim::instruments[0]);
    auto takerbasic_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_takerbasic, b, n); };
    std::size_t takerbasic_client = gatewaytakerbasic.connect(takerbasic_reply);
    sim::traders::TakerBasic<std::remove_reference_t<decltype(takerbasic.orders())>> takerbasic_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int takerbasic_ticks = 0;
    std::chrono::steady_clock::time_point takerbasic_paced{};   // when a served trader next orders
    sim::Turn takerbasic_turn;
    takerbasic.login();
    sim::MessageQueue to_replay("gatewayreplay -> replay");
    sim::MessageQueue from_replay("replay -> gatewayreplay");
    auto replay_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_replay, b, n); };
    sim::traders::Trader<decltype(replay_send)> replay("Replay", "REPLAY", "replay0000", replay_send);
    replay.orders().trade(sim::instruments[0]);
    auto replay_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_replay, b, n); };
    std::size_t replay_client = gatewayreplay.connect(replay_reply);
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
    sim::traders::Trader<decltype(requests_send)> requests("Requests", "ASKER1", "asker10000", requests_send);
    requests.orders().trade(sim::instruments[0]);
    auto requests_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_requests, b, n); };
    std::size_t requests_client = gatewayrequests.connect(requests_reply);
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
    sim::MessageQueue to_makersecond("gatewaymakersecond -> makersecond");
    sim::MessageQueue from_makersecond("makersecond -> gatewaymakersecond");
    auto makersecond_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_makersecond, b, n); };
    sim::traders::Trader<decltype(makersecond_send)> makersecond("MakerSecond", "MAKER2", "maker20000", makersecond_send);
    makersecond.orders().trade(sim::instruments[1]);
    auto makersecond_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_makersecond, b, n); };
    std::size_t makersecond_client = gatewaymakersecond.connect(makersecond_reply);
    sim::traders::MakerBasic<std::remove_reference_t<decltype(makersecond.orders())>> makersecond_strategy(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3, /*tick*/1);
    int makersecond_ticks = 0;
    std::chrono::steady_clock::time_point makersecond_paced{};   // when a served trader next orders
    sim::Turn makersecond_turn;
    makersecond.login();
    sim::MessageQueue to_takersecond("gatewaytakersecond -> takersecond");
    sim::MessageQueue from_takersecond("takersecond -> gatewaytakersecond");
    auto takersecond_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_takersecond, b, n); };
    sim::traders::Trader<decltype(takersecond_send)> takersecond("TakerSecond", "TAKER2", "taker20000", takersecond_send);
    takersecond.orders().trade(sim::instruments[1]);
    auto takersecond_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_takersecond, b, n); };
    std::size_t takersecond_client = gatewaytakersecond.connect(takersecond_reply);
    sim::traders::TakerBasic<std::remove_reference_t<decltype(takersecond.orders())>> takersecond_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/3);
    int takersecond_ticks = 0;
    std::chrono::steady_clock::time_point takersecond_paced{};   // when a served trader next orders
    sim::Turn takersecond_turn;
    takersecond.login();
    sim::MessageQueue to_fixmaker("gatewayfixmaker -> fixmaker");
    sim::MessageQueue from_fixmaker("fixmaker -> gatewayfixmaker");
    auto fixmaker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_fixmaker, b, n); };
    sim::traders::fix::Trader<decltype(fixmaker_send)> fixmaker("FixMaker", "FIXMKR", "fixmkr0000", fixmaker_send);
    fixmaker.orders().trade(sim::instruments[1]);
    auto fixmaker_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_fixmaker, b, n); };
    std::size_t fixmaker_client = gatewayfixmaker.connect(fixmaker_reply);
    sim::traders::MakerBasic<std::remove_reference_t<decltype(fixmaker.orders())>> fixmaker_strategy(/*mid*/10000, /*spread*/40, /*size*/100, /*levels*/3, /*tick*/1);
    int fixmaker_ticks = 0;
    std::chrono::steady_clock::time_point fixmaker_paced{};   // when a served trader next orders
    sim::Turn fixmaker_turn;
    fixmaker.login();
    sim::MessageQueue to_fixtaker("gatewayfixtaker -> fixtaker");
    sim::MessageQueue from_fixtaker("fixtaker -> gatewayfixtaker");
    auto fixtaker_send = [&](const char* b, std::size_t n) { sim::write_blocking(from_fixtaker, b, n); };
    sim::traders::fix::Trader<decltype(fixtaker_send)> fixtaker("FixTaker", "FIXTKR", "fixtkr0000", fixtaker_send);
    fixtaker.orders().trade(sim::instruments[1]);
    auto fixtaker_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_fixtaker, b, n); };
    std::size_t fixtaker_client = gatewayfixtaker.connect(fixtaker_reply);
    sim::traders::TakerBasic<std::remove_reference_t<decltype(fixtaker.orders())>> fixtaker_strategy(/*mid*/10000, /*cross*/20, /*size*/50, /*period*/9);
    int fixtaker_ticks = 0;
    std::chrono::steady_clock::time_point fixtaker_paced{};   // when a served trader next orders
    sim::Turn fixtaker_turn;
    fixtaker.login();

    // Each listening socket belongs to its gateway's thread; --serve is what opens them.
    std::unique_ptr<net::TcpServer> gatewaymakerbasic_order_entry;
    std::map<int, std::size_t> gatewaymakerbasic_sessions;
    std::unique_ptr<net::TcpServer> gatewaytakerbasic_order_entry;
    std::map<int, std::size_t> gatewaytakerbasic_sessions;
    std::unique_ptr<net::TcpServer> gatewayreplay_order_entry;
    std::map<int, std::size_t> gatewayreplay_sessions;
    std::unique_ptr<net::TcpServer> gatewayrequests_order_entry;
    std::map<int, std::size_t> gatewayrequests_sessions;
    std::unique_ptr<net::TcpServer> gatewaymakersecond_order_entry;
    std::map<int, std::size_t> gatewaymakersecond_sessions;
    std::unique_ptr<net::TcpServer> gatewaytakersecond_order_entry;
    std::map<int, std::size_t> gatewaytakersecond_sessions;
    std::unique_ptr<net::TcpServer> gatewayfixmaker_order_entry;
    std::map<int, std::size_t> gatewayfixmaker_sessions;
    std::unique_ptr<net::TcpServer> gatewayfixtaker_order_entry;
    std::map<int, std::size_t> gatewayfixtaker_sessions;
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
        gatewaymakersecond_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaymakersecond_port);
        gatewaytakersecond_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewaytakersecond_port);
        gatewayfixmaker_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewayfixmaker_port);
        gatewayfixtaker_order_entry = std::make_unique<net::TcpServer>(cfg.order_entry.host, cfg.gatewayfixtaker_port);
        glimpse_server = std::make_unique<net::TcpServer>(cfg.glimpse_replay_host, cfg.glimpse_replay_port);
        totalviewtcp_server = std::make_unique<net::TcpServer>(cfg.totalviewtcp_replay_host, cfg.totalviewtcp_replay_port);
    }

    // One thread per component. Each owns its own state: the gateway its client sockets,
    // the engine its book, each feed its publisher — nothing is shared but the queues.
    sim::Component engine_thread("engine", [&] { return engine_component.step(sequenced); });
    sim::Component sequencer_thread("sequencer", [&] { return sequencer_component.step(to_sequencer_gatewaymakerbasic, to_sequencer_gatewaytakerbasic, to_sequencer_gatewayreplay, to_sequencer_gatewayrequests, to_sequencer_gatewaymakersecond, to_sequencer_gatewaytakersecond, to_sequencer_gatewayfixmaker, to_sequencer_gatewayfixtaker, from_day); });
    sim::Component day_thread("day", [&] { return day_component.step(); });
    sim::Component totalview_thread("totalview", [&] {
        const bool worked = to_totalview.read([&](std::string_view message) { if (const auto asked = sim::book_end_of(message)) totalview_rec.mark(*asked); totalview_events.on_message(message); });
        // A feed that has published nothing for a second says its session is alive, or over.
        totalview.idle();
        return worked;
    });
    sim::Component glimpse_thread("glimpse", [&] {
        bool worked = to_glimpse.read([&](std::string_view message) { glimpse.on_message(message.data(), message.size()); });
        if (glimpse_server) worked = glimpse_server->poll([&](int fd, std::string_view bytes) {
            glimpse.on_bytes(glimpse_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = glimpse_sessions.find(fd); gone != glimpse_sessions.end()) {
                glimpse.disconnect(gone->second);
                glimpse_sessions.erase(gone);
            }
        }, [&](int fd) { glimpse_sessions.emplace(fd, glimpse.connect([server = glimpse_server.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
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
    sim::Component totalviewtcp_thread("totalviewtcp", [&] {
        bool worked = to_totalviewtcp.read([&](std::string_view message) { totalviewtcp.on_message(message.data(), message.size()); });
        if (totalviewtcp_server) worked = totalviewtcp_server->poll([&](int fd, std::string_view bytes) {
            totalviewtcp.on_bytes(totalviewtcp_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = totalviewtcp_sessions.find(fd); gone != totalviewtcp_sessions.end()) {
                totalviewtcp.disconnect(gone->second);
                totalviewtcp_sessions.erase(gone);
            }
        }, [&](int fd) { totalviewtcp_sessions.emplace(fd, totalviewtcp.connect([server = totalviewtcp_server.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
        totalviewtcp.tick();
        for (auto session = totalviewtcp_sessions.begin(); session != totalviewtcp_sessions.end(); ) {
            if (!totalviewtcp.ended(session->second)) { ++session; continue; }
            if (totalviewtcp_server) totalviewtcp_server->close(session->first);
            totalviewtcp.disconnect(session->second);
            session = totalviewtcp_sessions.erase(session);
            worked = true;
        }
        return totalviewtcp.pump() || worked;
    });
    sim::Component gatewaymakerbasic_thread("gatewaymakerbasic", [&] {
        bool worked = to_clients_gatewaymakerbasic.read([&](std::string_view message) { gatewaymakerbasic_events.on_message(message); });
        worked = from_makerbasic.read([&](std::string_view bytes) { gatewaymakerbasic.on_bytes(makerbasic_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewaymakerbasic_order_entry) worked = gatewaymakerbasic_order_entry->poll([&](int fd, std::string_view bytes) {
            gatewaymakerbasic.on_bytes(gatewaymakerbasic_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaymakerbasic_sessions.find(fd); gone != gatewaymakerbasic_sessions.end()) {
                gatewaymakerbasic.disconnect(gone->second);
                gatewaymakerbasic_sessions.erase(gone);
            }
        }, [&](int fd) { gatewaymakerbasic_sessions.emplace(fd, gatewaymakerbasic.connect([server = gatewaymakerbasic_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
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
            gatewaytakerbasic.on_bytes(gatewaytakerbasic_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaytakerbasic_sessions.find(fd); gone != gatewaytakerbasic_sessions.end()) {
                gatewaytakerbasic.disconnect(gone->second);
                gatewaytakerbasic_sessions.erase(gone);
            }
        }, [&](int fd) { gatewaytakerbasic_sessions.emplace(fd, gatewaytakerbasic.connect([server = gatewaytakerbasic_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
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
            gatewayreplay.on_bytes(gatewayreplay_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewayreplay_sessions.find(fd); gone != gatewayreplay_sessions.end()) {
                gatewayreplay.disconnect(gone->second);
                gatewayreplay_sessions.erase(gone);
            }
        }, [&](int fd) { gatewayreplay_sessions.emplace(fd, gatewayreplay.connect([server = gatewayreplay_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
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
    sim::Component gatewayrequests_thread("gatewayrequests", [&] {
        bool worked = to_clients_gatewayrequests.read([&](std::string_view message) { gatewayrequests_events.on_message(message); });
        worked = from_requests.read([&](std::string_view bytes) { gatewayrequests.on_bytes(requests_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewayrequests_order_entry) worked = gatewayrequests_order_entry->poll([&](int fd, std::string_view bytes) {
            gatewayrequests.on_bytes(gatewayrequests_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewayrequests_sessions.find(fd); gone != gatewayrequests_sessions.end()) {
                gatewayrequests.disconnect(gone->second);
                gatewayrequests_sessions.erase(gone);
            }
        }, [&](int fd) { gatewayrequests_sessions.emplace(fd, gatewayrequests.connect([server = gatewayrequests_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
        gatewayrequests.tick();
        if (gatewayrequests.ended(requests_client)) {
            gatewayrequests.disconnect(requests_client);
            requests_client = gatewayrequests.connect(requests_reply);
            worked = true;
        }
        for (auto session = gatewayrequests_sessions.begin(); session != gatewayrequests_sessions.end(); ) {
            if (!gatewayrequests.ended(session->second)) { ++session; continue; }
            if (gatewayrequests_order_entry) gatewayrequests_order_entry->close(session->first);
            gatewayrequests.disconnect(session->second);
            session = gatewayrequests_sessions.erase(session);
            worked = true;
        }
        return worked;
    });
    sim::Component gatewaymakersecond_thread("gatewaymakersecond", [&] {
        bool worked = to_clients_gatewaymakersecond.read([&](std::string_view message) { gatewaymakersecond_events.on_message(message); });
        worked = from_makersecond.read([&](std::string_view bytes) { gatewaymakersecond.on_bytes(makersecond_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewaymakersecond_order_entry) worked = gatewaymakersecond_order_entry->poll([&](int fd, std::string_view bytes) {
            gatewaymakersecond.on_bytes(gatewaymakersecond_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaymakersecond_sessions.find(fd); gone != gatewaymakersecond_sessions.end()) {
                gatewaymakersecond.disconnect(gone->second);
                gatewaymakersecond_sessions.erase(gone);
            }
        }, [&](int fd) { gatewaymakersecond_sessions.emplace(fd, gatewaymakersecond.connect([server = gatewaymakersecond_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
        gatewaymakersecond.tick();
        if (gatewaymakersecond.ended(makersecond_client)) {
            gatewaymakersecond.disconnect(makersecond_client);
            makersecond_client = gatewaymakersecond.connect(makersecond_reply);
            worked = true;
        }
        for (auto session = gatewaymakersecond_sessions.begin(); session != gatewaymakersecond_sessions.end(); ) {
            if (!gatewaymakersecond.ended(session->second)) { ++session; continue; }
            if (gatewaymakersecond_order_entry) gatewaymakersecond_order_entry->close(session->first);
            gatewaymakersecond.disconnect(session->second);
            session = gatewaymakersecond_sessions.erase(session);
            worked = true;
        }
        return worked;
    });
    sim::Component gatewaytakersecond_thread("gatewaytakersecond", [&] {
        bool worked = to_clients_gatewaytakersecond.read([&](std::string_view message) { gatewaytakersecond_events.on_message(message); });
        worked = from_takersecond.read([&](std::string_view bytes) { gatewaytakersecond.on_bytes(takersecond_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewaytakersecond_order_entry) worked = gatewaytakersecond_order_entry->poll([&](int fd, std::string_view bytes) {
            gatewaytakersecond.on_bytes(gatewaytakersecond_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewaytakersecond_sessions.find(fd); gone != gatewaytakersecond_sessions.end()) {
                gatewaytakersecond.disconnect(gone->second);
                gatewaytakersecond_sessions.erase(gone);
            }
        }, [&](int fd) { gatewaytakersecond_sessions.emplace(fd, gatewaytakersecond.connect([server = gatewaytakersecond_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
        gatewaytakersecond.tick();
        if (gatewaytakersecond.ended(takersecond_client)) {
            gatewaytakersecond.disconnect(takersecond_client);
            takersecond_client = gatewaytakersecond.connect(takersecond_reply);
            worked = true;
        }
        for (auto session = gatewaytakersecond_sessions.begin(); session != gatewaytakersecond_sessions.end(); ) {
            if (!gatewaytakersecond.ended(session->second)) { ++session; continue; }
            if (gatewaytakersecond_order_entry) gatewaytakersecond_order_entry->close(session->first);
            gatewaytakersecond.disconnect(session->second);
            session = gatewaytakersecond_sessions.erase(session);
            worked = true;
        }
        return worked;
    });
    sim::Component gatewayfixmaker_thread("gatewayfixmaker", [&] {
        bool worked = to_clients_gatewayfixmaker.read([&](std::string_view message) { gatewayfixmaker_events.on_message(message); });
        worked = from_fixmaker.read([&](std::string_view bytes) { gatewayfixmaker.on_bytes(fixmaker_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewayfixmaker_order_entry) worked = gatewayfixmaker_order_entry->poll([&](int fd, std::string_view bytes) {
            gatewayfixmaker.on_bytes(gatewayfixmaker_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewayfixmaker_sessions.find(fd); gone != gatewayfixmaker_sessions.end()) {
                gatewayfixmaker.disconnect(gone->second);
                gatewayfixmaker_sessions.erase(gone);
            }
        }, [&](int fd) { gatewayfixmaker_sessions.emplace(fd, gatewayfixmaker.connect([server = gatewayfixmaker_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
        gatewayfixmaker.tick();
        if (gatewayfixmaker.ended(fixmaker_client)) {
            gatewayfixmaker.disconnect(fixmaker_client);
            fixmaker_client = gatewayfixmaker.connect(fixmaker_reply);
            worked = true;
        }
        for (auto session = gatewayfixmaker_sessions.begin(); session != gatewayfixmaker_sessions.end(); ) {
            if (!gatewayfixmaker.ended(session->second)) { ++session; continue; }
            if (gatewayfixmaker_order_entry) gatewayfixmaker_order_entry->close(session->first);
            gatewayfixmaker.disconnect(session->second);
            session = gatewayfixmaker_sessions.erase(session);
            worked = true;
        }
        return worked;
    });
    sim::Component gatewayfixtaker_thread("gatewayfixtaker", [&] {
        bool worked = to_clients_gatewayfixtaker.read([&](std::string_view message) { gatewayfixtaker_events.on_message(message); });
        worked = from_fixtaker.read([&](std::string_view bytes) { gatewayfixtaker.on_bytes(fixtaker_client, bytes.data(), bytes.size()); }) || worked;
        if (gatewayfixtaker_order_entry) worked = gatewayfixtaker_order_entry->poll([&](int fd, std::string_view bytes) {
            gatewayfixtaker.on_bytes(gatewayfixtaker_sessions.at(fd), bytes.data(), bytes.size());
        }, [&](int fd) {
            if (const auto gone = gatewayfixtaker_sessions.find(fd); gone != gatewayfixtaker_sessions.end()) {
                gatewayfixtaker.disconnect(gone->second);
                gatewayfixtaker_sessions.erase(gone);
            }
        }, [&](int fd) { gatewayfixtaker_sessions.emplace(fd, gatewayfixtaker.connect([server = gatewayfixtaker_order_entry.get(), fd](const char* b, std::size_t n) { server->send(fd, b, n); })); }) || worked;
        gatewayfixtaker.tick();
        if (gatewayfixtaker.ended(fixtaker_client)) {
            gatewayfixtaker.disconnect(fixtaker_client);
            fixtaker_client = gatewayfixtaker.connect(fixtaker_reply);
            worked = true;
        }
        for (auto session = gatewayfixtaker_sessions.begin(); session != gatewayfixtaker_sessions.end(); ) {
            if (!gatewayfixtaker.ended(session->second)) { ++session; continue; }
            if (gatewayfixtaker_order_entry) gatewayfixtaker_order_entry->close(session->first);
            gatewayfixtaker.disconnect(session->second);
            session = gatewayfixtaker_sessions.erase(session);
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
    sim::Component requests_thread("requests", [&] {
        bool worked = to_requests.read([&](std::string_view bytes) { requests.on_bytes(bytes.data(), bytes.size()); });
        requests.tick();   // its session's heartbeats, and the venue gone silent
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
    sim::Component makersecond_thread("makersecond", [&] {
        bool worked = to_makersecond.read([&](std::string_view bytes) { makersecond.on_bytes(bytes.data(), bytes.size()); });
        makersecond.tick();   // its session's heartbeats, and the venue gone silent
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (makersecond.logged_in() && std::chrono::steady_clock::now() >= makersecond_paced) {
                makersecond_strategy.step(makersecond.orders());
                ++makersecond_ticks;
                makersecond_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!makersecond_turn.asked()) return worked;
        if (makersecond.logged_in()) { makersecond_strategy.step(makersecond.orders()); ++makersecond_ticks; }
        makersecond_turn.taken();
        return true;
    });
    sim::Component takersecond_thread("takersecond", [&] {
        bool worked = to_takersecond.read([&](std::string_view bytes) { takersecond.on_bytes(bytes.data(), bytes.size()); });
        takersecond.tick();   // its session's heartbeats, and the venue gone silent
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (takersecond.logged_in() && std::chrono::steady_clock::now() >= takersecond_paced) {
                takersecond_strategy.step(takersecond.orders());
                ++takersecond_ticks;
                takersecond_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!takersecond_turn.asked()) return worked;
        if (takersecond.logged_in()) { takersecond_strategy.step(takersecond.orders()); ++takersecond_ticks; }
        takersecond_turn.taken();
        return true;
    });
    sim::Component fixmaker_thread("fixmaker", [&] {
        bool worked = to_fixmaker.read([&](std::string_view bytes) { fixmaker.on_bytes(bytes.data(), bytes.size()); });
        fixmaker.tick();   // its session's heartbeats, and the venue gone silent
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (fixmaker.logged_in() && std::chrono::steady_clock::now() >= fixmaker_paced) {
                fixmaker_strategy.step(fixmaker.orders());
                ++fixmaker_ticks;
                fixmaker_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!fixmaker_turn.asked()) return worked;
        if (fixmaker.logged_in()) { fixmaker_strategy.step(fixmaker.orders()); ++fixmaker_ticks; }
        fixmaker_turn.taken();
        return true;
    });
    sim::Component fixtaker_thread("fixtaker", [&] {
        bool worked = to_fixtaker.read([&](std::string_view bytes) { fixtaker.on_bytes(bytes.data(), bytes.size()); });
        fixtaker.tick();   // its session's heartbeats, and the venue gone silent
        // A served venue runs free; the startup run takes turns, so it is the same every time.
        // Served, the trader takes in what has arrived on every pass and orders at its own pace,
        // twenty times a second: sleeping in the step would leave the venue's answers waiting.
        if (serve) {
            if (fixtaker.logged_in() && std::chrono::steady_clock::now() >= fixtaker_paced) {
                fixtaker_strategy.step(fixtaker.orders());
                ++fixtaker_ticks;
                fixtaker_paced = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
                worked = true;
            }
            return worked;
        }
        if (!fixtaker_turn.asked()) return worked;
        if (fixtaker.logged_in()) { fixtaker_strategy.step(fixtaker.orders()); ++fixtaker_ticks; }
        fixtaker_turn.taken();
        return true;
    });

    engine_thread.start();
    sequencer_thread.start();
    day_thread.start();
    totalview_thread.start();
    gatewaymakerbasic_thread.start();
    gatewaytakerbasic_thread.start();
    gatewayreplay_thread.start();
    gatewayrequests_thread.start();
    gatewaymakersecond_thread.start();
    gatewaytakersecond_thread.start();
    gatewayfixmaker_thread.start();
    gatewayfixtaker_thread.start();
    glimpse_thread.start();
    totalviewtcp_thread.start();
    makerbasic_thread.start();
    takerbasic_thread.start();
    replay_thread.start();
    requests_thread.start();
    makersecond_thread.start();
    takersecond_thread.start();
    fixmaker_thread.start();
    fixtaker_thread.start();

    // The startup run, taken in turns: one trader acts, the venue settles, the next acts. The
    // same orders meet in the same order every time, whatever the threads' timing.
    if (!serve) {
        // Everyone is logged in before the first turn.
        bool settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000));
        for (int tick = 0; tick < 20; ++tick) {
            for (const auto& scheduled : decltype(day_component)::schedule) {
                if (!(tick == scheduled.tick)) continue;
                day_component.request(scheduled);
                settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            }
            day_component.indicate();
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            makerbasic_turn.ask();
            settled = makerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            takerbasic_turn.ask();
            settled = takerbasic_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            replay_turn.ask();
            settled = replay_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            requests_turn.ask();
            settled = requests_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            makersecond_turn.ask();
            settled = makersecond_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            takersecond_turn.ask();
            settled = takersecond_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            fixmaker_turn.ask();
            settled = fixmaker_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
            fixtaker_turn.ask();
            settled = fixtaker_turn.wait() && settled;
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
        }
        for (const auto& scheduled : decltype(day_component)::schedule) {
            if (!(scheduled.tick >= 20)) continue;
            day_component.request(scheduled);
            settled = sim::quiesce({ &to_sequencer_gatewaymakerbasic, &to_sequencer_gatewaytakerbasic, &to_sequencer_gatewayreplay, &to_sequencer_gatewayrequests, &to_sequencer_gatewaymakersecond, &to_sequencer_gatewaytakersecond, &to_sequencer_gatewayfixmaker, &to_sequencer_gatewayfixtaker, &sequenced, &to_clients_gatewaymakerbasic, &to_clients_gatewaytakerbasic, &to_clients_gatewayreplay, &to_clients_gatewayrequests, &to_clients_gatewaymakersecond, &to_clients_gatewaytakersecond, &to_clients_gatewayfixmaker, &to_clients_gatewayfixtaker, &from_day, &to_totalview, &to_makerbasic, &from_makerbasic, &to_takerbasic, &from_takerbasic, &to_replay, &from_replay, &to_requests, &from_requests, &to_makersecond, &from_makersecond, &to_takersecond, &from_takersecond, &to_fixmaker, &from_fixmaker, &to_fixtaker, &from_fixtaker, &to_glimpse, &to_totalviewtcp }, { &engine_thread, &sequencer_thread, &gatewaymakerbasic_thread, &gatewaytakerbasic_thread, &gatewayreplay_thread, &gatewayrequests_thread, &gatewaymakersecond_thread, &gatewaytakersecond_thread, &gatewayfixmaker_thread, &gatewayfixtaker_thread, &day_thread, &totalview_thread, &makerbasic_thread, &takerbasic_thread, &replay_thread, &requests_thread, &makersecond_thread, &takersecond_thread, &fixmaker_thread, &fixtaker_thread, &glimpse_thread, &totalviewtcp_thread }, std::chrono::milliseconds(5000)) && settled;
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
    makersecond_thread.stop();
    to_makersecond.close();
    takersecond_thread.stop();
    to_takersecond.close();
    fixmaker_thread.stop();
    to_fixmaker.close();
    fixtaker_thread.stop();
    to_fixtaker.close();
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
    gatewaymakersecond_thread.stop();
    to_clients_gatewaymakersecond.close();
    from_makersecond.close();
    gatewaytakersecond_thread.stop();
    to_clients_gatewaytakersecond.close();
    from_takersecond.close();
    gatewayfixmaker_thread.stop();
    to_clients_gatewayfixmaker.close();
    from_fixmaker.close();
    gatewayfixtaker_thread.stop();
    to_clients_gatewayfixtaker.close();
    from_fixtaker.close();
    day_thread.stop();
    sequencer_thread.stop();
    to_sequencer_gatewaymakerbasic.close();
    to_sequencer_gatewaytakerbasic.close();
    to_sequencer_gatewayreplay.close();
    to_sequencer_gatewayrequests.close();
    to_sequencer_gatewaymakersecond.close();
    to_sequencer_gatewaytakersecond.close();
    to_sequencer_gatewayfixmaker.close();
    to_sequencer_gatewayfixtaker.close();
    from_day.close();
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
            void silent() { over = true; }
        } watched;
        std::size_t session = 0;
        auto to_service = [&](const char* b, std::size_t n) { glimpse.on_bytes(session, b, n); };
        sim::replay::glimpse::soupbin::Client<decltype(to_service), watcher> reader(to_service, watched);
        session = glimpse.connect([&](const char* b, std::size_t n) { reader.on_bytes(b, n); });
        reader.login("MAKER1", "maker10000");
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
            void silent() { over = true; }
        } watched;
        std::size_t session = 0;
        auto to_service = [&](const char* b, std::size_t n) { totalviewtcp.on_bytes(session, b, n); };
        sim::replay::totalviewtcp::soupbin::Client<decltype(to_service), watcher> reader(to_service, watched);
        session = totalviewtcp.connect([&](const char* b, std::size_t n) { reader.on_bytes(b, n); });
        reader.login("MAKER1", "maker10000", {}, 1);   // from the first message of the session
        totalviewtcp.pump();
        std::printf("nasdaq-simulator: TotalViewTcp served %llu messages%s, resume from %llu\n",
                    (unsigned long long)watched.messages, watched.over ? " then ended the session" : "", (unsigned long long)watched.resume);
    }

    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, mass_cancel_response=%llu, disable_order_entry_response=%llu, enable_order_entry_response=%llu, rejected_order=%llu, account_query_response=%llu, system_event=%llu, order_accepted=%llu, replaced=%llu, order_modified=%llu, order_executed=%llu, broken_trade=%llu, order_restated=%llu, order_priority_update=%llu, cancel_pending=%llu, cancel_reject=%llu, aiq_canceled=%llu, canceled=%llu\n",
                makerbasic.name().c_str(), makerbasic.logged_in() ? makerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)makerbasic.entered(), (unsigned long long)makerbasic.canceled(),
            (unsigned long long)makerbasic.answers().mass_cancel_response_message,
            (unsigned long long)makerbasic.answers().disable_order_entry_response_message,
            (unsigned long long)makerbasic.answers().enable_order_entry_response_message,
            (unsigned long long)makerbasic.answers().rejected_order_message,
            (unsigned long long)makerbasic.answers().account_query_response_message,
            (unsigned long long)makerbasic.answers().system_event_message,
            (unsigned long long)makerbasic.answers().order_accepted_message,
            (unsigned long long)makerbasic.answers().replaced_message,
            (unsigned long long)makerbasic.answers().order_modified_message,
            (unsigned long long)makerbasic.answers().order_executed_message,
            (unsigned long long)makerbasic.answers().broken_trade_message,
            (unsigned long long)makerbasic.answers().order_restated_message,
            (unsigned long long)makerbasic.answers().order_priority_update_message,
            (unsigned long long)makerbasic.answers().cancel_pending_message,
            (unsigned long long)makerbasic.answers().cancel_reject_message,
            (unsigned long long)makerbasic.answers().aiq_canceled_message,
            (unsigned long long)makerbasic.answers().canceled_message);
    if (makerbasic.silences() > 0)
        std::printf("nasdaq-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", makerbasic.name().c_str(),
                    (unsigned long long)makerbasic.silences(), makerbasic.logged_in() ? "logged in again" : "not logged in again");
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, mass_cancel_response=%llu, disable_order_entry_response=%llu, enable_order_entry_response=%llu, rejected_order=%llu, account_query_response=%llu, system_event=%llu, order_accepted=%llu, replaced=%llu, order_modified=%llu, order_executed=%llu, broken_trade=%llu, order_restated=%llu, order_priority_update=%llu, cancel_pending=%llu, cancel_reject=%llu, aiq_canceled=%llu, canceled=%llu\n",
                takerbasic.name().c_str(), takerbasic.logged_in() ? takerbasic.session_name().c_str() : "not logged in",
                (unsigned long long)takerbasic.entered(), (unsigned long long)takerbasic.canceled(),
            (unsigned long long)takerbasic.answers().mass_cancel_response_message,
            (unsigned long long)takerbasic.answers().disable_order_entry_response_message,
            (unsigned long long)takerbasic.answers().enable_order_entry_response_message,
            (unsigned long long)takerbasic.answers().rejected_order_message,
            (unsigned long long)takerbasic.answers().account_query_response_message,
            (unsigned long long)takerbasic.answers().system_event_message,
            (unsigned long long)takerbasic.answers().order_accepted_message,
            (unsigned long long)takerbasic.answers().replaced_message,
            (unsigned long long)takerbasic.answers().order_modified_message,
            (unsigned long long)takerbasic.answers().order_executed_message,
            (unsigned long long)takerbasic.answers().broken_trade_message,
            (unsigned long long)takerbasic.answers().order_restated_message,
            (unsigned long long)takerbasic.answers().order_priority_update_message,
            (unsigned long long)takerbasic.answers().cancel_pending_message,
            (unsigned long long)takerbasic.answers().cancel_reject_message,
            (unsigned long long)takerbasic.answers().aiq_canceled_message,
            (unsigned long long)takerbasic.answers().canceled_message);
    if (takerbasic.silences() > 0)
        std::printf("nasdaq-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", takerbasic.name().c_str(),
                    (unsigned long long)takerbasic.silences(), takerbasic.logged_in() ? "logged in again" : "not logged in again");
    std::printf("nasdaq-simulator: trader %s replayed %zu quotes from captures/TotalView.pcap\n", replay.name().c_str(), replay_strategy.available());
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, mass_cancel_response=%llu, disable_order_entry_response=%llu, enable_order_entry_response=%llu, rejected_order=%llu, account_query_response=%llu, system_event=%llu, order_accepted=%llu, replaced=%llu, order_modified=%llu, order_executed=%llu, broken_trade=%llu, order_restated=%llu, order_priority_update=%llu, cancel_pending=%llu, cancel_reject=%llu, aiq_canceled=%llu, canceled=%llu\n",
                replay.name().c_str(), replay.logged_in() ? replay.session_name().c_str() : "not logged in",
                (unsigned long long)replay.entered(), (unsigned long long)replay.canceled(),
            (unsigned long long)replay.answers().mass_cancel_response_message,
            (unsigned long long)replay.answers().disable_order_entry_response_message,
            (unsigned long long)replay.answers().enable_order_entry_response_message,
            (unsigned long long)replay.answers().rejected_order_message,
            (unsigned long long)replay.answers().account_query_response_message,
            (unsigned long long)replay.answers().system_event_message,
            (unsigned long long)replay.answers().order_accepted_message,
            (unsigned long long)replay.answers().replaced_message,
            (unsigned long long)replay.answers().order_modified_message,
            (unsigned long long)replay.answers().order_executed_message,
            (unsigned long long)replay.answers().broken_trade_message,
            (unsigned long long)replay.answers().order_restated_message,
            (unsigned long long)replay.answers().order_priority_update_message,
            (unsigned long long)replay.answers().cancel_pending_message,
            (unsigned long long)replay.answers().cancel_reject_message,
            (unsigned long long)replay.answers().aiq_canceled_message,
            (unsigned long long)replay.answers().canceled_message);
    if (replay.silences() > 0)
        std::printf("nasdaq-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", replay.name().c_str(),
                    (unsigned long long)replay.silences(), replay.logged_in() ? "logged in again" : "not logged in again");
    std::printf("nasdaq-simulator: trader %s replayed %zu quotes from captures/Ouch.pcap\n", requests.name().c_str(), requests_strategy.available());
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, mass_cancel_response=%llu, disable_order_entry_response=%llu, enable_order_entry_response=%llu, rejected_order=%llu, account_query_response=%llu, system_event=%llu, order_accepted=%llu, replaced=%llu, order_modified=%llu, order_executed=%llu, broken_trade=%llu, order_restated=%llu, order_priority_update=%llu, cancel_pending=%llu, cancel_reject=%llu, aiq_canceled=%llu, canceled=%llu\n",
                requests.name().c_str(), requests.logged_in() ? requests.session_name().c_str() : "not logged in",
                (unsigned long long)requests.entered(), (unsigned long long)requests.canceled(),
            (unsigned long long)requests.answers().mass_cancel_response_message,
            (unsigned long long)requests.answers().disable_order_entry_response_message,
            (unsigned long long)requests.answers().enable_order_entry_response_message,
            (unsigned long long)requests.answers().rejected_order_message,
            (unsigned long long)requests.answers().account_query_response_message,
            (unsigned long long)requests.answers().system_event_message,
            (unsigned long long)requests.answers().order_accepted_message,
            (unsigned long long)requests.answers().replaced_message,
            (unsigned long long)requests.answers().order_modified_message,
            (unsigned long long)requests.answers().order_executed_message,
            (unsigned long long)requests.answers().broken_trade_message,
            (unsigned long long)requests.answers().order_restated_message,
            (unsigned long long)requests.answers().order_priority_update_message,
            (unsigned long long)requests.answers().cancel_pending_message,
            (unsigned long long)requests.answers().cancel_reject_message,
            (unsigned long long)requests.answers().aiq_canceled_message,
            (unsigned long long)requests.answers().canceled_message);
    if (requests.silences() > 0)
        std::printf("nasdaq-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", requests.name().c_str(),
                    (unsigned long long)requests.silences(), requests.logged_in() ? "logged in again" : "not logged in again");
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, mass_cancel_response=%llu, disable_order_entry_response=%llu, enable_order_entry_response=%llu, rejected_order=%llu, account_query_response=%llu, system_event=%llu, order_accepted=%llu, replaced=%llu, order_modified=%llu, order_executed=%llu, broken_trade=%llu, order_restated=%llu, order_priority_update=%llu, cancel_pending=%llu, cancel_reject=%llu, aiq_canceled=%llu, canceled=%llu\n",
                makersecond.name().c_str(), makersecond.logged_in() ? makersecond.session_name().c_str() : "not logged in",
                (unsigned long long)makersecond.entered(), (unsigned long long)makersecond.canceled(),
            (unsigned long long)makersecond.answers().mass_cancel_response_message,
            (unsigned long long)makersecond.answers().disable_order_entry_response_message,
            (unsigned long long)makersecond.answers().enable_order_entry_response_message,
            (unsigned long long)makersecond.answers().rejected_order_message,
            (unsigned long long)makersecond.answers().account_query_response_message,
            (unsigned long long)makersecond.answers().system_event_message,
            (unsigned long long)makersecond.answers().order_accepted_message,
            (unsigned long long)makersecond.answers().replaced_message,
            (unsigned long long)makersecond.answers().order_modified_message,
            (unsigned long long)makersecond.answers().order_executed_message,
            (unsigned long long)makersecond.answers().broken_trade_message,
            (unsigned long long)makersecond.answers().order_restated_message,
            (unsigned long long)makersecond.answers().order_priority_update_message,
            (unsigned long long)makersecond.answers().cancel_pending_message,
            (unsigned long long)makersecond.answers().cancel_reject_message,
            (unsigned long long)makersecond.answers().aiq_canceled_message,
            (unsigned long long)makersecond.answers().canceled_message);
    if (makersecond.silences() > 0)
        std::printf("nasdaq-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", makersecond.name().c_str(),
                    (unsigned long long)makersecond.silences(), makersecond.logged_in() ? "logged in again" : "not logged in again");
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, mass_cancel_response=%llu, disable_order_entry_response=%llu, enable_order_entry_response=%llu, rejected_order=%llu, account_query_response=%llu, system_event=%llu, order_accepted=%llu, replaced=%llu, order_modified=%llu, order_executed=%llu, broken_trade=%llu, order_restated=%llu, order_priority_update=%llu, cancel_pending=%llu, cancel_reject=%llu, aiq_canceled=%llu, canceled=%llu\n",
                takersecond.name().c_str(), takersecond.logged_in() ? takersecond.session_name().c_str() : "not logged in",
                (unsigned long long)takersecond.entered(), (unsigned long long)takersecond.canceled(),
            (unsigned long long)takersecond.answers().mass_cancel_response_message,
            (unsigned long long)takersecond.answers().disable_order_entry_response_message,
            (unsigned long long)takersecond.answers().enable_order_entry_response_message,
            (unsigned long long)takersecond.answers().rejected_order_message,
            (unsigned long long)takersecond.answers().account_query_response_message,
            (unsigned long long)takersecond.answers().system_event_message,
            (unsigned long long)takersecond.answers().order_accepted_message,
            (unsigned long long)takersecond.answers().replaced_message,
            (unsigned long long)takersecond.answers().order_modified_message,
            (unsigned long long)takersecond.answers().order_executed_message,
            (unsigned long long)takersecond.answers().broken_trade_message,
            (unsigned long long)takersecond.answers().order_restated_message,
            (unsigned long long)takersecond.answers().order_priority_update_message,
            (unsigned long long)takersecond.answers().cancel_pending_message,
            (unsigned long long)takersecond.answers().cancel_reject_message,
            (unsigned long long)takersecond.answers().aiq_canceled_message,
            (unsigned long long)takersecond.answers().canceled_message);
    if (takersecond.silences() > 0)
        std::printf("nasdaq-simulator: trader %s: the venue went silent on it %llu time(s); %s\n", takersecond.name().c_str(),
                    (unsigned long long)takersecond.silences(), takersecond.logged_in() ? "logged in again" : "not logged in again");
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, ExecutionReport=%llu, OrderCancelReject=%llu\n",
                fixmaker.name().c_str(), fixmaker.logged_in() ? fixmaker.session_name().c_str() : "not logged in",
                (unsigned long long)fixmaker.entered(), (unsigned long long)fixmaker.canceled(),
            (unsigned long long)fixmaker.answers().ExecutionReport,
            (unsigned long long)fixmaker.answers().OrderCancelReject);
    std::printf("nasdaq-simulator: trader %s (%s) entered %llu, canceled %llu, ExecutionReport=%llu, OrderCancelReject=%llu\n",
                fixtaker.name().c_str(), fixtaker.logged_in() ? fixtaker.session_name().c_str() : "not logged in",
                (unsigned long long)fixtaker.entered(), (unsigned long long)fixtaker.canceled(),
            (unsigned long long)fixtaker.answers().ExecutionReport,
            (unsigned long long)fixtaker.answers().OrderCancelReject);
    std::printf("nasdaq-simulator: %llu trades matched, ipc carried %llu order + %llu event messages\n",
                (unsigned long long)trades.load(), (unsigned long long)ipc_msgs.load(), (unsigned long long)ipc_events.load());
    return 0;
}
