// Generated Odx Security Token Session order-entry gateway — one component, one process.
// 
// The sessions it serves and the orders it admits are the same ones the single binary
// serves; only its links differ, and those are rings in shared memory that the venue's launcher created.

#include "protocols/orderentry/Gateway.hpp"
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
        sim::IpcRing to_clients_gatewaymakerbasic("engine -> gatewaymakerbasic", sim::IpcRing::End::Reads);
        sim::MessageRing from_makerbasic("makerbasic -> gatewaymakerbasic", sim::MessageRing::End::Reads);
        sim::IpcRing to_sequencer_gatewaymakerbasic("gatewaymakerbasic -> sequencer", sim::IpcRing::End::Writes);
        sim::MessageRing to_makerbasic("gatewaymakerbasic -> makerbasic", sim::MessageRing::End::Writes);

        std::atomic<std::uint64_t> ipc_msgs{0};

        // MakerBasic's gateway: each client's session reaches the engine as framed Ipc messages.
        auto gatewaymakerbasic_emit = [&](const char* b, std::size_t n) {
            ipc_msgs.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_sequencer_gatewaymakerbasic, b, n);
        };
        sim::orderentry::Gateway<decltype(gatewaymakerbasic_emit)> gatewaymakerbasic(gatewaymakerbasic_emit, 1, 2);
        gatewaymakerbasic.allow("ALICE1", "alice00001");
        gatewaymakerbasic.allow("BOB001", "bob0000001");
        gatewaymakerbasic.allow("GUEST1", "guest00001");

        // The engine events the gateway answers, on the gateway's own thread.
        sim::IpcEventServer gatewaymakerbasic_events(
            [&](const sim::ipc_ns::trade_message& t) { gatewaymakerbasic.on_event(t); },
            [](const sim::ipc_ns::order_rested_message&) {},
            [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakerbasic.on_event(c); },
            [&](const sim::ipc_ns::order_accepted_message& a) { gatewaymakerbasic.on_event(a); },
            [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaymakerbasic.on_event(e); },
            [&](const sim::ipc_ns::order_refused_message& j) { gatewaymakerbasic.on_event(j); },
            [](const sim::ipc_ns::phase_announced_message&) {},
            [](const sim::ipc_ns::cross_completed_message&) {},
            [](const sim::ipc_ns::imbalance_indicated_message&) {});

        // A session per trader process, against the link that carries it. To the gateway a
        // trader on a link is what a client on a socket is: somewhere to write bytes.
        auto makerbasic_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_makerbasic, b, n); };
        std::size_t makerbasic_client = gatewaymakerbasic.connect(makerbasic_reply);

        // Order entry over Tcp: a session per socket, opened as each client arrives.
        net::TcpServer order_entry(cfg.order_entry.host, cfg.gatewaymakerbasic_port);
        std::map<int, std::size_t> sessions;

        std::printf("odx-gatewaymakerbasic: order entry on %s:%u, 1 trader(s) on rings\n",
                    cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaymakerbasic_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_clients_gatewaymakerbasic.read([&](std::string_view message) { gatewaymakerbasic_events.on_message(message); }) || worked;
            worked = from_makerbasic.read([&](std::string_view bytes) { gatewaymakerbasic.on_bytes(makerbasic_client, bytes.data(), bytes.size()); }) || worked;
            worked = order_entry.poll([&](int fd, std::string_view bytes) {
                auto session = sessions.find(fd);
                if (session == sessions.end())
                    session = sessions.emplace(fd, gatewaymakerbasic.connect([&order_entry, fd](const char* b, std::size_t n) { order_entry.send(fd, b, n); })).first;
                gatewaymakerbasic.on_bytes(session->second, bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = sessions.find(fd); gone != sessions.end()) {
                    gatewaymakerbasic.disconnect(gone->second);
                    sessions.erase(gone);
                }
            }) || worked;
            gatewaymakerbasic.tick();
            if (gatewaymakerbasic.ended(makerbasic_client)) {
                gatewaymakerbasic.disconnect(makerbasic_client);
                makerbasic_client = gatewaymakerbasic.connect(makerbasic_reply);
                worked = true;
            }
            for (auto session = sessions.begin(); session != sessions.end(); ) {
                if (!gatewaymakerbasic.ended(session->second)) { ++session; continue; }
                order_entry.close(session->first);
                gatewaymakerbasic.disconnect(session->second);
                session = sessions.erase(session);
                worked = true;
            }
            to_sequencer_gatewaymakerbasic.deliver();
            to_makerbasic.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("odx-gatewaymakerbasic: stopped — %llu to the engine, %zu open order(s)\n",
                    (unsigned long long)ipc_msgs.load(), gatewaymakerbasic.open_orders());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-gatewaymakerbasic: %s\n", problem.what());
        return 1;
    }

    return 0;
}
