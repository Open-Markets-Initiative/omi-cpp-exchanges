#define BOOST_TEST_MODULE sim_tests
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include "MatchingEngine.hpp"
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
        [&](const sim::ipc_ns::order_canceled_message& c) { gateway.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gateway.on_event(a); },
        [](const sim::ipc_ns::order_eliminated_message&) {},
        [](const sim::ipc_ns::order_refused_message&) {},
        [](const sim::ipc_ns::order_replaced_message&) {},
        [](const sim::ipc_ns::order_reduced_message&) {});
    auto event_emit = [&](const char* b, std::size_t n) { events.on_message(std::string_view(b, n)); };
    matching::MatchingEngine engine(
        [&](const matching::Trade& t)  { sim::emit_trade(event_emit, t); },
        [&](const matching::Order& o) { sim::emit_rested(event_emit, o); },
        [&](const matching::Order& o) { sim::emit_canceled(event_emit, o); },
        [&](const matching::Order& o) { sim::emit_eliminated(event_emit, o); },
        [&](const matching::Order& o) { sim::emit_accepted(event_emit, o); });
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

    // An order the gateway admits, answered once the engine has it
    entry::enter_order_message order{};
    order.fields.user_ref_num.set(7);
    order.fields.side.set(static_cast<std::remove_cvref_t<decltype(order.fields.side.get().value())>>('B'));
    order.fields.quantity.set(100);
    order.fields.price.set(10000);
    order.fields.symbol.set("ZVZZT");
    order.fields.time_in_force.set(static_cast<std::remove_cvref_t<decltype(order.fields.time_in_force.get().value())>>('0'));
    order.fields.display.set(static_cast<std::remove_cvref_t<decltype(order.fields.display.get().value())>>('Y'));
    order.fields.cross_type.set(static_cast<std::remove_cvref_t<decltype(order.fields.cross_type.get().value())>>('N'));
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

