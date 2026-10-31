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
#include "../../marketdata/totalview/messages/AddOrderNoMpidAttributionMessage.hpp"
#include "../../marketdata/totalview/messages/OrderDeleteMessage.hpp"
#include "../../marketdata/totalview/messages/OrderExecutedMessage.hpp"

namespace sim::replay::glimpse {

namespace feed_ns = ::nasdaq::nsmequities::totalview::itch::v5_0_2026;

// The Glimpse service: the book as it stands, served to each client that logs in,
// framed as Nasdaq.NsmEquities.TotalView.Glimpse.v5.0 sequenced data. Messages arrive from the TotalView feed in publish order.
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
        if (length <= 2) return;
        const auto* bytes = reinterpret_cast<const std::byte*>(message);
        switch (message[2]) {
            case 'A': {
                const auto& m = *feed_ns::add_order_no_mpid_attribution_message::parse(const_cast<std::byte*>(bytes));
                const std::uint64_t reference = static_cast<std::uint64_t>(m.fields.order_reference_number.get().value());
                // A resting order joins the book.
                book_[reference].assign(message, length);
                break;
            }
            case 'D': {
                const auto& m = *feed_ns::order_delete_message::parse(const_cast<std::byte*>(bytes));
                const std::uint64_t reference = static_cast<std::uint64_t>(m.fields.order_reference_number.get().value());
                // A canceled order leaves it.
                book_.erase(reference);
                break;
            }
            case 'E': {
                const auto& m = *feed_ns::order_executed_message::parse(const_cast<std::byte*>(bytes));
                const std::uint64_t reference = static_cast<std::uint64_t>(m.fields.order_reference_number.get().value());
                // An execution takes its shares off the order that was resting.
                auto held = book_.find(reference);
                if (held == book_.end()) break;
                auto& resting = *feed_ns::add_order_no_mpid_attribution_message::parse(reinterpret_cast<std::byte*>(held->second.data()));
                const auto executed = static_cast<std::uint64_t>(m.fields.executed_shares.get().value());
                const auto open = static_cast<std::uint64_t>(resting.fields.shares.get().value());
                if (executed >= open) { book_.erase(held); break; }
                resting.fields.shares.set(static_cast<std::remove_cvref_t<decltype(resting.fields.shares.get().value())>>(open - executed));
                break;
            }
            default: break;   // a message the book is not kept from
        }
    }

    // What each client is still owed, sent from the service's own thread.
    bool pump() {
        bool worked = false;
        for (auto& [number, client] : clients_) {
            if (!client->serving) continue;
            // The book as it stands, then the session ends: the client resumes on the live feed.
            for (const auto& resting : book_) client->session.sequenced(resting.second.data(), resting.second.size());
            client->session.end_of_session();
            client->serving = false;
            worked = true;
        }
        return worked;
    }

    std::uint64_t seen() const { return seen_; }
    std::size_t held() const { return book_.size(); }
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
            // A client that asks for nothing in particular is told where the live feed resumes.
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
    std::map<std::uint64_t, std::string> book_;   // the reference each resting order is known by
};

}
