// Generated IEX exchange simulator — entry point.
//
// Wiring (in-process):
//   SimpleMaker / SimpleTaker --> Orders adapter --(encoded FIX)--> Gateway --> MatchingEngine
//   MatchingEngine trades --> stdout + DEEP feed --> market-data recorder
//   Orders --> FIX record log

#include "FixWire.hpp"
#include "FixSession.hpp"
#include "FixGateway.hpp"
#include "FixOrders.hpp"
#include "FixStrategies.hpp"
#include "MatchingEngine.hpp"
#include "FeedServer.hpp"
#include "FixLog.hpp"
#include "MarketDataLog.hpp"
#include "SnapServer.hpp"
#include "SimulatorConfiguration.hpp"

#include <charconv>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include "protocols/marketdata/deep/messages/TradeReportMessage.hpp"
#include "protocols/marketdata/deep/messages/PriceLevelBuyUpdateMessage.hpp"
#include "protocols/marketdata/deep/messages/PriceLevelSellUpdateMessage.hpp"

#include "protocols/marketdata/tops/messages/QuoteUpdateMessage.hpp"

#include "protocols/ipc/IpcBus.hpp"
#include "protocols/ipc/IpcTransforms.hpp"

#include "net/Network.hpp"

using namespace iex::order_entry;

