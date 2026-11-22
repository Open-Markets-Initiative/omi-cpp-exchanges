// Generated Nasdaq Stock Market Session order-entry gateway — one component, one process.
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
        sim::IpcRing to_clients_gatewaymakersecond("engine -> gatewaymakersecond", sim::IpcRing::End::Reads);
        sim::MessageRing from_makersecond("makersecond -> gatewaymakersecond", sim::MessageRing::End::Reads);
        sim::IpcRing to_sequencer_gatewaymakersecond("gatewaymakersecond -> sequencer", sim::IpcRing::End::Writes);
        sim::MessageRing to_makersecond("gatewaymakersecond -> makersecond", sim::MessageRing::End::Writes);

        std::atomic<std::uint64_t> ipc_msgs{0};

        // MakerSecond's gateway: each client's session reaches the engine as framed Ipc messages.
        auto gatewaymakersecond_emit = [&](const char* b, std::size_t n) {
            ipc_msgs.fetch_add(1, std::memory_order_relaxed);
            sim::write_blocking(to_sequencer_gatewaymakersecond, b, n);
        };
        sim::orderentry::Gateway<decltype(gatewaymakersecond_emit)> gatewaymakersecond(gatewaymakersecond_emit, 5, 8);
        gatewaymakersecond.allow("MAKER1", "maker10000");
        gatewaymakersecond.allow("TAKER1", "taker10000");
        gatewaymakersecond.allow("REPLAY", "replay0000");
        gatewaymakersecond.allow("ASKER1", "asker10000");
        gatewaymakersecond.allow("MAKER2", "maker20000");
        gatewaymakersecond.allow("TAKER2", "taker20000");
        gatewaymakersecond.allow("FIXMKR", "fixmkr0000");
        gatewaymakersecond.allow("FIXTKR", "fixtkr0000");
        gatewaymakersecond.allow("RISKST", "riskst0000");
        gatewaymakersecond.allow("RSTRCT", "rstrct0000");
        gatewaymakersecond.allow("RATEDP", "ratedp0000");
        gatewaymakersecond.allow("KILLSW", "killsw0000");
        gatewaymakersecond.allow("GROSSX", "grossx0000");
        gatewaymakersecond.allow("FATFNG", "fatfng0000");
        gatewaymakersecond.allow("OUCETC", "oucetc0000");
        gatewaymakersecond.allow("HALTCX", "haltcx0000");
        gatewaymakersecond.allow("MELOBY", "meloby0000");
        gatewaymakersecond.allow("MELOSL", "melosl0000");
        gatewaymakersecond.allow("MELOPG", "melopg0000");
        gatewaymakersecond.allow("FIXETC", "fixetc0000");
        gatewaymakersecond.allow("FIXOPT", "fixopt0000");
        gatewaymakersecond.allow("FIXRSK", "fixrsk0000");

        // The engine events the gateway answers, on the gateway's own thread.
        sim::IpcEventServer gatewaymakersecond_events(
            [&](const sim::ipc_ns::trade_message& t) { gatewaymakersecond.on_event(t); },
            [&](const sim::ipc_ns::order_rested_message& o) { gatewaymakersecond.on_event(o); },
            [&](const sim::ipc_ns::order_canceled_message& c) { gatewaymakersecond.on_event(c); },
            [&](const sim::ipc_ns::order_accepted_message& a) { gatewaymakersecond.on_event(a); },
            [&](const sim::ipc_ns::order_eliminated_message& e) { gatewaymakersecond.on_event(e); },
            [&](const sim::ipc_ns::order_refused_message& j) { gatewaymakersecond.on_event(j); },
            [&](const sim::ipc_ns::phase_announced_message& s) { gatewaymakersecond.on_event(s); },
            [](const sim::ipc_ns::cross_completed_message&) {},
            [](const sim::ipc_ns::imbalance_indicated_message&) {},
            [&](const sim::ipc_ns::order_replaced_message& w) { gatewaymakersecond.on_event(w); },
            [&](const sim::ipc_ns::order_reduced_message& y) { gatewaymakersecond.on_event(y); },
            [&](const sim::ipc_ns::cancel_pending_message& g) { gatewaymakersecond.on_event(g); },
            [&](const sim::ipc_ns::reduce_refused_message& z) { gatewaymakersecond.on_event(z); },
            [&](const sim::ipc_ns::trade_broken_message& x) { gatewaymakersecond.on_event(x); });

        // A session per trader process, against the link that carries it. To the gateway a
        // trader on a link is what a client on a socket is: somewhere to write bytes.
        auto makersecond_reply = [&](const char* b, std::size_t n) { sim::write_blocking(to_makersecond, b, n); };
        std::size_t makersecond_client = gatewaymakersecond.connect(makersecond_reply);

        // Order entry over Tcp: a session per socket, opened as each client arrives.
        net::TcpServer order_entry(cfg.order_entry.host, cfg.gatewaymakersecond_port);
        std::map<int, std::size_t> sessions;

        std::printf("nasdaq-gatewaymakersecond: order entry on %s:%u, 1 trader(s) on rings\n",
                    cfg.order_entry.host.c_str(), (unsigned)cfg.gatewaymakersecond_port);

        // Nothing to do means nothing to do: back off rather than spin a core.
        sim::reading();   // launch.sh starts the next component once this one reads its links
        while (running.load(std::memory_order_acquire)) {
            bool worked = false;
            worked = to_clients_gatewaymakersecond.read([&](std::string_view message) { gatewaymakersecond_events.on_message(message); }) || worked;
            worked = from_makersecond.read([&](std::string_view bytes) { gatewaymakersecond.on_bytes(makersecond_client, bytes.data(), bytes.size()); }) || worked;
            worked = order_entry.poll([&](int fd, std::string_view bytes) {
                gatewaymakersecond.on_bytes(sessions.at(fd), bytes.data(), bytes.size());
            }, [&](int fd) {
                if (const auto gone = sessions.find(fd); gone != sessions.end()) {
                    gatewaymakersecond.disconnect(gone->second);
                    sessions.erase(gone);
                }
            }, [&](int fd) { sessions.emplace(fd, gatewaymakersecond.connect([&order_entry, fd](const char* b, std::size_t n) { order_entry.send(fd, b, n); })); }) || worked;
            gatewaymakersecond.tick();
            if (gatewaymakersecond.ended(makersecond_client)) {
                gatewaymakersecond.disconnect(makersecond_client);
                makersecond_client = gatewaymakersecond.connect(makersecond_reply);
                worked = true;
            }
            for (auto session = sessions.begin(); session != sessions.end(); ) {
                if (!gatewaymakersecond.ended(session->second)) { ++session; continue; }
                order_entry.close(session->first);
                gatewaymakersecond.disconnect(session->second);
                session = sessions.erase(session);
                worked = true;
            }
            to_sequencer_gatewaymakersecond.deliver();
            to_makersecond.deliver();
            if (!worked) std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        std::printf("nasdaq-gatewaymakersecond: stopped — %llu to the engine, %zu open order(s)\n",
                    (unsigned long long)ipc_msgs.load(), gatewaymakersecond.open_orders());
    } catch (const std::exception& problem) {
        std::fprintf(stderr, "nasdaq-gatewaymakersecond: %s\n", problem.what());
        return 1;
    }

    return 0;
}
