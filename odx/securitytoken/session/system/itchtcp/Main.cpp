// Generated Odx Security Token Session ItchTcp — one component, one process.
// 
// A stream service over Itch: it holds what the feed has published and
// serves a client from where that client asks. The same service the single binary runs as a
// thread; only its link differs, and that is one of rings.

#include "protocols/replay/itchtcp/ReplayServer.hpp"
#include "run/SharedRing.hpp"
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
        sim::MessageRing to_itchtcp("itch -> itchtcp", sim::MessageRing::End::Reads);

        sim::replay::itchtcp::Server<std::function<void(const char*, std::size_t)>> itchtcp("ODXSTDAY01");
        itchtcp.allow("ALICE1", "alice00001");
        itchtcp.allow("BOB001", "bob0000001");
        itchtcp.allow("GUEST1", "guest00001");

        // Its own socket: the clients that ask it for a replay dial this, not the gateway.
        net::TcpServer itchtcp_server(cfg.itchtcp_replay_host, cfg.itchtcp_replay_port);   // where its settings say: every interface in a container
        std::map<int, std::size_t> itchtcp_sessions;

        std::printf("odx-itchtcp: serving stream of Itch on port %u\n", (unsigned)cfg.itchtcp_replay_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_itchtcp.read([&](std::string_view message) { itchtcp.on_message(message.data(), message.size()); }) || worked;
            worked = itchtcp_server.poll([&](int fd, std::string_view bytes) {
                auto session = itchtcp_sessions.find(fd);
                if (session == itchtcp_sessions.end())
                    session = itchtcp_sessions.emplace(fd, itchtcp.connect([&server = itchtcp_server, fd](const char* b, std::size_t n) { server.send(fd, b, n); })).first;
                itchtcp.on_bytes(session->second, bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = itchtcp_sessions.find(fd); gone != itchtcp_sessions.end()) {
                    itchtcp.disconnect(gone->second);
                    itchtcp_sessions.erase(gone);
                }
            }) || worked;
            itchtcp.tick();
            for (auto session = itchtcp_sessions.begin(); session != itchtcp_sessions.end(); ) {
                if (!itchtcp.ended(session->second)) { ++session; continue; }
                itchtcp_server.close(session->first);
                itchtcp.disconnect(session->second);
                session = itchtcp_sessions.erase(session);
                worked = true;
            }
            // What it has to serve a waiting client, a step at a time.
            worked = itchtcp.pump() || worked;
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("odx-itchtcp: stopped — %llu messages held\n", (unsigned long long)itchtcp.seen());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "odx-itchtcp: %s\n", problem.what());
        return 1;
    }

    return 0;
}