int main(int argc, char** argv) {
    // Runtime endpoints: simulator.ini (or --config <file>) decides IPs/ports at process start.
    sim::RuntimeConfig cfg = sim::load_config(argc, argv);

    std::puts("iex-simulator configuration (runtime simulator.ini):");
    std::printf("  order-entry  %s:%u\n", cfg.order_entry.host.c_str(), (unsigned)cfg.order_entry.port);
    std::printf("  feed Tops      %s:%u\n", cfg.tops_feed.host.c_str(), (unsigned)cfg.tops_feed.port);
    std::printf("  feed Deep      %s:%u\n", cfg.deep_feed.host.c_str(), (unsigned)cfg.deep_feed.port);
    std::printf("  feed DeepPlus  %s:%u\n", cfg.deepplus_feed.host.c_str(), (unsigned)cfg.deepplus_feed.port);
    std::printf("  snap Deep      port %u\n", (unsigned)cfg.deep_snap_port);
    std::printf("  snap Tops      port %u\n", (unsigned)cfg.tops_snap_port);
    std::printf("  snap DeepPlus  port %u\n", (unsigned)cfg.deepplus_snap_port);
    std::printf("  recording    dir=%s fix-log=%s market-data=%s\n", cfg.recording_directory.c_str(), "on", "on");

    // FIX record log (bracketed header + raw message). Empty path = disabled.
    sim::FixLog fixlog("recordings/fix/session.log");
    std::uint32_t out_seq = 0, in_seq = 0;

    // Market-data feed servers — segments tee to the recorder, Snap server, and real UDP multicast.
    sim::MarketDataLog tops_rec("recordings/marketdata/Tops.dat");
    snap::SnapServer tops_snap;
    net::UdpMulticastPublisher tops_mc(cfg.tops_feed.host, cfg.tops_feed.port);
    feed::FeedServer tops(0x8003, 1, [&](const char* s, std::size_t n) { tops_rec(s, n); tops_snap.on_segment(s, n); tops_mc(s, n); });
    sim::MarketDataLog deep_rec("recordings/marketdata/Deep.dat");
    snap::SnapServer deep_snap;
    net::UdpMulticastPublisher deep_mc(cfg.deep_feed.host, cfg.deep_feed.port);
    feed::FeedServer deep(0x8004, 1, [&](const char* s, std::size_t n) { deep_rec(s, n); deep_snap.on_segment(s, n); deep_mc(s, n); });
    sim::MarketDataLog deepplus_rec("recordings/marketdata/DeepPlus.dat");
    snap::SnapServer deepplus_snap;
    net::UdpMulticastPublisher deepplus_mc(cfg.deepplus_feed.host, cfg.deepplus_feed.port);
    feed::FeedServer deepplus(0x8005, 1, [&](const char* s, std::size_t n) { deepplus_rec(s, n); deepplus_snap.on_segment(s, n); deepplus_mc(s, n); });

    std::uint64_t trades = 0;
    // Event consumers: decode engine events off the Ipc bus -> market data + exec reports.
    sim::IpcEventServer events(
        [&](const sim::ipc_ns::trade_message& t) {
            ++trades;
            std::printf("TRADE aggressor=%llu resting=%llu px=%lld qty=%llu\n",
                        (unsigned long long)t.aggressor_order_id.get().value(), (unsigned long long)t.resting_order_id.get().value(),
                        (long long)t.price.get().value(), (unsigned long long)t.quantity.get().value());
            // DEEP Trade Report via the declared transform (IexEquities.Ipc.Transforms.xml).
            auto dm = sim::to_deep_trade(t);
            deep.publish_message(reinterpret_cast<const char*>(&dm), sizeof dm);
            deep.flush();
            // DEEP+ Trade (order-level) via the declared transform.
            auto dpt = sim::to_deepplus_trade(t);
            deepplus.publish_message(reinterpret_cast<const char*>(&dpt), sizeof dpt);
            deepplus.flush();

            // Fill -> ExecutionReport (35=8) back to the client, logged inbound ('<').
            ExecutionReport er{};
            { auto r = std::to_chars(er.cl_ord_id, er.cl_ord_id + sizeof er.cl_ord_id - 1, t.aggressor_order_id.get().value()); *r.ptr = '\0'; }
            er.ord_status = '2';  er.exec_type = 'F';   // Filled / Trade
            er.price = static_cast<double>(t.price.get().value());  er.order_qty = static_cast<double>(t.quantity.get().value());
            char erbody[512];
            std::size_t ern = encode(er, erbody);
            fixlog.record('<', ++in_seq, '8', std::string_view(erbody, ern));
        },
        // Rest -> DEEP Price Level Update + TOPS Quote Update -> feeds + recorders.
        [&](const sim::ipc_ns::order_rested_message& o) {
            // Via the declared transforms (Ipc.Transforms.xml).
            if (o.side.get().value() == 'B') {
                auto u = sim::to_deep_price_level_buy(o);
                deep.publish_message(reinterpret_cast<const char*>(&u), sizeof u);
            } else {
                auto u = sim::to_deep_price_level_sell(o);
                deep.publish_message(reinterpret_cast<const char*>(&u), sizeof u);
            }
            deep.flush();
            auto q = o.side.get().value() == 'B' ? sim::to_tops_quote_bid(o) : sim::to_tops_quote_ask(o);
            tops.publish_message(reinterpret_cast<const char*>(&q), sizeof q);
            tops.flush();
            // DEEP+ Add Order (order-level) via the declared transform.
            auto ao = sim::to_deepplus_add_order(o);
            deepplus.publish_message(reinterpret_cast<const char*>(&ao), sizeof ao);
            deepplus.flush();
        });

    // Engine -> consumers over the Ipc bus: events encode as framed ipc messages.
    std::uint64_t ipc_events = 0;
    auto event_emit = [&](const char* b, std::size_t n) {
        ++ipc_events;
        events.on_message(std::string_view(b, n));
    };
    matching::MatchingEngine engine(
        [&](const matching::Trade& t)  { sim::emit_trade(event_emit, t); },
        [&](const matching::Order& o) { sim::emit_rested(event_emit, o); });

    // Gateway -> engine over the Ipc bus: calls encode as framed ipc messages
    // (the generated ipc structs); the server decodes and drives the engine.
    std::uint64_t ipc_msgs = 0;
    sim::IpcEngineServer ipc_server(engine);
    sim::IpcEngineClient ipc_client([&](const char* b, std::size_t n) {
        ++ipc_msgs;
        ipc_server.on_message(std::string_view(b, n));
    });
    Gateway gateway(ipc_client, [](char, std::string_view) {});

    // Orders adapter: log each outbound message, then route it into the gateway.
    Orders orders([&](char type, std::string_view body) {
        fixlog.record('>', ++out_seq, type, body);
        gateway.on_app(type, body);
    });

// Two strategies drive order flow through the same adapter: a 3-level maker quoting
// around 10000 with a 10-tick spread, and a taker crossing 20 ticks every 3 steps.
SimpleMaker<decltype(orders)> maker(/*mid*/10000, /*spread*/10, /*size*/100, /*levels*/3);
SimpleTaker<decltype(orders)> taker(/*mid*/10000, /*cross*/20, /*size*/50,  /*period*/3);

for (int tick = 0; tick < 20; ++tick) {
    maker.step(orders);
    taker.step(orders);
}

std::printf("iex-simulator: ran 20 ticks (maker+taker), %llu trades matched; MD segments recorded\n",
            (unsigned long long)trades);
    std::printf("iex-simulator: ipc bus carried %llu order + %llu event messages\n", (unsigned long long)ipc_msgs, (unsigned long long)ipc_events);
    // Snap (replay) self-check: SnapshotRequest -> Start + Data* + End.
    {
        std::uint64_t served = 0, data_msgs = 0;
        char req[3 + 40 + 4 + 4 + 8] = {};
        std::uint16_t rlen = static_cast<std::uint16_t>(sizeof req - 2);
        std::memcpy(req, &rlen, 2);
        req[2] = snap::MessageType::SnapshotRequest;   // token/channel/session zeroed; min seq 0
        deep_snap.on_message(std::string_view(req, sizeof req), [&](const char* b, std::size_t n) {
            ++served;
            if (n >= 3 && b[2] == snap::MessageType::SnapshotData) ++data_msgs;
        });
        std::printf("iex-simulator: snap replay served %llu messages (%llu segments) from Deep\n",
                    (unsigned long long)served, (unsigned long long)data_msgs);
    }

    // --serve: live venue. Tcp order entry + snapshot servers; Udp multicast market data.
    bool serve = false;
    for (int i = 1; i < argc; ++i) if (std::string_view(argv[i]) == "--serve") serve = true;
    if (serve) {
        std::signal(SIGPIPE, SIG_IGN);   // a client hanging up must not kill the venue
        net::TcpServer order_entry(cfg.order_entry.host, cfg.order_entry.port);
        net::TcpServer deep_snap_srv("0.0.0.0", cfg.deep_snap_port);
        net::TcpServer tops_snap_srv("0.0.0.0", cfg.tops_snap_port);
        net::TcpServer deepplus_snap_srv("0.0.0.0", cfg.deepplus_snap_port);
        std::puts("iex-simulator: serving — order-entry + snapshot Tcp, market-data multicast (Ctrl-C to stop)");
        for (;;) {
            // Keep the market moving so multicast + snapshots stay live.
            maker.step(orders);
            taker.step(orders);

            // External order entry over Tcp -> the gateway (decode ignores the framing tags).
            order_entry.poll([&](int, std::string_view msg) {
                char type = 'D';
                auto at = msg.find("\x01" "35=");
                if (at != std::string_view::npos && at + 4 < msg.size()) type = msg[at + 4];
                else if (msg.size() > 3 && msg.compare(0, 3, "35=") == 0) type = msg[3];
                gateway.on_app(type, msg);
            });

            // External snapshot requests over Tcp -> the feed's Snap server.
            deep_snap_srv.poll([&](int fd, std::string_view request) { deep_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); }); });
            tops_snap_srv.poll([&](int fd, std::string_view request) { tops_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); }); });
            deepplus_snap_srv.poll([&](int fd, std::string_view request) { deepplus_snap.on_message(request, [&](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); }); });
            ::usleep(50000);   // 20 Hz
        }
    }
    return 0;
}
