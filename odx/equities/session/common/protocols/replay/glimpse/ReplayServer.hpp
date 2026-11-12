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
#include "../../marketdata/itch/messages/OrderAddedMessage.hpp"
#include "../../marketdata/itch/messages/OrderDeletedMessage.hpp"
#include "../../marketdata/itch/messages/OrderExecutedMessage.hpp"
#include "../../marketdata/itch/messages/OrderReplacedMessage.hpp"
#include "../../marketdata/itch/messages/SystemEventMessage.hpp"
#include "../../marketdata/itch/messages/PriceTickSizeMessage.hpp"
#include "../../marketdata/itch/messages/OrderbookDirectoryMessage.hpp"
#include "../../marketdata/itch/messages/TradingStateMessage.hpp"

namespace sim::replay::glimpse {

namespace feed_ns = ::odx::odxequities::pts::itch::v2_2;

// The Glimpse service: the book as it stands, served to each client that logs in,
// framed as Odx.OdxEquities.Pts.Glimpse.v2.0 sequenced data. Messages arrive from the Itch feed in publish order.
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
        // The second what follows is stamped under, which each message kept is served after.
        if (message[2] == 'T') { second_.assign(message, length); return; }
        // What the snapshot serves ahead of the book, kept as the feed published it.
        if (message[2] == 'S') {
            const auto& m = *feed_ns::system_event_message::parse(const_cast<std::byte*>(bytes));
            (void)m;
            if (static_cast<char>(m.fields.system_event.get().value()) == 'O') kept_0_.push_back(Kept{ second_, std::string(message, length) });
        }
        if (message[2] == 'S') {
            const auto& m = *feed_ns::system_event_message::parse(const_cast<std::byte*>(bytes));
            (void)m;
            if (static_cast<char>(m.fields.system_event.get().value()) == 'C') kept_1_.push_back(Kept{ second_, std::string(message, length) });
        }
        if (message[2] == 'L') {
            const auto& m = *feed_ns::price_tick_size_message::parse(const_cast<std::byte*>(bytes));
            (void)m;
            kept_2_.push_back(Kept{ second_, std::string(message, length) });
        }
        if (message[2] == 'R') {
            const auto& m = *feed_ns::orderbook_directory_message::parse(const_cast<std::byte*>(bytes));
            (void)m;
            kept_3_.push_back(Kept{ second_, std::string(message, length) });
        }
        if (message[2] == 'H') {
            const auto& m = *feed_ns::trading_state_message::parse(const_cast<std::byte*>(bytes));
            (void)m;
            kept_4_[std::string(reinterpret_cast<const char*>(&m.fields.orderbook_id), sizeof m.fields.orderbook_id)] = Kept{ second_, std::string(message, length) };
        }
        switch (message[2]) {
            case 'A': {
                const auto& m = *feed_ns::order_added_message::parse(const_cast<std::byte*>(bytes));
                const std::uint64_t reference = static_cast<std::uint64_t>(m.fields.order_number.get().value());
                // A resting order joins the book.
                book_[reference].assign(message, length);
                stamped_[reference] = second_;
                break;
            }
            case 'D': {
                const auto& m = *feed_ns::order_deleted_message::parse(const_cast<std::byte*>(bytes));
                const std::uint64_t reference = static_cast<std::uint64_t>(m.fields.order_number.get().value());
                // A canceled order leaves it.
                book_.erase(reference);
                stamped_.erase(reference);
                break;
            }
            case 'E': {
                const auto& m = *feed_ns::order_executed_message::parse(const_cast<std::byte*>(bytes));
                const std::uint64_t reference = static_cast<std::uint64_t>(m.fields.order_number.get().value());
                // An execution takes its shares off the order that was resting.
                auto held = book_.find(reference);
                if (held == book_.end()) break;
                auto& resting = *feed_ns::order_added_message::parse(reinterpret_cast<std::byte*>(held->second.data()));
                const auto executed = static_cast<std::uint64_t>(m.fields.executed_quantity.get().value());
                const auto open = static_cast<std::uint64_t>(resting.fields.quantity.get().value());
                if (executed >= open) { book_.erase(held); stamped_.erase(reference); break; }
                resting.fields.quantity.set(static_cast<std::remove_cvref_t<decltype(resting.fields.quantity.get().value())>>(open - executed));
                break;
            }
            case 'U': {
                const auto& m = *feed_ns::order_replaced_message::parse(const_cast<std::byte*>(bytes));
                const std::uint64_t reference = static_cast<std::uint64_t>(m.fields.original_order_number.get().value());
                // A replaced order leaves the book, and its replacement rests in its stead: the order as it was
                // added, under the new reference, quantity and price.
                auto held = book_.find(reference);
                if (held == book_.end()) break;
                std::string replaced = std::move(held->second);
                book_.erase(held);
                auto stamp = std::move(stamped_[reference]);
                stamped_.erase(reference);
                stamped_[static_cast<std::uint64_t>(m.fields.new_order_number.get().value())] = std::move(stamp);
                auto& rests = *feed_ns::order_added_message::parse(reinterpret_cast<std::byte*>(replaced.data()));
                rests.fields.order_number.set(static_cast<std::remove_cvref_t<decltype(rests.fields.order_number.get().value())>>(m.fields.new_order_number.get().value()));
                rests.fields.quantity.set(static_cast<std::remove_cvref_t<decltype(rests.fields.quantity.get().value())>>(m.fields.quantity.get().value()));
                rests.fields.price.set(static_cast<std::remove_cvref_t<decltype(rests.fields.price.get().value())>>(m.fields.price.get().value()));
                book_[static_cast<std::uint64_t>(m.fields.new_order_number.get().value())] = std::move(replaced);
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
            // What the snapshot lists, in its order, each after the second it was stamped under; then the session ends.
            std::string served;   // the Seconds message last served
            auto serve = [&](const std::string& second, const std::string& message) {
                if (!second.empty() && second != served) { client->session.sequenced(second.data() + 2, second.size() - 2); served = second; }
                client->session.sequenced(message.data() + 2, message.size() - 2);
            };
            for (const auto& kept : kept_0_) serve(kept.second, kept.message);
            for (const auto& kept : kept_1_) serve(kept.second, kept.message);
            for (const auto& kept : kept_2_) serve(kept.second, kept.message);
            for (const auto& kept : kept_3_) serve(kept.second, kept.message);
            for (const auto& [key, kept] : kept_4_) serve(kept.second, kept.message);
            for (const auto& [reference, resting] : book_) {
                const auto stamp = stamped_.find(reference);
                serve(stamp == stamped_.end() ? std::string{} : stamp->second, resting);
            }
            // End Of Snapshot Message: where the live feed resumes, big-endian in 8 bytes.
            {
                char end[9] = { 'G' };
                const std::uint64_t resumes = seen_ + 1;
                for (std::size_t at = 0; at < 8; ++at) end[1 + at] = static_cast<char>((resumes >> (8 * (7 - at))) & 0xff);
                client->session.sequenced(end, sizeof end);
            }
            client->session.end_of_session();
            client->serving = false;
            worked = true;
        }
        return worked;
    }

    // The time: every session fires what is due by it - its heartbeats, and the end of a client
    // gone silent. The loop calls it.
    void tick() { for (auto& held : clients_) held.second->session.tick(); }

    // Has a client's session ended - gone silent past what the session allows? Its connection is
    // then the loop's to close.
    bool ended(std::size_t client) const {
        const auto held = clients_.find(client);
        return held == clients_.end() || held->second->lost;
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
        void silent() { lost = true; live = false; serving = false; }   // heard nothing for longer than the session allows
        bool lost = false;
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
    struct Kept { std::string second; std::string message; };   // a message, and the Seconds message it was stamped under
    std::string second_;   // the feed's last Seconds message, which what follows it is stamped under
    std::vector<Kept> kept_0_;   // every system_event_message where fields.system_event is O
    std::vector<Kept> kept_1_;   // every system_event_message where fields.system_event is C
    std::vector<Kept> kept_2_;   // every price_tick_size_message
    std::vector<Kept> kept_3_;   // every orderbook_directory_message
    std::map<std::string, Kept> kept_4_;   // the last trading_state_message for each fields.orderbook_id
    std::map<std::uint64_t, std::string> stamped_;   // the Seconds message each resting order was added under
};

}
