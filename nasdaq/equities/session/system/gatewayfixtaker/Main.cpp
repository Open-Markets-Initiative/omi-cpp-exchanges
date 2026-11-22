// Generated Nasdaq Stock Market Session order-entry gateway — one component, one process.
// 
// The sessions it serves and the orders it admits are the same ones the single binary
// serves; only its links differ, and those are rings in shared memory that the venue's launcher created.

#include "protocols/orderentry/fix/Gateway.hpp"
#include "protocols/ipc/IpcBus.hpp"
#include "protocols/ipc/IpcTransforms.hpp"
#include "run/SharedRing.hpp"
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
        sim::IpcRing to_clients_gatewayfixtaker("engine -> gatewayfixtaker", sim::IpcRing::End::Reads);
        sim::MessageRing from_fixtaker("fixtaker -> gatewayfixtaker", sim::MessageRing::End::Reads);
        sim::IpcRing to_sequencer_gatewayfixtaker("gatewayfixtaker -> sequencer", sim::IpcRing::End::Writes);
        sim::MessageRing to_fixtaker("gatewayfixtaker -> fixtaker", sim::MessageRing::End::Writes);

        std::atomic<std::uint64_t> ipc_msgs{0};

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

        // A session per trader process, against the link that carries it. To the gateway a
        // trader on a link is what a client on a socket is: somewhere to write bytes.
        auto fixtaker_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_fixtaker, b, n); };
        std::size_t fixtaker_client = gatewayfixtaker.connect(fixtaker_reply);

        // Order entry over Tcp: a session per socket, opened as each client arrives.
        net::TcpServer order_entry(cfg.order_entry.host, cfg.gatewayfixtaker_port);
        std::map<int, std::size_t> sessions;

        std::printf("nasdaq-gatewayfixtaker: order entry on %s:%u, 1 trader(s) on rings\n",
                    cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayfixtaker_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        sim::reading();   // launch.sh starts the next component once this one reads its links
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_clients_gatewayfixtaker.read([&](std::string_view message) { gatewayfixtaker_events.on_message(message); }) || worked;
            worked = from_fixtaker.read([&](std::string_view bytes) { gatewayfixtaker.on_bytes(fixtaker_client, bytes.data(), bytes.size()); }) || worked;
            worked = order_entry.poll([&](int fd, std::string_view bytes) {
                gatewayfixtaker.on_bytes(sessions.at(fd), bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = sessions.find(fd); gone != sessions.end()) {
                    gatewayfixtaker.disconnect(gone->second);
                    sessions.erase(gone);
                }
            }, [&](int fd) { sessions.emplace(fd, gatewayfixtaker.connect([&order_entry, fd](const char* b, std::size_t n) { order_entry.send(fd, b, n); })); }) || worked;
            gatewayfixtaker.tick();
            if (gatewayfixtaker.ended(fixtaker_client)) {
                gatewayfixtaker.disconnect(fixtaker_client);
                fixtaker_client = gatewayfixtaker.connect(fixtaker_reply);
                worked = true;
            }
            for (auto session = sessions.begin(); session != sessions.end(); ) {
                if (!gatewayfixtaker.ended(session->second)) { ++session; continue; }
                order_entry.close(session->first);
                gatewayfixtaker.disconnect(session->second);
                session = sessions.erase(session);
                worked = true;
            }
            to_sequencer_gatewayfixtaker.deliver();
            to_fixtaker.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("nasdaq-gatewayfixtaker: stopped — %llu to the engine, %zu open order(s)\n",
                    (unsigned long long)ipc_msgs.load(), gatewayfixtaker.open_orders());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-gatewayfixtaker: %s\n", problem.what());
        return 1;
    }

    return 0;
}
