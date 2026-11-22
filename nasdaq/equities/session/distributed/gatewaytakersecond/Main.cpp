// Generated Nasdaq Stock Market Session order-entry gateway — one component, one process.
// 
// The sessions it serves and the orders it admits are the same ones the single binary
// serves; only its links differ, and those are sockets, dialled where the configuration says each component is.

#include "protocols/orderentry/Gateway.hpp"
#include "protocols/ipc/IpcBus.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
#include "run/SocketLink.hpp"
#include "net/Network.hpp"
#include "SimulatorConfiguration.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <map>
#include <memory>
#include <string_view>
#include <thread>

namespace {
// Interrupted by the launcher stopping the venue; the handler only asks.
std::atomic<bool> running{true};
void on_interrupt(int) { running.store(false, std::memory_order_release); }
}

int main(int argc, char** argv) {
    // A client hanging up must not kill the venue.
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, on_interrupt);
    std::signal(SIGTERM, on_interrupt);

    try {
        sim::RuntimeConfig cfg = sim::load_config(argc, argv);

        // The links, attached the way their names say.
        sim::IpcSocket to_clients_gatewaytakersecond("engine -> gatewaytakersecond", sim::IpcSocket::End::Reads, cfg.to_clients_gatewaytakersecond_link.host, cfg.to_clients_gatewaytakersecond_link.port);
        sim::MessageSocket from_takersecond("takersecond -> gatewaytakersecond", sim::MessageSocket::End::Reads, cfg.from_takersecond_link.host, cfg.from_takersecond_link.port);
        sim::IpcSocket to_sequencer_gatewaytakersecond("gatewaytakersecond -> sequencer", sim::IpcSocket::End::Writes, cfg.to_sequencer_gatewaytakersecond_link.host, cfg.to_sequencer_gatewaytakersecond_link.port);
        sim::MessageSocket to_takersecond("gatewaytakersecond -> takersecond", sim::MessageSocket::End::Writes, cfg.to_takersecond_link.host, cfg.to_takersecond_link.port);

        std::atomic<std::uint64_t> ipc_msgs{0};

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

        // A session per trader process, against the link that carries it. To the gateway a
        // trader on a link is what a client on a socket is: somewhere to write bytes.
        auto takersecond_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_takersecond, b, n); };
        std::size_t takersecond_client = gatewaytakersecond.connect(takersecond_reply);

        // Order entry over Tcp: a session per socket, opened as each client arrives.
        net::TcpServer order_entry(cfg.order_entry.host, cfg.gatewaytakersecond_port);
        std::map<int, std::size_t> sessions;

        std::printf("nasdaq-gatewaytakersecond: order entry on %s:%u, 1 trader(s) on sockets\n",
                    cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaytakersecond_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_clients_gatewaytakersecond.read([&](std::string_view message) { gatewaytakersecond_events.on_message(message); }) || worked;
            worked = from_takersecond.read([&](std::string_view bytes) { gatewaytakersecond.on_bytes(takersecond_client, bytes.data(), bytes.size()); }) || worked;
            worked = order_entry.poll([&](int fd, std::string_view bytes) {
                gatewaytakersecond.on_bytes(sessions.at(fd), bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = sessions.find(fd); gone != sessions.end()) {
                    gatewaytakersecond.disconnect(gone->second);
                    sessions.erase(gone);
                }
            }, [&](int fd) { sessions.emplace(fd, gatewaytakersecond.connect([&order_entry, fd](const char* b, std::size_t n) { order_entry.send(fd, b, n); })); }) || worked;
            gatewaytakersecond.tick();
            if (gatewaytakersecond.ended(takersecond_client)) {
                gatewaytakersecond.disconnect(takersecond_client);
                takersecond_client = gatewaytakersecond.connect(takersecond_reply);
                worked = true;
            }
            for (auto session = sessions.begin(); session != sessions.end(); ) {
                if (!gatewaytakersecond.ended(session->second)) { ++session; continue; }
                order_entry.close(session->first);
                gatewaytakersecond.disconnect(session->second);
                session = sessions.erase(session);
                worked = true;
            }
            to_sequencer_gatewaytakersecond.deliver();
            to_takersecond.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_clients_gatewaytakersecond.gaps() > 0) std::printf("nasdaq-gatewaytakersecond: %llu gap(s) on engine -> gatewaytakersecond — messages were lost\n", (unsigned long long)to_clients_gatewaytakersecond.gaps());
        if (from_takersecond.gaps() > 0) std::printf("nasdaq-gatewaytakersecond: %llu gap(s) on takersecond -> gatewaytakersecond — messages were lost\n", (unsigned long long)from_takersecond.gaps());
        std::printf("nasdaq-gatewaytakersecond: stopped — %llu to the engine, %zu open order(s)\n",
                    (unsigned long long)ipc_msgs.load(), gatewaytakersecond.open_orders());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-gatewaytakersecond: %s\n", problem.what());
        return 1;
    }

    return 0;
}
