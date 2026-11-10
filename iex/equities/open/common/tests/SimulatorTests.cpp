#define BOOST_TEST_MODULE sim_tests
#include <boost/test/included/unit_test.hpp>

#include <charconv>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include "MatchingEngine.hpp"
#include "protocols/orderentry/FixWire.hpp"
#include "protocols/orderentry/FixAcceptor.hpp"
#include "SnapServer.hpp"
#include "protocols/marketdata/tops/messages/SystemEventMessage.hpp"
#include "protocols/marketdata/tops/messages/SecurityDirectoryMessage.hpp"
#include "protocols/marketdata/tops/messages/TradingStatusMessage.hpp"
#include "protocols/marketdata/tops/messages/RetailLiquidityIndicatorMessage.hpp"
#include "protocols/marketdata/deep/messages/SystemEventMessage.hpp"
#include "protocols/marketdata/deep/messages/SecurityDirectoryMessage.hpp"
#include "protocols/marketdata/deep/messages/TradingStatusMessage.hpp"
#include "protocols/marketdata/deep/messages/RetailLiquidityIndicatorMessage.hpp"
#include "protocols/marketdata/deepplus/messages/SystemEventMessage.hpp"
#include "protocols/marketdata/deepplus/messages/SecurityDirectoryMessage.hpp"
#include "protocols/marketdata/deepplus/messages/TradingStatusMessage.hpp"
#include "protocols/marketdata/deepplus/messages/RetailLiquidityIndicatorMessage.hpp"
#include "protocols/ipc/structs/HeartbeatMessage.hpp"
#include "protocols/ipc/structs/NewOrderMessage.hpp"
#include "protocols/ipc/structs/CancelOrderMessage.hpp"
#include "protocols/ipc/structs/OrderRestedMessage.hpp"
#include "protocols/ipc/structs/OrderCanceledMessage.hpp"
#include "protocols/ipc/structs/TradeMessage.hpp"
#include "protocols/ipc/structs/OrderRefusedMessage.hpp"
#include "protocols/ipc/structs/PhaseRequestMessage.hpp"
#include "protocols/ipc/structs/PhaseAnnouncedMessage.hpp"
#include "protocols/ipc/structs/CrossCompletedMessage.hpp"
#include "protocols/ipc/structs/ImbalanceRequestMessage.hpp"
#include "protocols/ipc/structs/ImbalanceIndicatedMessage.hpp"
#include "protocols/ipc/IpcBus.hpp"

using namespace iexiexequities::order_entry;

// --- Matching engine ---

BOOST_AUTO_TEST_CASE(matching_crosses_and_generates_trade) {
    int trades = 0;
    matching::Trade last{};
    matching::MatchingEngine engine(
        [&](const matching::Trade& t) { ++trades; last = t; },
        [](const matching::Order&) {});

    engine.on_new_order({1, matching::Side::Sell, 100, 50});   // resting ask @100
    engine.on_new_order({2, matching::Side::Buy,  100, 50});   // marketable buy crosses

    BOOST_CHECK_EQUAL(trades, 1);
    BOOST_CHECK_EQUAL(last.price, 100);
    BOOST_CHECK_EQUAL(last.qty, 50u);
    BOOST_CHECK_EQUAL(last.aggressor_id, 2u);
    BOOST_CHECK_EQUAL(last.resting_id, 1u);
}

BOOST_AUTO_TEST_CASE(matching_rests_when_no_cross) {
    int trades = 0, rests = 0;
    matching::MatchingEngine engine(
        [&](const matching::Trade&) { ++trades; },
        [&](const matching::Order&) { ++rests; });

    engine.on_new_order({1, matching::Side::Buy, 99, 10});     // no ask to cross -> rests
    BOOST_CHECK_EQUAL(trades, 0);
    BOOST_CHECK_EQUAL(rests, 1);
}

