// Generated Nasdaq Stock Market Session Glimpse — one component, one process.
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
        glimpse.allow("MAKER1", "maker10000");
        glimpse.allow("TAKER1", "taker10000");
        glimpse.allow("REPLAY", "replay0000");
        glimpse.allow("ASKER1", "asker10000");
        glimpse.allow("MAKER2", "maker20000");
        glimpse.allow("TAKER2", "taker20000");
        glimpse.allow("FIXMKR", "fixmkr0000");
        glimpse.allow("FIXTKR", "fixtkr0000");
        glimpse.allow("RISKST", "riskst0000");
        glimpse.allow("RSTRCT", "rstrct0000");
        glimpse.allow("RATEDP", "ratedp0000");
        glimpse.allow("KILLSW", "killsw0000");
        glimpse.allow("GROSSX", "grossx0000");
        glimpse.allow("FATFNG", "fatfng0000");
        glimpse.allow("OUCETC", "oucetc0000");
        glimpse.allow("HALTCX", "haltcx0000");
        glimpse.allow("MELOBY", "meloby0000");
        glimpse.allow("MELOSL", "melosl0000");
        glimpse.allow("MELOPG", "melopg0000");
        glimpse.allow("FIXETC", "fixetc0000");
        glimpse.allow("FIXOPT", "fixopt0000");
        glimpse.allow("FIXRSK", "fixrsk0000");

        // Its own socket: the clients that ask it for a replay dial this, not the gateway.
        net::TcpServer glimpse_server(cfg.glimpse_replay_host, cfg.glimpse_replay_port);   // where its settings say: every interface in a container
        std::map<int, std::size_t> glimpse_sessions;

        std::printf("nasdaq-glimpse: serving snapshot of TotalView on port %u\n", (unsigned)cfg.glimpse_replay_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_glimpse.read([&](std::string_view message) { glimpse.on_message(message.data(), message.size()); }) || worked;
            worked = glimpse_server.poll([&](int fd, std::string_view bytes) {
                glimpse.on_bytes(glimpse_sessions.at(fd), bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = glimpse_sessions.find(fd); gone != glimpse_sessions.end()) {
                    glimpse.disconnect(gone->second);
                    glimpse_sessions.erase(gone);
                }
            }, [&](int fd) { glimpse_sessions.emplace(fd, glimpse.connect([&server = glimpse_server, fd](const char* b, std::size_t n) { server.send(fd, b, n); })); }) || worked;
            glimpse.tick();
            for (auto session = glimpse_sessions.begin(); session != glimpse_sessions.end(); ) {
                if (!glimpse.ended(session->second)) { ++session; continue; }
                glimpse_server.close(session->first);
                glimpse.disconnect(session->second);
                session = glimpse_sessions.erase(session);
                worked = true;
            }
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
