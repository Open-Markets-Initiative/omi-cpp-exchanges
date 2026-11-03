#define BOOST_TEST_MODULE sim_tests
#include <boost/test/included/unit_test.hpp>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include "MatchingEngine.hpp"
#include "engine/Engine.hpp"
#include "protocols/orderentry/Gateway.hpp"
#include "protocols/marketdata/totalview/PacketPublisher.hpp"

namespace entry = ::nasdaq::nsmequities::orders::ouch::v5_0;

// The engine crosses an aggressor against a resting order and numbers each fill.
BOOST_AUTO_TEST_CASE(engine_crosses_and_numbers_fills) {
    std::vector<matching::Trade> trades;
    std::vector<matching::Order> rested, canceled;
    matching::MatchingEngine engine(
        [&](const matching::Trade& t) { trades.push_back(t); },
        [&](const matching::Order& o) { rested.push_back(o); },
        [&](const matching::Order& o) { canceled.push_back(o); });

    engine.on_new_order(matching::Order{ 1, matching::Side::Buy, 10000, 100 });
    engine.on_new_order(matching::Order{ 2, matching::Side::Sell, 10000, 40 });

    BOOST_REQUIRE_EQUAL(trades.size(), 1u);
    BOOST_CHECK_EQUAL(trades[0].aggressor_id, 2u);
    BOOST_CHECK_EQUAL(trades[0].resting_id, 1u);
    BOOST_CHECK_EQUAL(trades[0].qty, 40u);
    BOOST_CHECK_EQUAL(trades[0].match_id, 1u);
    BOOST_CHECK_EQUAL(rested.size(), 1u);

    BOOST_CHECK(engine.on_cancel(1));
    BOOST_REQUIRE_EQUAL(canceled.size(), 1u);
    BOOST_CHECK_EQUAL(canceled[0].id, 1u);
}
// TotalView: the publisher frames a packet its own model reads back.
BOOST_AUTO_TEST_CASE(totalview_publishes_a_packet_its_model_reads) {
    std::string packet;
    sim::totalview::PacketPublisher publisher("NSMTV00001", [&](const char* b, std::size_t n) { packet.assign(b, n); });

    const char message[] = { 'A', 'B', 'C', 'D' };
    publisher.publish_message(message, sizeof message);
    publisher.publish_message(message, sizeof message);
    publisher.flush();

    BOOST_REQUIRE_EQUAL(packet.size(), sizeof(nasdaq::nsmequities::totalview::itch::v5_0_2026::packet_header) + 2 * sizeof message);
    const auto* header = nasdaq::nsmequities::totalview::itch::v5_0_2026::packet_header::parse(reinterpret_cast<const std::byte*>(packet.data()));
    BOOST_CHECK_EQUAL(header->session.get_trimmed().value(), "NSMTV00001");
    BOOST_CHECK_EQUAL(header->sequence_number.get().value(), 1u);
    BOOST_CHECK_EQUAL(header->message_count.get().value(), 2u);
    BOOST_CHECK_EQUAL(publisher.next_seq(), 3u);
}

