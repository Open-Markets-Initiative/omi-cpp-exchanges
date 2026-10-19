// Generated Nasdaq Stock Market TotalViewTcp — one component, one process.
// 
// A stream service over TotalView: it holds what the feed has published and
// serves a client from where that client asks. The same service the single binary runs as a
// thread; only its link differs, and that is one of sockets.

#include "protocols/replay/totalviewtcp/ReplayServer.hpp"
#include "run/SocketLink.hpp"
#include "net/Network.hpp"
#include "SimulatorConfiguration.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <functional>
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
    // A client hanging up must not take the service with it.
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, on_interrupt);
    std::signal(SIGTERM, on_interrupt);

    try {
        sim::RuntimeConfig cfg = sim::load_config(argc, argv);

        // The link, attached the way its name says: what the feed publishes arrives here.
        sim::MessageSocket to_totalviewtcp("totalview -> totalviewtcp", sim::MessageSocket::End::Reads, cfg.to_totalviewtcp_link.host, cfg.to_totalviewtcp_link.port);

        sim::replay::totalviewtcp::Server<std::function<void(const char*, std::size_t)>> totalviewtcp("NSMTV00001");
        totalviewtcp.allow("ALICE1", "alice00001");
        totalviewtcp.allow("BOB001", "bob0000001");
        totalviewtcp.allow("CAROL1", "carol00001");
        totalviewtcp.allow("DAVE01", "dave000001");

        // Its own socket: the clients that ask it for a replay dial this, not the gateway.
        net::TcpServer totalviewtcp_server(cfg.totalviewtcp_replay_port == 0 ? "0.0.0.0" : "127.0.0.1", cfg.totalviewtcp_replay_port);
        std::map<int, std::size_t> totalviewtcp_sessions;

        std::printf("nasdaq-totalviewtcp: serving stream of TotalView on port %u\n", (unsigned)cfg.totalviewtcp_replay_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_totalviewtcp.read([&](std::string_view message) { totalviewtcp.on_message(message.data(), message.size()); }) || worked;
            worked = totalviewtcp_server.poll([&](int fd, std::string_view bytes) {
                auto session = totalviewtcp_sessions.find(fd);
                if (session == totalviewtcp_sessions.end())
                    session = totalviewtcp_sessions.emplace(fd, totalviewtcp.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
                totalviewtcp.on_bytes(session->second, bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = totalviewtcp_sessions.find(fd); gone != totalviewtcp_sessions.end()) {
                    totalviewtcp.disconnect(gone->second);
                    totalviewtcp_sessions.erase(gone);
                }
            }) || worked;
            // What it has to serve a waiting client, a step at a time.
            worked = totalviewtcp.pump() || worked;
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_totalviewtcp.gaps() > 0) std::printf("nasdaq-totalviewtcp: %llu gap(s) on totalview -> totalviewtcp — messages were lost\n", (unsigned long long)to_totalviewtcp.gaps());
        std::printf("nasdaq-totalviewtcp: stopped — %llu messages held\n", (unsigned long long)totalviewtcp.seen());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-totalviewtcp: %s\n", problem.what());
        return 1;
    }

    return 0;
}
