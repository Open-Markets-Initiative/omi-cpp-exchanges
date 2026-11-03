#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include "../protocols/orderentry/FixInitiator.hpp"
#include "../protocols/ipc/IpcBus.hpp"

namespace sim::traders {

namespace protocol = ::iexiexequities::order_entry;

// The tags a trader writes, as Iex.IexEquities.OrderEntry.Fix.v2.91 numbers them.
struct Tag {
    static constexpr std::uint32_t ClOrdID = 11;
    static constexpr std::uint32_t OrdType = 40;
    static constexpr std::uint32_t OrderQty = 38;
    static constexpr std::uint32_t OrigClOrdID = 41;
    static constexpr std::uint32_t Price = 44;
    static constexpr std::uint32_t Side = 54;
    static constexpr std::uint32_t Symbol = 55;
    static constexpr std::uint32_t TransactTime = 60;
};

// What the venue answered, counted by the message each response transform builds (IexEquities.Ipc.Transforms.xml).
struct Answers {
    std::uint64_t ExecutionReport = 0;
};

// A strategy's intent, as FIX requests on a client session. `Clock` tells the time each is
// sent at.
template <typename Session, typename Clock>
class Orders {
public:
    Orders(Session& session, Clock& clock) : session_(session), clock_(clock) {}

    std::uint64_t buy (std::int64_t price, std::uint64_t quantity) { return submit('B', price, quantity); }
    std::uint64_t sell(std::int64_t price, std::uint64_t quantity) { return submit('S', price, quantity); }

    // Give up an order the trader entered, by the reference it entered it under (fix_cancel_order).
    void cancel(std::uint64_t reference) {
        const auto found = entered_.find(reference);
        if (found == entered_.end()) return;
        const auto [side, price, quantity] = found->second;
        (void)side; (void)price; (void)quantity;
        const auto now = clock_();
        std::array<char, 512> body{};
        protocol::Writer out{body.data(), body.data() + body.size()};
        out.put(Tag::ClOrdID, static_cast<std::int64_t>(++next_));
        out.put(Tag::OrigClOrdID, static_cast<std::int64_t>(reference));
        out.put(Tag::Symbol, std::string_view{sim::instrument_symbol});
        out.put(Tag::Side, [](char value) { switch (value) { case 'B': return '1'; case 'S': return '2'; default: return value; } }(side));
        out.put(Tag::OrderQty, static_cast<std::int64_t>(quantity));
        out.put_time(Tag::TransactTime, now);
        send("F", body.data(), out, now);
        entered_.erase(found);
        ++canceled_;
    }

    std::uint64_t entered()  const { return entered_count_; }
    std::uint64_t canceled() const { return canceled_; }

private:
    // What an order was entered with, which a cancel has to restate.
    struct Entered {
        char side;
        std::int64_t price;
        std::uint64_t quantity;
    };

    // An order the trader enters, numbered by its own reference (fix_new_order).
    std::uint64_t submit(char side, std::int64_t price, std::uint64_t quantity) {
        const std::uint64_t reference = ++next_;
        const auto now = clock_();
        std::array<char, 512> body{};
        protocol::Writer out{body.data(), body.data() + body.size()};
        out.put(Tag::ClOrdID, static_cast<std::int64_t>(reference));
        out.put(Tag::Symbol, std::string_view{sim::instrument_symbol});
        out.put(Tag::Side, [](char value) { switch (value) { case 'B': return '1'; case 'S': return '2'; default: return value; } }(side));
        out.put_scaled(Tag::Price, static_cast<std::int64_t>(price), 4);
        out.put(Tag::OrderQty, static_cast<std::int64_t>(quantity));
        out.put(Tag::OrdType, '2');   // a trader entering a price enters a limit order
        out.put_time(Tag::TransactTime, now);
        send("D", body.data(), out, now);
        entered_[reference] = Entered{side, price, quantity};
        ++entered_count_;
        return reference;
    }

    // One request onto the session, which numbers, frames and keeps it.
    void send(std::string_view type, const char* body, const protocol::Writer& out, const std::int64_t now) {
        if (!out.overflowed) session_.message(type, std::string_view{body, out.used(body)}, now);
    }

    Session& session_;
    Clock& clock_;
    std::map<std::uint64_t, Entered> entered_;
    std::uint64_t next_ = 0;
    std::uint64_t entered_count_ = 0;
    std::uint64_t canceled_ = 0;
};

// A trader: a FIX client session of this venue, the orders it speaks through, and what came
// back. Its `Send` writes to the venue; the trader is its own session handler. The strategy
// stays outside it, driving `orders()`, so any strategy runs any venue. It logs on as its
// user name, which a FIX venue knows a client by (SenderCompID), to the venue's own (TargetCompID).
template <typename Send, typename Clock = protocol::SystemClock>
class Trader {
public:
    using Session = protocol::Initiator<Send, Trader&>;

    Trader(std::string name, std::string username, std::string password, Send send, Clock clock = {})
        : name_(std::move(name)), username_(std::move(username)), password_(std::move(password)), clock_(clock),
          session_(identity(), send, *this), orders_(session_, clock_) {}

    Trader(const Trader&)            = delete;
    Trader& operator=(const Trader&) = delete;

    // Join the venue's session, at the interval the session starts with. Nothing else is sent
    // until the login is accepted.
    void login() { session_.login(session_.heartbeat_interval(), clock_()); }

    Orders<Session, Clock>& orders() { return orders_; }

    void on_bytes(const char* data, std::size_t length) { session_.on_bytes(data, length, clock_()); }

    // The time: the session fires what is due - its heartbeats, its tests of a silent venue.
    void tick() { session_.tick(clock_()); }

    // The session's handler: what the venue said back.
    void login_accepted() {}
    void login_refused() { refused_ = true; }
    void logout() {}
    void message(std::string_view type, std::string_view) {
        if (type == "8") { ++answers_.ExecutionReport; return; }
    }

    const std::string& name() const { return name_; }
    std::string session_name() const { return "IEXG"; }
    const Answers& answers() const { return answers_; }
    bool logged_in() const { return session_.in_logged_on(); }
    bool refused() const { return refused_; }
    bool ended() const { return session_.ended(); }
    std::uint64_t entered() const { return orders_.entered(); }
    std::uint64_t canceled() const { return orders_.canceled(); }

private:
    // Who the trader is to the venue, and who the venue is: the comp ids its session names.
    protocol::InitiatorConfig identity() const {
        protocol::InitiatorConfig config{};
        config.sender_comp_id = username_;
        config.target_comp_id = "IEXG";
        return config;
    }

    std::string name_;
    std::string username_;
    std::string password_;
    Clock       clock_;
    Session     session_;
    Orders<Session, Clock> orders_;
    Answers     answers_{};
    bool        refused_ = false;
};

}  // namespace sim::traders