BOOST_AUTO_TEST_CASE(matching_cancel_removes_resting_order) {
    matching::MatchingEngine engine(
        [](const matching::Trade&) {},
        [](const matching::Order&) {});

    engine.on_new_order({7, matching::Side::Buy, 99, 10});     // rests
    BOOST_CHECK(engine.on_cancel(7));                          // found + removed
    BOOST_CHECK(!engine.on_cancel(7));                         // gone
}

// --- FIX wire codec (encode/decode round-trip) ---

BOOST_AUTO_TEST_CASE(fix_wire_new_order_round_trip) {
    NewOrderSingle out{};
    out.side      = '1';
    out.price     = 100.25;
    out.order_qty = 25;
    { auto r = std::to_chars(out.cl_ord_id, out.cl_ord_id + sizeof out.cl_ord_id - 1, 42); *r.ptr = '\0'; }

    char body[512];
    std::size_t n = encode(out, body);

    NewOrderSingle in{};
    BOOST_REQUIRE(decode(std::string_view(body, n), in));
    BOOST_CHECK_EQUAL(in.side, '1');
    BOOST_CHECK_CLOSE(in.price, 100.25, 1e-6);
    BOOST_CHECK_CLOSE(in.order_qty, 25.0, 1e-6);
    BOOST_CHECK_EQUAL(std::string_view(in.cl_ord_id, 2), "42");
}
// --- FIX session replay (ResendRequest -> PossDup replay), as the acceptor's chart has it ---

namespace {
struct ResendHandler {
    bool login(std::string_view, std::string_view, std::string&) { return true; }
    void message(std::string_view, std::string_view) {}
    void logout() {}
};
}

BOOST_AUTO_TEST_CASE(fix_acceptor_replays_on_resend_request) {
    unsigned sent = 0, replayed = 0;
    auto transport = [&](const char* b, std::size_t n) {
        ++sent;
        if (std::string_view(b, n).find("\x01" "43=Y") != std::string_view::npos) ++replayed;
    };
    Acceptor<decltype(transport), ResendHandler> session(AcceptorConfig{ "FIX.4.2", "SIM" }, transport, ResendHandler{});

    BOOST_REQUIRE(session.on_message("8=FIX.4.2\x01" "35=A\x01" "34=1\x01" "49=TRADER\x01" "56=SIM\x01" "108=30\x01", 0));   // answered: seq 1
    BOOST_REQUIRE(session.in_logged_on());
    session.message("8", "37=1\x01", 0);   // seq 2
    session.message("8", "37=2\x01", 0);   // seq 3

    BOOST_REQUIRE(session.on_message("8=FIX.4.2\x01" "35=2\x01" "34=2\x01" "7=2\x01" "16=3\x01", 0));   // ResendRequest 2..3

    BOOST_CHECK_EQUAL(sent, 5u);        // 3 originals + 2 replays
    BOOST_CHECK_EQUAL(replayed, 2u);    // seqs 2 and 3, flagged PossDup
}
// --- FIX logon refused, with the handler's reason as the chart has it ---

namespace {
struct RefusingHandler {
    std::string reason;
    bool login(std::string_view, std::string_view, std::string& refusal) { refusal = reason; return false; }
    void message(std::string_view, std::string_view) {}
    void logout() {}
};
}

BOOST_AUTO_TEST_CASE(fix_acceptor_refuses_a_logon_with_its_reason) {
    std::string written;
    auto transport = [&](const char* b, std::size_t n) { written.assign(b, n); };
    Acceptor<decltype(transport), RefusingHandler> session(AcceptorConfig{ "FIX.4.2", "SIM" }, transport, RefusingHandler{ "credentials not recognised" });

    session.on_message("8=FIX.4.2\x01" "35=A\x01" "34=1\x01" "49=TRADER\x01" "56=SIM\x01" "108=30\x01", 0);

    BOOST_CHECK(written.find("\x01" "35=5\x01") != std::string::npos);
    BOOST_CHECK(written.find("\x01" "58=credentials not recognised\x01") != std::string::npos);
    BOOST_CHECK(session.ended());
}