// A client logs in, enters an order, is filled by the engine and cancels what is left.
BOOST_AUTO_TEST_CASE(gateway_admits_fills_and_cancels_an_order) {
    std::vector<std::string> sent;
    std::function<void(const char*, std::size_t)> to_engine;
    auto emit = [&](const char* b, std::size_t n) { to_engine(b, n); };
    sim::orderentry::Gateway<decltype(emit)> gateway(emit);
    gateway.allow("ALICE1", "alice00001");

    sim::IpcEventServer events(
        [&](const sim::ipc_ns::trade_message& t) { gateway.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gateway.on_event(c); });
    auto event_emit = [&](const char* b, std::size_t n) { events.on_message(std::string_view(b, n)); };
    matching::MatchingEngine engine(
        [&](const matching::Trade& t)  { sim::emit_trade(event_emit, t); },
        [&](const matching::Order& o) { sim::emit_rested(event_emit, o); },
        [&](const matching::Order& o) { sim::emit_canceled(event_emit, o); });
    sim::IpcEngineServer ipc_server(engine);
    to_engine = [&](const char* b, std::size_t n) { ipc_server.on_message(std::string_view(b, n)); };

    const auto client = gateway.connect([&](const char* b, std::size_t n) { sent.emplace_back(b, n); });
    const auto whole = [](const auto& packet) {
        return std::string(reinterpret_cast<const char*>(&packet), packet.header.packet_length.get().value() + 2);
    };
    // Each answer is a whole packet: its server header, then the message the model lays out.
    const auto answer = [&](std::size_t at) { return reinterpret_cast<const std::byte*>(sent.at(at).data()); };

    // Login, presenting what this protocol's own login carries.
    entry::login_request_packet login{};
    login.fields.username.set("ALICE1");
    login.fields.password.set("alice00001");
    login.fields.requested_session.set("");
    login.fields.requested_sequence_number.set("0");
    gateway.on_bytes(client, whole(login).data(), whole(login).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 1u);
    // The first answer is the login admitted, read by the code the model selects it on.
    BOOST_CHECK_EQUAL(static_cast<std::uint64_t>(entry::server_packet_header::parse(answer(0))->server_packet_type.get().value()), static_cast<std::uint64_t>('A'));

    // An order the gateway admits, answered before the engine sees it
    entry::enter_order_message order{};
    order.fields.user_ref_num.set(7);
    order.fields.side.set(static_cast<std::remove_cvref_t<decltype(order.fields.side.get().value())>>('B'));
    order.fields.quantity.set(100);
    order.fields.price.set(10000);
    order.fields.symbol.set("ZVZZT");
    gateway.on_bytes(client, whole(order).data(), whole(order).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 2u);
    BOOST_CHECK_EQUAL(gateway.open_orders(), 1u);
    const auto* accepted = entry::order_accepted_message::parse(answer(1));
    BOOST_CHECK_EQUAL(accepted->fields.user_ref_num.get().value(), 7u);

    // The engine fills part of it against an order of its own
    engine.on_new_order(matching::Order{ 99, matching::Side::Sell, 10000, 40 });
    BOOST_REQUIRE_EQUAL(sent.size(), 3u);
    const auto* filled = entry::order_executed_message::parse(answer(2));
    BOOST_CHECK_EQUAL(filled->fields.user_ref_num.get().value(), 7u);
    BOOST_CHECK_EQUAL(filled->fields.quantity.get().value(), 40u);

    // The client cancels, and is told what was left
    entry::cancel_order_message request{};
    request.fields.user_ref_num.set(7);
    request.fields.quantity.set(0);
    gateway.on_bytes(client, whole(request).data(), whole(request).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 4u);
    const auto* gone = entry::canceled_message::parse(answer(3));
    BOOST_CHECK_EQUAL(gone->fields.user_ref_num.get().value(), 7u);
    BOOST_CHECK_EQUAL(gateway.open_orders(), 0u);
}

