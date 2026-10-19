// Generated Nasdaq Stock Market order-entry gateway — one component, one process.
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
        sim::IpcRing to_clients("engine -> gateway", sim::IpcRing::End::Reads);
        sim::MessageRing from_maker("maker -> gateway", sim::MessageRing::End::Reads);
        sim::MessageRing from_taker("taker -> gateway", sim::MessageRing::End::Reads);
        sim::MessageRing from_replay("replay -> gateway", sim::MessageRing::End::Reads);
        sim::MessageRing from_requests("requests -> gateway", sim::MessageRing::End::Reads);
        sim::IpcRing to_engine("gateway -> engine", sim::IpcRing::End::Writes);
        sim::MessageRing to_maker("gateway -> maker", sim::MessageRing::End::Writes);
        sim::MessageRing to_taker("gateway -> taker", sim::MessageRing::End::Writes);
        sim::MessageRing to_replay("gateway -> replay", sim::MessageRing::End::Writes);
        sim::MessageRing to_requests("gateway -> requests", sim::MessageRing::End::Writes);

        std::atomic<std::uint64_t> ipc_msgs{0};

        // Order entry: each client's session reaches the engine as framed Ipc messages.
        auto gateway_emit = [&](const char* b, std::size_t n) {
            ipc_msgs.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_engine, b, n);
        };
        sim::orderentry::Gateway<decltype(gateway_emit)> gateway(gateway_emit);
        gateway.allow("ALICE1", "alice00001");
        gateway.allow("BOB001", "bob0000001");
        gateway.allow("CAROL1", "carol00001");
        gateway.allow("DAVE01", "dave000001");

        // The engine events the gateway answers, on the gateway's own thread.
        sim::IpcEventServer client_events(
            [&](const sim::ipc_ns::trade_message& t) { gateway.on_event(t); },
            [](const sim::ipc_ns::order_rested_message&) {},
            [&](const sim::ipc_ns::order_canceled_message& c) { gateway.on_event(c); });

        // A session per trader process, against the link that carries it. To the gateway a
        // trader on a link is what a client on a socket is: somewhere to write bytes.
        const std::size_t maker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_maker, b, n); });
        const std::size_t taker_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_taker, b, n); });
        const std::size_t replay_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_replay, b, n); });
        const std::size_t requests_client = gateway.connect([&](const char* b, std::size_t n) { sim::write_blocking(to_requests, b, n); });

        // Order entry over Tcp: a session per socket, opened as each client arrives.
        net::TcpServer order_entry(cfg.order_entry.host, cfg.order_entry.port);
        std::map<int, std::size_t> sessions;

        std::printf("nasdaq-gateway: order entry on %s:%u, 4 trader(s) on rings\n",
                    cfg.order_entry.host.c_str(), (unsigned)cfg.order_entry.port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_clients.read([&](std::string_view message) { client_events.on_message(message); }) || worked;
            worked = from_maker.read([&](std::string_view bytes) { gateway.on_bytes(maker_client, bytes.data(), bytes.size()); }) || worked;
            worked = from_taker.read([&](std::string_view bytes) { gateway.on_bytes(taker_client, bytes.data(), bytes.size()); }) || worked;
            worked = from_replay.read([&](std::string_view bytes) { gateway.on_bytes(replay_client, bytes.data(), bytes.size()); }) || worked;
            worked = from_requests.read([&](std::string_view bytes) { gateway.on_bytes(requests_client, bytes.data(), bytes.size()); }) || worked;
            worked = order_entry.poll([&](int fd, std::string_view bytes) {
                auto session = sessions.find(fd);
                if (session == sessions.end())
                    session = sessions.emplace(fd, gateway.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
                gateway.on_bytes(session->second, bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = sessions.find(fd); gone != sessions.end()) {
                    gateway.disconnect(gone->second);
                    sessions.erase(gone);
                }
            }) || worked;
            to_engine.deliver();
            to_maker.deliver();
            to_taker.deliver();
            to_replay.deliver();
            to_requests.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("nasdaq-gateway: stopped — %llu to the engine, %zu open order(s)\n",
                    (unsigned long long)ipc_msgs.load(), gateway.open_orders());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-gateway: %s\n", problem.what());
        return 1;
    }

    return 0;
}
