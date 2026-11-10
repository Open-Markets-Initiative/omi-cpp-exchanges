#pragma once

#include "MatchingEngine.hpp"
#include "TradingPhases.hpp"
#include "protocols/ipc/IpcBus.hpp"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <string_view>
#include <utility>

namespace sim::components {

// The matching engine as a component: framed Ipc orders in from one link, framed Ipc events out
// through `emit`, and a book of its own that nothing else touches.
template <typename Emit>
class Engine {
    // The book's callbacks, as named types rather than lambdas.
    //
    // MatchingEngine takes its callbacks as template arguments so that a match calls through
    // nothing, and the venue deduces them from the lambdas it passes. A component cannot: a
    // lambda capturing `this` has no name to declare a member with. So each callback is a struct
    // holding the component it belongs to, which deduces the same way and costs the same
    // nothing - a std::function here would put an indirect call on every trade.
    struct OnTrade {
        Engine* self;
        void operator()(const matching::Trade& trade) const { self->matched(trade); }
    };

    struct OnRested {
        Engine* self;
        void operator()(const matching::Order& order) const { sim::emit_rested(self->emit_, order); }
    };

    struct OnCanceled {
        Engine* self;
        void operator()(const matching::Order& order) const { sim::emit_canceled(self->emit_, order); }
    };

    using Book = matching::MatchingEngine<OnTrade, OnRested, OnCanceled>;

    // The day's callbacks: an order a phase does not accept, what the venue announces as its
    // phases change, each auction it uncrosses, and what the auction it collects for would cross
    // at when asked, all framed onto the Ipc for the gateway and the feeds.
    struct OnRefused {
        Engine* self;
        void operator()(const matching::Order& order) const { sim::emit_refused(self->emit_, order); }
    };

    struct OnAnnounced {
        Engine* self;
        void operator()(matching::Announcement announcement) const { sim::emit_announced(self->emit_, announcement); }
    };

    struct OnCrossed {
        Engine* self;
        void operator()(matching::Cross cross, const matching::Indicative& at, std::uint64_t match) const { sim::emit_crossed(self->emit_, cross, at, match); }
    };
    struct OnIndicated {
        Engine* self;
        void operator()(matching::Cross cross, const matching::Indicative& at) const { sim::emit_indicated(self->emit_, cross, at); }
    };
    using Day = matching::TradingPhases<Book, OnRefused, OnAnnounced, OnCrossed, OnIndicated>;

public:
    Engine(Emit emit, std::atomic<std::uint64_t>& trades)
        : emit_(std::move(emit)),
          trades_(trades),
          book_(OnTrade{this}, OnRested{this}, OnCanceled{this}),
          day_(book_, OnRefused{this}, OnAnnounced{this}, OnCrossed{this}, OnIndicated{this}),
          server_(day_) { day_.reference(sim::instrument_reference); }

    // The callbacks hold the component itself, so it stays where it was built.
    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&)                 = delete;
    Engine& operator=(Engine&&)      = delete;

    // One pass: at most one message off the link - an order matched, or a change of phase made -
    // and its events emitted. False when there was nothing there, which is what tells a component
    // thread to back off.
    template <typename Inbound>
    bool step(Inbound& inbound) {
        return inbound.read([this](std::string_view message) { server_.on_message(message); });
    }

    // The book itself, for a test that puts an order in without going through a link.
    Book& book() { return book_; }

    // The trading day, for a test that walks it by hand.
    Day& day() { return day_; }

private:
    void matched(const matching::Trade& trade) {
        trades_.fetch_add(1, std::memory_order_relaxed);
        std::printf("TRADE aggressor=%llu resting=%llu px=%lld qty=%llu\n",
                    (unsigned long long)trade.aggressor_id, (unsigned long long)trade.resting_id,
                    (long long)trade.price, (unsigned long long)trade.qty);
        sim::emit_trade(emit_, trade);
    }

    Emit                        emit_;
    std::atomic<std::uint64_t>& trades_;
    Book                        book_;
    Day                         day_;
    sim::IpcEngineServer<Day>  server_;
};

}