// The day walked to its end refuses an order, and the client is told it is canceled.
BOOST_AUTO_TEST_CASE(VerifyRefusedOrderIsCanceledClosed) {
    std::vector<std::string> sent;
    std::function<void(const char*, std::size_t)> to_engine;
    auto emit = [&](const char* b, std::size_t n) { to_engine(b, n); };
    sim::orderentry::Gateway<decltype(emit)> gateway(emit);
    gateway.allow("ALICE1", "alice00001");

    std::size_t traded = 0, rested = 0;
    sim::IpcEventServer events(
        [&](const sim::ipc_ns::trade_message&) { ++traded; },
        [&](const sim::ipc_ns::order_rested_message&) { ++rested; },
        [](const sim::ipc_ns::order_canceled_message&) {},
        [&](const sim::ipc_ns::order_refused_message& j) { gateway.on_event(j); },
        [](const sim::ipc_ns::phase_announced_message&) {},
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {});
    auto event_emit = [&](const char* b, std::size_t n) { events.on_message(std::string_view(b, n)); };
    std::atomic<std::uint64_t> trades{0};
    sim::components::Engine<decltype(event_emit)> engine(event_emit, trades);
    sim::IpcEngineServer ipc_server(engine.day());
    to_engine = [&](const char* b, std::size_t n) { ipc_server.on_message(std::string_view(b, n)); };

    // Every change the day is declared with, in order, each one made.
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::PreOpen));
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::Open));
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::Halt));
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::Resume));
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::PreClose));
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::Close));
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::End));

    const auto client = gateway.connect([&](const char* b, std::size_t n) { sent.emplace_back(b, n); });
    const auto whole = [](const auto& packet) {
        return std::string(reinterpret_cast<const char*>(&packet), packet.header.packet_length.get().value() + 2);
    };
    // Each answer is a whole packet: its server header, then the message the model lays out.
    const auto answer = [&](std::size_t at) { return reinterpret_cast<const std::byte*>(sent.at(at).data()); };

    // Login, presenting what this protocol's own login carries.
    entry::login_request_packet login{};
    login.fields.username.set("ALICE1");
    login.fields.password.set("alice00001");
    login.fields.requested_session.set("");
    login.fields.requested_sequence_number.set("0");
    gateway.on_bytes(client, whole(login).data(), whole(login).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 1u);
    // The first answer is the login admitted, read by the code the model selects it on.
    BOOST_CHECK_EQUAL(static_cast<std::uint64_t>(entry::server_packet_header::parse(answer(0))->server_packet_type.get().value()), static_cast<std::uint64_t>('A'));

    // An order the gateway admits, answered before the engine sees it
    entry::enter_order_message order{};
    order.fields.user_ref_num.set(7);
    order.fields.side.set(static_cast<std::remove_cvref_t<decltype(order.fields.side.get().value())>>('B'));
    order.fields.quantity.set(100);
    order.fields.price.set(10000);
    order.fields.symbol.set("ZVZZT");
    gateway.on_bytes(client, whole(order).data(), whole(order).size());
    // Accepted at the gateway, then refused by the day.
    BOOST_REQUIRE_EQUAL(sent.size(), 3u);
    const auto* refused = entry::canceled_message::parse(answer(2));
    BOOST_CHECK_EQUAL(refused->fields.user_ref_num.get().value(), 7u);
    BOOST_CHECK_EQUAL(refused->fields.quantity.get().value(), 100u);
    BOOST_CHECK_EQUAL(static_cast<char>(refused->fields.cancel_order_reason.get().value()), 'E');
    BOOST_CHECK_EQUAL(gateway.open_orders(), 0u);
    // Nothing reached the book.
    BOOST_CHECK_EQUAL(traded, 0u);
    BOOST_CHECK_EQUAL(rested, 0u);
}

// An order held for the Opening cross is filled in it, and the client is told the cross it filled in.
BOOST_AUTO_TEST_CASE(VerifyCrossFillIsFlaggedWithItsCross) {
    std::vector<std::string> sent;
    std::function<void(const char*, std::size_t)> to_engine;
    auto emit = [&](const char* b, std::size_t n) { to_engine(b, n); };
    sim::orderentry::Gateway<decltype(emit)> gateway(emit);
    gateway.allow("ALICE1", "alice00001");

    sim::IpcEventServer events(
        [&](const sim::ipc_ns::trade_message& t) { gateway.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [](const sim::ipc_ns::order_canceled_message&) {},
        [&](const sim::ipc_ns::order_refused_message& j) { gateway.on_event(j); },
        [](const sim::ipc_ns::phase_announced_message&) {},
        [](const sim::ipc_ns::cross_completed_message&) {},
        [](const sim::ipc_ns::imbalance_indicated_message&) {});
    auto event_emit = [&](const char* b, std::size_t n) { events.on_message(std::string_view(b, n)); };
    std::atomic<std::uint64_t> trades{0};
    sim::components::Engine<decltype(event_emit)> engine(event_emit, trades);
    sim::IpcEngineServer ipc_server(engine.day());
    to_engine = [&](const char* b, std::size_t n) { ipc_server.on_message(std::string_view(b, n)); };

    // Every change the day is declared with up to the one that ends the Opening cross, each one made.
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::PreOpen));

    const auto client = gateway.connect([&](const char* b, std::size_t n) { sent.emplace_back(b, n); });
    const auto whole = [](const auto& packet) {
        return std::string(reinterpret_cast<const char*>(&packet), packet.header.packet_length.get().value() + 2);
    };
    // Each answer is a whole packet: its server header, then the message the model lays out.
    const auto answer = [&](std::size_t at) { return reinterpret_cast<const std::byte*>(sent.at(at).data()); };

    // Login, presenting what this protocol's own login carries.
    entry::login_request_packet login{};
    login.fields.username.set("ALICE1");
    login.fields.password.set("alice00001");
    login.fields.requested_session.set("");
    login.fields.requested_sequence_number.set("0");
    gateway.on_bytes(client, whole(login).data(), whole(login).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 1u);
    // The first answer is the login admitted, read by the code the model selects it on.
    BOOST_CHECK_EQUAL(static_cast<std::uint64_t>(entry::server_packet_header::parse(answer(0))->server_packet_type.get().value()), static_cast<std::uint64_t>('A'));

    // An order the gateway admits, answered before the engine sees it
    entry::enter_order_message order{};
    order.fields.user_ref_num.set(7);
    order.fields.side.set(static_cast<std::remove_cvref_t<decltype(order.fields.side.get().value())>>('B'));
    order.fields.quantity.set(100);
    order.fields.price.set(10000);
    order.fields.symbol.set("ZVZZT");
    gateway.on_bytes(client, whole(order).data(), whole(order).size());
    // Accepted at the gateway, then held for the cross.
    BOOST_REQUIRE_EQUAL(sent.size(), 2u);
    // An order of the engine's own for it to cross against.
    engine.day().on_new_order(matching::Order{ 99, matching::Side::Sell, 10000, 40 });
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::Open));
    BOOST_REQUIRE_EQUAL(sent.size(), 3u);
    const auto* filled = entry::order_executed_message::parse(answer(2));
    BOOST_CHECK_EQUAL(filled->fields.user_ref_num.get().value(), 7u);
    BOOST_CHECK_EQUAL(filled->fields.quantity.get().value(), 40u);
    BOOST_CHECK_EQUAL(static_cast<char>(filled->fields.liquidity_flag.get().value()), 'O');
}

