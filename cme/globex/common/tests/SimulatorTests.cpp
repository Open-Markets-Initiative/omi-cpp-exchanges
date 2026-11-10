#define BOOST_TEST_MODULE sim_tests
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include "MatchingEngine.hpp"
#include "protocols/orderentry/Gateway.hpp"

namespace entry = ::cme::ilink3::v9;

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
// What a match filled each resting order, and in what order they were filled.
struct Fills {
    std::vector<std::uint64_t> order;
    std::vector<std::pair<std::uint64_t, std::uint64_t>> filled;
    void on(const matching::Trade& t) { order.push_back(t.resting_id); filled.emplace_back(t.resting_id, t.qty); }
    std::uint64_t of(std::uint64_t id) const {
        std::uint64_t qty = 0;
        for (const auto& [resting, lots] : filled) if (resting == id) qty += lots;
        return qty;
    }
    std::uint64_t total() const {
        std::uint64_t qty = 0;
        for (const auto& fill : filled) qty += fill.second;
        return qty;
    }
};

// Cme's Pro Rata example (CME Globex Matching Algorithm Steps): six buys at one price, earliest
// first, and a sell of 50 against them with a Pro Rata minimum of 2. Each is allocated its share
// of the level, rounded down - 0, 1, 7, 0, 3, 37 - and the 1 is below the minimum, so is none.
// What rounding leaves, 3, goes in time order to the first. No order is TOP here: none is as
// large as the TOP minimum the book is given, so this is the Pro Rata step as the example has it.
BOOST_AUTO_TEST_CASE(VerifyProRataAllocatesEachShareAboveTheMinimum) {
    Fills fills;
    matching::MatchingEngine engine([&](const matching::Trade& t) { fills.on(t); }, [](const matching::Order&) {});
    engine.allocate(matching::Allocation{ matching::Algorithm::allocation, 2, 1000, 0 });
    const std::uint64_t working[] = { 5, 9, 57, 4, 28, 300 };
    for (std::uint64_t at = 0; at < 6; ++at) engine.on_new_order({ at + 1, matching::Side::Buy, 100, working[at] });

    engine.on_new_order({ 99, matching::Side::Sell, 100, 50 });

    BOOST_CHECK_EQUAL(fills.of(1), 3u);    // 0 by share, then the 3 rounding left, first in time
    BOOST_CHECK_EQUAL(fills.of(2), 0u);    // 1 by share, below the minimum
    BOOST_CHECK_EQUAL(fills.of(3), 7u);
    BOOST_CHECK_EQUAL(fills.of(4), 0u);
    BOOST_CHECK_EQUAL(fills.of(5), 3u);
    BOOST_CHECK_EQUAL(fills.of(6), 37u);
    BOOST_CHECK_EQUAL(fills.total(), 50u);
}

// TOP first: the order that bettered the market takes what it can before the rest of the level is
// shared. A buy of 10 opens the bid and is TOP; a buy of 300 joins it and is not. A sell of 20 fills
// the TOP order's 10, and the 10 left are shared by Pro Rata - all of it to the one order left.
BOOST_AUTO_TEST_CASE(VerifyTopOrderIsFilledBeforeTheLevelIsShared) {
    Fills fills;
    matching::MatchingEngine engine([&](const matching::Trade& t) { fills.on(t); }, [](const matching::Order&) {});
    engine.allocate(matching::Allocation{ matching::Algorithm::allocation, 2, 1, 0 });
    engine.on_new_order({ 1, matching::Side::Buy, 100, 10 });
    engine.on_new_order({ 2, matching::Side::Buy, 100, 300 });

    engine.on_new_order({ 99, matching::Side::Sell, 100, 20 });

    BOOST_CHECK_EQUAL(fills.of(1), 10u);
    BOOST_CHECK_EQUAL(fills.of(2), 10u);
}