BOOST_AUTO_TEST_CASE(fix_acceptor_refuses_a_logon_without_a_reason) {
    std::string written;
    auto transport = [&](const char* b, std::size_t n) { written.assign(b, n); };
    Acceptor<decltype(transport), RefusingHandler> session(AcceptorConfig{ "FIX.4.2", "SIM" }, transport, RefusingHandler{});

    session.on_message("8=FIX.4.2\x01" "35=A\x01" "34=1\x01" "49=TRADER\x01" "56=SIM\x01" "108=30\x01", 0);

    BOOST_CHECK(written.find("\x01" "35=5\x01") != std::string::npos);
    BOOST_CHECK(written.find("\x01" "58=") == std::string::npos);
    BOOST_CHECK(session.ended());
}
// --- FIX logon for another target, refused by the session before its handler is asked ---

namespace {
struct CountingHandler {
    int* asked;
    bool login(std::string_view, std::string_view, std::string&) { ++*asked; return true; }
    void message(std::string_view, std::string_view) {}
    void logout() {}
};
}

BOOST_AUTO_TEST_CASE(fix_acceptor_refuses_a_logon_for_another_target) {
    std::string written;
    int asked = 0;
    auto transport = [&](const char* b, std::size_t n) { written.assign(b, n); };
    Acceptor<decltype(transport), CountingHandler> session(AcceptorConfig{ "FIX.4.2", "SIM" }, transport, CountingHandler{ &asked });

    session.on_message("8=FIX.4.2\x01" "35=A\x01" "34=1\x01" "49=TRADER\x01" "56=ELSEWHERE\x01" "108=30\x01", 0);

    BOOST_CHECK(written.find("\x01" "35=5\x01") != std::string::npos);
    BOOST_CHECK(session.ended());
    BOOST_CHECK_EQUAL(asked, 0);
}
// --- Snap snapshot (replay) service ---

static std::vector<char> snap_request(std::uint64_t minimum = 0) {
    std::vector<char> req(3 + 40 + 4 + 4 + 8, '\0');
    std::uint16_t len = static_cast<std::uint16_t>(req.size() - 2);
    std::memcpy(req.data(), &len, 2);
    req[2] = snap::MessageType::SnapshotRequest;
    std::memcpy(req.data() + 3 + 40 + 4 + 4, &minimum, 8);
    return req;
}

BOOST_AUTO_TEST_CASE(snap_serves_start_data_end) {
    snap::SnapServer server;
    const char seg1[] = "SEGMENT-ONE";
    const char seg2[] = "SEGMENT-TWO!";
    server.on_segment(seg1, sizeof seg1 - 1);
    server.on_segment(seg2, sizeof seg2 - 1);

    std::vector<char> types;
    std::size_t data_bytes = 0;
    auto req = snap_request();
    bool ok = server.on_message(std::string_view(req.data(), req.size()),
        [&](const char* b, std::size_t n) {
            types.push_back(b[2]);
            if (b[2] == snap::MessageType::SnapshotData) data_bytes += n - 3;
        });

    BOOST_REQUIRE(ok);
    BOOST_REQUIRE_EQUAL(types.size(), 4u);   // s, d, d, x
    BOOST_CHECK_EQUAL(types.front(), snap::MessageType::SnapshotStart);
    BOOST_CHECK_EQUAL(types[1], snap::MessageType::SnapshotData);
    BOOST_CHECK_EQUAL(types[2], snap::MessageType::SnapshotData);
    BOOST_CHECK_EQUAL(types.back(), snap::MessageType::SnapshotEnd);
    BOOST_CHECK_EQUAL(data_bytes, (sizeof seg1 - 1) + (sizeof seg2 - 1));   // segments verbatim
}

