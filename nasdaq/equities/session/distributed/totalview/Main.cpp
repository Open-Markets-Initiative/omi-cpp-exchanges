// Generated Nasdaq Stock Market Session TotalView feed — one component, one process.
// 
// The transforms it publishes through are the ones declared for it, the same as in the
// single binary; only its links differ, and those are sockets, dialled where the configuration says each component is.

#include "protocols/marketdata/totalview/PacketPublisher.hpp"
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
        sim::IpcSocket to_totalview("engine -> totalview", sim::IpcSocket::End::Reads, cfg.to_totalview_link.host, cfg.to_totalview_link.port);
        sim::MessageSocket to_glimpse("totalview -> glimpse", sim::MessageSocket::End::Writes, cfg.to_glimpse_link.host, cfg.to_glimpse_link.port);
        sim::MessageSocket to_totalviewtcp("totalview -> totalviewtcp", sim::MessageSocket::End::Writes, cfg.to_totalviewtcp_link.host, cfg.to_totalviewtcp_link.port);

        // The publisher: what it writes goes to the recording and out as multicast, as it
        // does in the single binary.
        sim::MarketDataLog totalview_rec("recordings/marketdata/TotalView.dat");
        net::UdpMulticastPublisher totalview_mc(cfg.totalview_feed.host, cfg.totalview_feed.port);
        sim::totalview::PacketPublisher totalview("NSMTV00001", [&](const char* s, std::size_t n) { totalview_rec(s, n); totalview_mc(s, n); });

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

        std::printf("nasdaq-totalview: publishing to %s:%u\n", cfg.totalview_feed.host.c_str(), (unsigned)cfg.totalview_feed.port);

        std::uint64_t handled = 0;
        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_totalview.read([&](std::string_view message) { if (const auto asked = sim::book_end_of(message)) totalview_rec.mark(*asked); totalview_events.on_message(message); ++handled; }) || worked;
            to_glimpse.deliver();
            to_totalviewtcp.deliver();
            totalview.idle();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_totalview.gaps() > 0) std::printf("nasdaq-totalview: %llu gap(s) on engine -> totalview — messages were lost\n", (unsigned long long)to_totalview.gaps());
        std::printf("nasdaq-totalview: stopped — %llu events read, %llu messages published\n",
                    (unsigned long long)handled, (unsigned long long)(totalview.next_seq() - 1));
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-totalview: %s\n", problem.what());
        return 1;
    }

    return 0;
}
