#define BOOST_TEST_MODULE sim_tests_fix
#include <boost/test/included/unit_test.hpp>

#include <charconv>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include "protocols/orderentry/fix/FixWire.hpp"
#include "protocols/orderentry/fix/FixAcceptor.hpp"

using namespace nasdaqnsmequities::order_entry;

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
