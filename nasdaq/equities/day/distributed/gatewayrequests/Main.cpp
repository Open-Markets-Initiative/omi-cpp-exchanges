// Generated Nasdaq Stock Market Day order-entry gateway — one component, one process.
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
        sim::IpcSocket to_clients_gatewayrequests("engine -> gatewayrequests", sim::IpcSocket::End::Reads, cfg.to_clients_gatewayrequests_link.host, cfg.to_clients_gatewayrequests_link.port);
        sim::MessageSocket from_requests("requests -> gatewayrequests", sim::MessageSocket::End::Reads, cfg.from_requests_link.host, cfg.from_requests_link.port);
        sim::IpcSocket to_sequencer_gatewayrequests("gatewayrequests -> sequencer", sim::IpcSocket::End::Writes, cfg.to_sequencer_gatewayrequests_link.host, cfg.to_sequencer_gatewayrequests_link.port);
        sim::MessageSocket to_requests("gatewayrequests -> requests", sim::MessageSocket::End::Writes, cfg.to_requests_link.host, cfg.to_requests_link.port);

        std::atomic<std::uint64_t> ipc_msgs{0};

        // Requests's gateway: each client's session reaches the engine as framed Ipc messages.
        auto gatewayrequests_emit = [&](const char* b, std::size_t n) {
            ipc_msgs.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_sequencer_gatewayrequests, b, n);
        };
        sim::orderentry::Gateway<decltype(gatewayrequests_emit)> gatewayrequests(gatewayrequests_emit, 4, 4);
        gatewayrequests.allow("ALICE1", "alice00001");
        gatewayrequests.allow("BOB001", "bob0000001");
        gatewayrequests.allow("CAROL1", "carol00001");
        gatewayrequests.allow("DAVE01", "dave000001");

        // The engine events the gateway answers, on the gateway's own thread.
        sim::IpcEventServer gatewayrequests_events(
            [&](const sim::ipc_ns::trade_message& t) { gatewayrequests.on_event(t); },
            [](const sim::ipc_ns::order_rested_message&) {},
            [&](const sim::ipc_ns::order_canceled_message& c) { gatewayrequests.on_event(c); },
            [&](const sim::ipc_ns::order_accepted_message& a) { gatewayrequests.on_event(a); },
            [&](const sim::ipc_ns::order_eliminated_message& e) { gatewayrequests.on_event(e); },
            [&](const sim::ipc_ns::order_refused_message& j) { gatewayrequests.on_event(j); },
            [](const sim::ipc_ns::phase_announced_message&) {},
            [](const sim::ipc_ns::cross_completed_message&) {},
            [](const sim::ipc_ns::imbalance_indicated_message&) {});

        // A session per trader process, against the link that carries it. To the gateway a
        // trader on a link is what a client on a socket is: somewhere to write bytes.
        const std::size_t requests_client = gatewayrequests.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_requests, b, n); });

        // Order entry over Tcp: a session per socket, opened as each client arrives.
        net::TcpServer order_entry(cfg.order_entry.host, cfg.gatewayrequests_port);
        std::map<int, std::size_t> sessions;

        std::printf("nasdaq-gatewayrequests: order entry on %s:%u, 1 trader(s) on sockets\n",
                    cfg.order_entry.host.c_str(), (unsigned)cfg.gatewayrequests_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_clients_gatewayrequests.read([&](std::string_view message) { gatewayrequests_events.on_message(message); }) || worked;
            worked = from_requests.read([&](std::string_view bytes) { gatewayrequests.on_bytes(requests_client, bytes.data(), bytes.size()); }) || worked;
            worked = order_entry.poll([&](int fd, std::string_view bytes) {
                auto session = sessions.find(fd);
                if (session == sessions.end())
                    session = sessions.emplace(fd, gatewayrequests.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
                gatewayrequests.on_bytes(session->second, bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = sessions.find(fd); gone != sessions.end()) {
                    gatewayrequests.disconnect(gone->second);
                    sessions.erase(gone);
                }
            }) || worked;
            to_sequencer_gatewayrequests.deliver();
            to_requests.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_clients_gatewayrequests.gaps() > 0) std::printf("nasdaq-gatewayrequests: %llu gap(s) on engine -> gatewayrequests — messages were lost\n", (unsigned long long)to_clients_gatewayrequests.gaps());
        if (from_requests.gaps() > 0) std::printf("nasdaq-gatewayrequests: %llu gap(s) on requests -> gatewayrequests — messages were lost\n", (unsigned long long)from_requests.gaps());
        std::printf("nasdaq-gatewayrequests: stopped — %llu to the engine, %zu open order(s)\n",
                    (unsigned long long)ipc_msgs.load(), gatewayrequests.open_orders());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-gatewayrequests: %s\n", problem.what());
        return 1;
    }

    return 0;
}
