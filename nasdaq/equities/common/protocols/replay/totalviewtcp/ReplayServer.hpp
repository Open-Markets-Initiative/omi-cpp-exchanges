#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include "SoupBinSession.hpp"

namespace sim::replay::totalviewtcp {


// The TotalViewTcp service: the feed's messages, served to each client that logs in,
// framed as Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 sequenced data. Messages arrive from the TotalView feed in publish order.
template <typename Send>
class Server {
public:
    explicit Server(std::string_view session) : session_(session) {}

    // A login the service accepts; with none registered, any login is accepted.
    void allow(std::string username, std::string password) { credentials_[std::move(username)] = std::move(password); }

    // A new client connection, writing through `send`; returns the session's own number,
    // handed out once and never again.
    std::size_t connect(Send send) {
        const auto client = next_client_++;
        clients_.emplace(client, std::make_unique<Client>(*this, std::move(send)));
        return client;
    }

    // A client that has gone: what it was still owed, it is owed no longer.
    void disconnect(std::size_t client) { clients_.erase(client); }

    void on_bytes(std::size_t client, const char* data, std::size_t length) {
        if (const auto held = clients_.find(client); held != clients_.end()) {
            held->second->session.on_bytes(data, length);
        }
    }

    // One message the feed published, in publish order.
    void on_message(const char* message, std::size_t length) {
        ++seen_;
        stream_.emplace_back(message, length);
        // A client already served follows the feed from here.
        for (auto& [number, client] : clients_) if (client->live) client->session.sequenced(message, length);
    }

    // What each client is still owed, sent from the service's own thread.
    bool pump() {
        bool worked = false;
        for (auto& [number, client] : clients_) {
            if (!client->serving) continue;
            // The stream from where the client asked, then it follows the feed live.
            const std::size_t from = client->from == 0 ? stream_.size() : client->from - 1;
            for (std::size_t at = from; at < stream_.size(); ++at) client->session.sequenced(stream_[at].data(), stream_[at].size());
            client->serving = false;
            client->live = true;
            worked = true;
        }
        return worked;
    }

    std::uint64_t seen() const { return seen_; }
    std::size_t stored() const { return stream_.size(); }
    std::size_t clients() const { return clients_.size(); }

private:
    // One client connection: its session, and what it is still owed.
    struct Client {
        Server& server;
        Send send;
        soupbin::Session<Send, Client> session;
        bool serving = false;   // asked for what the service holds, not yet served
        bool live    = false;   // served, and now following the feed
        std::uint64_t from = 0;

        Client(Server& owner, Send writer) : server(owner), send(std::move(writer)), session(server.session_, send, *this) {}

        std::optional<soupbin::status> login(std::string_view username, std::string_view password, std::string_view, std::uint64_t requested) {
            const auto refused = server.login(username, password);
            if (refused) return refused;
            // A client that asks for nothing in particular is served from here on, numbered from here.
            if (requested == 0) session.resume_from(server.seen() + 1);
            from = requested;
            serving = true;
            return std::nullopt;
        }
        void message(std::string_view) {}   // a replay client sends nothing but its login
        void logout() { live = false; serving = false; }
    };

    // A login is refused with the reason the exchange states, or the first the model gives.
    std::optional<soupbin::status> login(std::string_view username, std::string_view password) const {
        if (credentials_.empty()) return std::nullopt;
        const auto found = credentials_.find(std::string(username));
        if (found != credentials_.end() && found->second == password) return std::nullopt;
        return static_cast<soupbin::status>('A');
    }

    std::string session_;
    std::map<std::size_t, std::unique_ptr<Client>> clients_;
    std::size_t next_client_ = 0;   // session numbers, never reused
    std::map<std::string, std::string, std::less<>> credentials_;
    std::uint64_t seen_ = 0;
    std::vector<std::string> stream_;
};

}