// Cme's FIFO exception: an aggressor at least as large as all that rests at a price takes the level
// in time order, whatever the algorithm. A sell of 400 against the six buys of the Pro Rata example,
// 403 lots between them, would be shared; one of 403 is not.
BOOST_AUTO_TEST_CASE(VerifyAggressorTakingTheWholeLevelIsFilledInTimeOrder) {
    Fills fills;
    matching::MatchingEngine engine([&](const matching::Trade& t) { fills.on(t); }, [](const matching::Order&) {});
    engine.allocate(matching::Allocation{ matching::Algorithm::allocation, 2, 1000, 0 });
    const std::uint64_t working[] = { 5, 9, 57, 4, 28, 300 };
    for (std::uint64_t at = 0; at < 6; ++at) engine.on_new_order({ at + 1, matching::Side::Buy, 100, working[at] });

    engine.on_new_order({ 99, matching::Side::Sell, 100, 403 });

    BOOST_REQUIRE_EQUAL(fills.order.size(), 6u);
    for (std::uint64_t at = 0; at < 6; ++at) {
        BOOST_CHECK_EQUAL(fills.order[at], at + 1);   // each in the order it came
        BOOST_CHECK_EQUAL(fills.of(at + 1), working[at]);
    }
}

// A client logs in, enters an order, is filled by the engine and cancels what is left.
BOOST_AUTO_TEST_CASE(gateway_admits_fills_and_cancels_an_order) {
    std::vector<std::string> sent;
    std::function<void(const char*, std::size_t)> to_engine;
    auto emit = [&](const char* b, std::size_t n) { to_engine(b, n); };
    sim::orderentry::Gateway<decltype(emit)> gateway(emit);
    gateway.allow("ABC", "TESTF");

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
        return std::string(reinterpret_cast<const char*>(&packet), packet.header.message_length.get().value());
    };
    // Each answer is a whole packet: its server header, then the message the model lays out.
    const auto answer = [&](std::size_t at) { return reinterpret_cast<const std::byte*>(sent.at(at).data()); };

    // Login, presenting what this protocol's own login carries.
    entry::negotiate login{};
    login.fields.session.set("ABC");
    login.fields.firm.set("TESTF");
    gateway.on_bytes(client, whole(login).data(), whole(login).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 1u);
    // The first answer is the login admitted, read by the code the model selects it on.
    BOOST_CHECK_EQUAL(static_cast<std::uint64_t>(entry::message_header::parse(answer(0) + 4)->template_id.get().value()), static_cast<std::uint64_t>(501));
    // Then established: the negotiated session bound to this connection, numbering from 1.
    entry::establish establish{};
    establish.fields.next_seq_no.set(1);
    gateway.on_bytes(client, whole(establish).data(), whole(establish).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 2u);
    BOOST_CHECK_EQUAL(static_cast<std::uint64_t>(entry::message_header::parse(answer(1) + 4)->template_id.get().value()), static_cast<std::uint64_t>(504));

    // An order the gateway admits, answered before the engine sees it
    entry::new_order_single order{};
    order.fields.clordid.set("7");
    order.fields.seq_num.set(1);
    order.fields.side.set(static_cast<std::remove_cvref_t<decltype(order.fields.side.get().value())>>(1));
    order.fields.order_qty.set(100);
    order.fields.price_optional.set(10000);
    order.fields.security_id.set(sim::instrument_id);
    gateway.on_bytes(client, whole(order).data(), whole(order).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 3u);
    BOOST_CHECK_EQUAL(gateway.open_orders(), 1u);
    const auto* accepted = entry::execution_report_new::parse(answer(2));
    BOOST_CHECK_EQUAL(std::string(accepted->fields.clordid.get_trimmed().value()), "7");

    // The engine fills part of it against an order of its own
    engine.on_new_order(matching::Order{ 99, matching::Side::Sell, 10000, 40 });
    BOOST_REQUIRE_EQUAL(sent.size(), 4u);
    const auto* filled = entry::execution_report_trade_outright::parse(answer(3));
    BOOST_CHECK_EQUAL(std::string(filled->fields.clordid.get_trimmed().value()), "7");
    BOOST_CHECK_EQUAL(filled->fields.last_qty.get().value(), 40u);

    // The client cancels, and is told what was left
    entry::order_cancel_request request{};
    request.fields.clordid.set("7");
    request.fields.seq_num.set(2);
    gateway.on_bytes(client, whole(request).data(), whole(request).size());
    BOOST_REQUIRE_EQUAL(sent.size(), 5u);
    const auto* gone = entry::execution_report_cancel::parse(answer(4));
    BOOST_CHECK_EQUAL(std::string(gone->fields.clordid.get_trimmed().value()), "7");
    BOOST_CHECK_EQUAL(gateway.open_orders(), 0u);
}