BOOST_AUTO_TEST_CASE(snap_rejects_unknown_and_unavailable) {
    snap::SnapServer server;

    // Unknown message type -> ErrorResponse 'U'.
    char bad[4] = { 2, 0, 'z', 0 };
    char reason = 0; char rtype = 0;
    server.on_message(std::string_view(bad, 4),
        [&](const char* b, std::size_t) { rtype = b[2]; reason = b[3]; });
    BOOST_CHECK_EQUAL(rtype, snap::MessageType::ErrorResponse);
    BOOST_CHECK_EQUAL(reason, snap::RejectReason::UnknownMessage);

    // Minimum sequence beyond what the server has -> 'R' Snapshot Not Yet Available.
    auto req = snap_request(/*minimum*/99);
    server.on_message(std::string_view(req.data(), req.size()),
        [&](const char* b, std::size_t) { rtype = b[2]; reason = b[3]; });
    BOOST_CHECK_EQUAL(rtype, snap::MessageType::ErrorResponse);
    BOOST_CHECK_EQUAL(reason, snap::RejectReason::NotYetAvailable);
}
// --- Tops feed messages ---
namespace tops_ns = iex::iexequities::tops::iextp::v1_66;

BOOST_AUTO_TEST_CASE(tops_system_event_message_encodes_type) {
    tops_ns::system_event_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(tops_ns::message_type::enum_type::system_event_message));
    BOOST_CHECK(sizeof m >= sizeof(tops_ns::message_header));
}

BOOST_AUTO_TEST_CASE(tops_security_directory_message_encodes_type) {
    tops_ns::security_directory_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(tops_ns::message_type::enum_type::security_directory_message));
    BOOST_CHECK(sizeof m >= sizeof(tops_ns::message_header));
}

BOOST_AUTO_TEST_CASE(tops_trading_status_message_encodes_type) {
    tops_ns::trading_status_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(tops_ns::message_type::enum_type::trading_status_message));
    BOOST_CHECK(sizeof m >= sizeof(tops_ns::message_header));
}

BOOST_AUTO_TEST_CASE(tops_retail_liquidity_indicator_message_encodes_type) {
    tops_ns::retail_liquidity_indicator_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(tops_ns::message_type::enum_type::retail_liquidity_indicator_message));
    BOOST_CHECK(sizeof m >= sizeof(tops_ns::message_header));
}


// --- Deep feed messages ---
namespace deep_ns = iex::iexequities::deep::iextp::v1_08;

BOOST_AUTO_TEST_CASE(deep_system_event_message_encodes_type) {
    deep_ns::system_event_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deep_ns::message_type::enum_type::system_event_message));
    BOOST_CHECK(sizeof m >= sizeof(deep_ns::message_header));
}

BOOST_AUTO_TEST_CASE(deep_security_directory_message_encodes_type) {
    deep_ns::security_directory_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deep_ns::message_type::enum_type::security_directory_message));
    BOOST_CHECK(sizeof m >= sizeof(deep_ns::message_header));
}

BOOST_AUTO_TEST_CASE(deep_trading_status_message_encodes_type) {
    deep_ns::trading_status_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deep_ns::message_type::enum_type::trading_status_message));
    BOOST_CHECK(sizeof m >= sizeof(deep_ns::message_header));
}

BOOST_AUTO_TEST_CASE(deep_retail_liquidity_indicator_message_encodes_type) {
    deep_ns::retail_liquidity_indicator_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deep_ns::message_type::enum_type::retail_liquidity_indicator_message));
    BOOST_CHECK(sizeof m >= sizeof(deep_ns::message_header));
}


// --- DeepPlus feed messages ---
namespace deepplus_ns = iex::iexequities::deepplus::iextp::v1_05;

BOOST_AUTO_TEST_CASE(deepplus_system_event_message_encodes_type) {
    deepplus_ns::system_event_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deepplus_ns::message_type::enum_type::system_event_message));
    BOOST_CHECK(sizeof m >= sizeof(deepplus_ns::message_header));
}

