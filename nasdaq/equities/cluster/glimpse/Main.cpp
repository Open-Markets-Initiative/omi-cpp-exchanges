// Generated Nasdaq Stock Market Glimpse — one component, one process.
// 
// A snapshot service over TotalView: it holds what the feed has published and
// serves a client from where that client asks. The same service the single binary runs as a
// thread; only its link differs, and that is one of sockets.

#include "protocols/replay/glimpse/ReplayServer.hpp"
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
        sim::MessageSocket to_glimpse("totalview -> glimpse", sim::MessageSocket::End::Reads, cfg.to_glimpse_link.host, cfg.to_glimpse_link.port);

        sim::replay::glimpse::Server<std::function<void(const char*, std::size_t)>> glimpse("NSMGL00001");
        glimpse.allow("ALICE1", "alice00001");
        glimpse.allow("BOB001", "bob0000001");
        glimpse.allow("CAROL1", "carol00001");
        glimpse.allow("DAVE01", "dave000001");

        // Its own socket: the clients that ask it for a replay dial this, not the gateway.
        net::TcpServer glimpse_server(cfg.glimpse_replay_port == 0 ? "0.0.0.0" : "127.0.0.1", cfg.glimpse_replay_port);
        std::map<int, std::size_t> glimpse_sessions;

        std::printf("nasdaq-glimpse: serving snapshot of TotalView on port %u\n", (unsigned)cfg.glimpse_replay_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_glimpse.read([&](std::string_view message) { glimpse.on_message(message.data(), message.size()); }) || worked;
            worked = glimpse_server.poll([&](int fd, std::string_view bytes) {
                auto session = glimpse_sessions.find(fd);
                if (session == glimpse_sessions.end())
                    session = glimpse_sessions.emplace(fd, glimpse.connect([fd](const char* b, std::size_t n) { net::TcpServer::send(fd, b, n); })).first;
                glimpse.on_bytes(session->second, bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = glimpse_sessions.find(fd); gone != glimpse_sessions.end()) {
                    glimpse.disconnect(gone->second);
                    glimpse_sessions.erase(gone);
                }
            }) || worked;
            // What it has to serve a waiting client, a step at a time.
            worked = glimpse.pump() || worked;
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        if (to_glimpse.gaps() > 0) std::printf("nasdaq-glimpse: %llu gap(s) on totalview -> glimpse — messages were lost\n", (unsigned long long)to_glimpse.gaps());
        std::printf("nasdaq-glimpse: stopped — %llu messages held\n", (unsigned long long)glimpse.seen());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-glimpse: %s\n", problem.what());
        return 1;
    }

    return 0;
}