// The Opening auction collecting is asked what it would cross at, and says so.
BOOST_AUTO_TEST_CASE(VerifyCollectingAuctionIndicatesItsImbalance) {
    std::vector<sim::ipc_ns::imbalance_indicated_message> indicated;
    sim::IpcEventServer events(
        [](const sim::ipc_ns::trade_message&) {},
        [](const sim::ipc_ns::order_rested_message&) {},
        [](const sim::ipc_ns::order_canceled_message&) {},
        [](const sim::ipc_ns::order_refused_message&) {},
        [](const sim::ipc_ns::phase_announced_message&) {},
        [](const sim::ipc_ns::cross_completed_message&) {},
        [&](const sim::ipc_ns::imbalance_indicated_message& i) { indicated.push_back(i); });
    auto event_emit = [&](const char* b, std::size_t n) { events.on_message(std::string_view(b, n)); };
    std::atomic<std::uint64_t> trades{0};
    sim::components::Engine<decltype(event_emit)> engine(event_emit, trades);
    sim::IpcEngineServer ipc_server(engine.day());
    // The day's question, framed onto the Ipc and served by the engine.
    const auto ask = [&] {
        std::string frame;
        auto emit = [&](const char* b, std::size_t n) { frame.assign(b, n); };
        sim::emit_imbalance_request(emit);
        BOOST_REQUIRE(ipc_server.on_message(frame));
    };

    // Every change the day is declared with up to the one that starts collecting for the Opening auction, each one made.
    BOOST_REQUIRE(sim::change_phase(engine.day(), sim::PhaseChange::PreOpen));

    // Nothing in the book: not enough orders to say a price.
    ask();
    BOOST_REQUIRE_EQUAL(indicated.size(), 1u);
    BOOST_CHECK_EQUAL(static_cast<char>(indicated[0].imbalance_direction.get().value()), 'O');
    BOOST_CHECK_EQUAL(indicated[0].paired_quantity.get().value(), 0u);

    // A buy of 100 and a sell of 40 at one price: 40 pair there, and 60 buyers are left over.
    engine.day().on_new_order(matching::Order{ 1, matching::Side::Buy, 10000, 100 });
    engine.day().on_new_order(matching::Order{ 2, matching::Side::Sell, 10000, 40 });
    ask();
    BOOST_REQUIRE_EQUAL(indicated.size(), 2u);
    BOOST_CHECK_EQUAL(static_cast<char>(indicated[1].cross.get().value()), 'O');
    BOOST_CHECK_EQUAL(indicated[1].price.get().value(), 10000);
    BOOST_CHECK_EQUAL(indicated[1].paired_quantity.get().value(), 40u);
    BOOST_CHECK_EQUAL(indicated[1].imbalance_quantity.get().value(), 60u);
    BOOST_CHECK_EQUAL(static_cast<char>(indicated[1].imbalance_direction.get().value()), 'B');
}