BOOST_AUTO_TEST_CASE(deepplus_security_directory_message_encodes_type) {
    deepplus_ns::security_directory_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deepplus_ns::message_type::enum_type::security_directory_message));
    BOOST_CHECK(sizeof m >= sizeof(deepplus_ns::message_header));
}

BOOST_AUTO_TEST_CASE(deepplus_trading_status_message_encodes_type) {
    deepplus_ns::trading_status_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deepplus_ns::message_type::enum_type::trading_status_message));
    BOOST_CHECK(sizeof m >= sizeof(deepplus_ns::message_header));
}

BOOST_AUTO_TEST_CASE(deepplus_retail_liquidity_indicator_message_encodes_type) {
    deepplus_ns::retail_liquidity_indicator_message m{};
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&m);
    BOOST_CHECK_EQUAL(raw[2], static_cast<unsigned char>(deepplus_ns::message_type::enum_type::retail_liquidity_indicator_message));
    BOOST_CHECK(sizeof m >= sizeof(deepplus_ns::message_header));
}


// --- Exchange-internal Ipc messages ---
namespace ipc_ns = iex::iexequities::matchingengine::ipc::v1_0;

BOOST_AUTO_TEST_CASE(ipc_heartbeat_message_compiles_and_parses) {
    ipc_ns::heartbeat_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::heartbeat_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_new_order_message_compiles_and_parses) {
    ipc_ns::new_order_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::new_order_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_cancel_order_message_compiles_and_parses) {
    ipc_ns::cancel_order_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::cancel_order_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_order_rested_message_compiles_and_parses) {
    ipc_ns::order_rested_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::order_rested_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_order_canceled_message_compiles_and_parses) {
    ipc_ns::order_canceled_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::order_canceled_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_trade_message_compiles_and_parses) {
    ipc_ns::trade_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::trade_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_order_refused_message_compiles_and_parses) {
    ipc_ns::order_refused_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::order_refused_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_phase_request_message_compiles_and_parses) {
    ipc_ns::phase_request_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::phase_request_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_phase_announced_message_compiles_and_parses) {
    ipc_ns::phase_announced_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::phase_announced_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_cross_completed_message_compiles_and_parses) {
    ipc_ns::cross_completed_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::cross_completed_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_imbalance_request_message_compiles_and_parses) {
    ipc_ns::imbalance_request_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::imbalance_request_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

BOOST_AUTO_TEST_CASE(ipc_imbalance_indicated_message_compiles_and_parses) {
    ipc_ns::imbalance_indicated_message m{};
    auto* buffer = reinterpret_cast<std::byte*>(&m);
    BOOST_CHECK(ipc_ns::imbalance_indicated_message::parse(buffer) == &m);
    BOOST_CHECK(sizeof m > 0u);
}

// Gateway -> engine over the Ipc bus: orders encode to framed ipc messages, the
// server decodes and drives the engine, and a cross generates a trade.
BOOST_AUTO_TEST_CASE(ipc_bus_round_trip_matches_and_cancels) {
    int trades = 0, rests = 0;
    matching::MatchingEngine engine(
        [&](const matching::Trade&) { ++trades; },
        [&](const matching::Order&) { ++rests; });

    sim::IpcEngineServer server(engine);
    std::uint64_t frames = 0;
    auto emit = [&](const char* b, std::size_t n) {
        ++frames;
        BOOST_REQUIRE(server.on_message(std::string_view(b, n)));
    };
    sim::IpcEngineClient client(emit);

    client.on_new_order({1, matching::Side::Sell, 100, 50});   // rests
    client.on_new_order({2, matching::Side::Buy,  100, 50});   // crosses
    client.on_cancel(1);                                        // nothing left to cancel

    BOOST_CHECK_EQUAL(frames, 3u);
    BOOST_CHECK_EQUAL(trades, 1);
    BOOST_CHECK_EQUAL(rests, 1);
}