// MakerBasic buys through its own gateway and TakerBasic sells into it through another: the engine
// tells both gateways of every event, and each answers only the orders it numbered.
BOOST_AUTO_TEST_CASE(VerifyEachTradersGatewayAnswersOnlyItsOwnOrders) {
    std::function<void(const char*, std::size_t)> to_engine;
    // The two orders' numbers, as the trade the engine makes carries them.
    std::uint64_t resting = 0, aggressor = 0;

    // MakerBasic's gateway, gateway 1 of 4: it numbers 1, 5, 9 ...
    std::vector<std::string> makerbasic_sent;
    auto gatewaymakerbasic_emit = [&](const char* b, std::size_t n) { to_engine(b, n); };
    sim::orderentry::Gateway<decltype(gatewaymakerbasic_emit)> gatewaymakerbasic(gatewaymakerbasic_emit, 1, 4);
    gatewaymakerbasic.allow("ALICE1", "alice00001");
    gatewaymakerbasic.allow("BOB001", "bob0000001");
    gatewaymakerbasic.allow("CAROL1", "carol00001");
    gatewaymakerbasic.allow("DAVE01", "dave000001");
    sim::IpcEventServer gatewaymakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { resting = t.resting_order_id.get().value(); aggressor = t.aggressor_order_id.get().value(); gatewaymakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaymakerbasic.on_event(a); },
        [](const sim::ipc_ns::order_eliminated_message&) {},
        [](const sim::ipc_ns::order_refused_message&) {},
        [](const sim::ipc_ns::order_replaced_message&) {},
        [](const sim::ipc_ns::order_reduced_message&) {});

    // TakerBasic's gateway, gateway 2 of 4: it numbers 2, 6, 10 ...
    std::vector<std::string> takerbasic_sent;
    auto gatewaytakerbasic_emit = [&](const char* b, std::size_t n) { to_engine(b, n); };
    sim::orderentry::Gateway<decltype(gatewaytakerbasic_emit)> gatewaytakerbasic(gatewaytakerbasic_emit, 2, 4);
    gatewaytakerbasic.allow("ALICE1", "alice00001");
    gatewaytakerbasic.allow("BOB001", "bob0000001");
    gatewaytakerbasic.allow("CAROL1", "carol00001");
    gatewaytakerbasic.allow("DAVE01", "dave000001");
    sim::IpcEventServer gatewaytakerbasic_events(
        [&](const sim::ipc_ns::trade_message& t) { gatewaytakerbasic.on_event(t); },
        [](const sim::ipc_ns::order_rested_message&) {},
        [&](const sim::ipc_ns::order_canceled_message& c) { gatewaytakerbasic.on_event(c); },
        [&](const sim::ipc_ns::order_accepted_message& a) { gatewaytakerbasic.on_event(a); },
        [](const sim::ipc_ns::order_eliminated_message&) {},
        [](const sim::ipc_ns::order_refused_message&) {},
        [](const sim::ipc_ns::order_replaced_message&) {},
        [](const sim::ipc_ns::order_reduced_message&) {});

    // The engine's events, told to every gateway.
    auto event_emit = [&](const char* b, std::size_t n) {
        gatewaymakerbasic_events.on_message(std::string_view(b, n));
        gatewaytakerbasic_events.on_message(std::string_view(b, n));
    };
    matching::MatchingEngine engine(
        [&](const matching::Trade& t)  { sim::emit_trade(event_emit, t); },
        [&](const matching::Order& o) { sim::emit_rested(event_emit, o); },
        [&](const matching::Order& o) { sim::emit_canceled(event_emit, o); },
        [&](const matching::Order& o) { sim::emit_eliminated(event_emit, o); },
        [&](const matching::Order& o) { sim::emit_accepted(event_emit, o); });
    sim::IpcEngineServer ipc_server(engine);
    to_engine = [&](const char* b, std::size_t n) { ipc_server.on_message(std::string_view(b, n)); };

    const auto makerbasic_client = gatewaymakerbasic.connect([&](const char* b, std::size_t n) { makerbasic_sent.emplace_back(b, n); });
    const auto whole = [](const auto& packet) {
        return std::string(reinterpret_cast<const char*>(&packet), packet.header.packet_length.get().value() + 2);
    };
    // Each answer is a whole packet: its server header, then the message the model lays out.
    const auto makerbasic_answer = [&](std::size_t at) { return reinterpret_cast<const std::byte*>(makerbasic_sent.at(at).data()); };

    // Login, presenting what this protocol's own login carries.
    entry::login_request_packet makerbasic_login{};
    makerbasic_login.fields.username.set("ALICE1");
    makerbasic_login.fields.password.set("alice00001");
    makerbasic_login.fields.requested_session.set("");
    makerbasic_login.fields.requested_sequence_number.set("0");
    gatewaymakerbasic.on_bytes(makerbasic_client, whole(makerbasic_login).data(), whole(makerbasic_login).size());
    BOOST_REQUIRE_EQUAL(makerbasic_sent.size(), 1u);
    // The first answer is the login admitted, read by the code the model selects it on.
    BOOST_CHECK_EQUAL(static_cast<std::uint64_t>(entry::server_packet_header::parse(makerbasic_answer(0))->server_packet_type.get().value()), static_cast<std::uint64_t>('A'));

    const auto takerbasic_client = gatewaytakerbasic.connect([&](const char* b, std::size_t n) { takerbasic_sent.emplace_back(b, n); });
    // Each answer is a whole packet: its server header, then the message the model lays out.
    const auto takerbasic_answer = [&](std::size_t at) { return reinterpret_cast<const std::byte*>(takerbasic_sent.at(at).data()); };

    // Login, presenting what this protocol's own login carries.
    entry::login_request_packet takerbasic_login{};
    takerbasic_login.fields.username.set("BOB001");
    takerbasic_login.fields.password.set("bob0000001");
    takerbasic_login.fields.requested_session.set("");
    takerbasic_login.fields.requested_sequence_number.set("0");
    gatewaytakerbasic.on_bytes(takerbasic_client, whole(takerbasic_login).data(), whole(takerbasic_login).size());
    BOOST_REQUIRE_EQUAL(takerbasic_sent.size(), 1u);
    // The first answer is the login admitted, read by the code the model selects it on.
    BOOST_CHECK_EQUAL(static_cast<std::uint64_t>(entry::server_packet_header::parse(takerbasic_answer(0))->server_packet_type.get().value()), static_cast<std::uint64_t>('A'));

    // MakerBasic buys a hundred, and only its own gateway answers.
    // An order the gateway admits, answered once the engine has it
    entry::enter_order_message buy{};
    buy.fields.user_ref_num.set(7);
    buy.fields.side.set(static_cast<std::remove_cvref_t<decltype(buy.fields.side.get().value())>>('B'));
    buy.fields.quantity.set(100);
    buy.fields.price.set(10000);
    buy.fields.symbol.set("ZVZZT");
    buy.fields.time_in_force.set(static_cast<std::remove_cvref_t<decltype(buy.fields.time_in_force.get().value())>>('0'));
    buy.fields.display.set(static_cast<std::remove_cvref_t<decltype(buy.fields.display.get().value())>>('Y'));
    buy.fields.cross_type.set(static_cast<std::remove_cvref_t<decltype(buy.fields.cross_type.get().value())>>('N'));
    gatewaymakerbasic.on_bytes(makerbasic_client, whole(buy).data(), whole(buy).size());
    BOOST_REQUIRE_EQUAL(makerbasic_sent.size(), 2u);
    BOOST_REQUIRE_EQUAL(takerbasic_sent.size(), 1u);
    BOOST_CHECK_EQUAL(gatewaymakerbasic.open_orders(), 1u);
    BOOST_CHECK_EQUAL(gatewaytakerbasic.open_orders(), 0u);

    // TakerBasic sells forty into it, and each gateway tells its own trader of its side of the fill.
    // An order the gateway admits, answered once the engine has it
    entry::enter_order_message sell{};
    sell.fields.user_ref_num.set(8);
    sell.fields.side.set(static_cast<std::remove_cvref_t<decltype(sell.fields.side.get().value())>>('S'));
    sell.fields.quantity.set(40);
    sell.fields.price.set(10000);
    sell.fields.symbol.set("ZVZZT");
    sell.fields.time_in_force.set(static_cast<std::remove_cvref_t<decltype(sell.fields.time_in_force.get().value())>>('0'));
    sell.fields.display.set(static_cast<std::remove_cvref_t<decltype(sell.fields.display.get().value())>>('Y'));
    sell.fields.cross_type.set(static_cast<std::remove_cvref_t<decltype(sell.fields.cross_type.get().value())>>('N'));
    gatewaytakerbasic.on_bytes(takerbasic_client, whole(sell).data(), whole(sell).size());
    BOOST_REQUIRE_EQUAL(makerbasic_sent.size(), 3u);
    BOOST_REQUIRE_EQUAL(takerbasic_sent.size(), 3u);
    const auto* bought = entry::order_executed_message::parse(makerbasic_answer(2));
    BOOST_CHECK_EQUAL(bought->fields.user_ref_num.get().value(), 7u);
    BOOST_CHECK_EQUAL(bought->fields.quantity.get().value(), 40u);
    const auto* sold = entry::order_executed_message::parse(takerbasic_answer(2));
    BOOST_CHECK_EQUAL(sold->fields.user_ref_num.get().value(), 8u);
    BOOST_CHECK_EQUAL(sold->fields.quantity.get().value(), 40u);
    // MakerBasic's buy rests with sixty left; TakerBasic's sell is done.
    BOOST_CHECK_EQUAL(gatewaymakerbasic.open_orders(), 1u);
    BOOST_CHECK_EQUAL(gatewaytakerbasic.open_orders(), 0u);

    // Each order carries the number its own gateway gave it: each gateway's first.
    BOOST_CHECK_EQUAL(resting, 1u);
    BOOST_CHECK_EQUAL(aggressor, 2u);
}

