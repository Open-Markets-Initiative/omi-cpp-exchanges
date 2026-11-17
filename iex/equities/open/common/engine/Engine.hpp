#pragma once

#include "MatchingEngine.hpp"
#include "TradingPhases.hpp"
#include "protocols/ipc/IpcBus.hpp"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <string>
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

    struct OnEliminated {
        Engine* self;
        void operator()(const matching::Order& order) const { sim::emit_eliminated(self->emit_, order); }
    };

    struct OnAccepted {
        Engine* self;
        void operator()(const matching::Order& order) const { sim::emit_accepted(self->emit_, order); }
    };

    struct OnReplaced {
        Engine* self;
        void operator()(std::uint64_t replaced, const matching::Order& order) const { sim::emit_replaced(self->emit_, replaced, order); }
    };

    using Book = matching::MatchingEngine<OnTrade, OnRested, OnCanceled, OnEliminated, OnAccepted, OnReplaced>;

    // The day's callback: an order the phase it arrived in does not accept, framed onto the Ipc
    // for the gateway.
    struct OnRefused {
        Engine* self;
        void operator()(const matching::Order& order) const { sim::emit_refused(self->emit_, order, sim::Refusal::Phase); }
        void operator()(const matching::Order& order, sim::Refusal reason) const { sim::emit_refused(self->emit_, order, reason); }
                  void cancel(std::uint64_t id) const { sim::emit_cancel_refused(self->emit_, id); }
        // why the phase refused it, as its chart says
        void operator()(const matching::Order& order, matching::Reason reason) const {
            switch (reason) {
                case matching::Reason::Type: sim::emit_refused(self->emit_, order, sim::Refusal::Type); return;
                case matching::Reason::Lifetime: sim::emit_refused(self->emit_, order, sim::Refusal::Lifetime); return;
            }
        }
    };

    using Day = matching::TradingPhases<Book, OnRefused>;

public:
    Engine(Emit emit, std::atomic<std::uint64_t>& trades)
        : emit_(std::move(emit)),
          trades_(trades),
          book_(OnTrade{this}, OnRested{this}, OnCanceled{this}, OnEliminated{this}, OnAccepted{this}, OnReplaced{this}),
          day_(book_, OnRefused{this}),
          server_(day_, OnRefused{ this }) {}

    // The callbacks hold the component itself, so it stays where it was built.
    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&)                 = delete;
    Engine& operator=(Engine&&)      = delete;

    // One pass: at most one order off the link, matched and its events emitted. False when there
    // was nothing there, which is what tells a component thread to back off.
    template <typename Inbound>
    bool step(Inbound& inbound) {
        return inbound.read([this](std::string_view message) { server_.on_message(message); if (server_.book_asked()) told(server_.book_sequence()); });
    }

    // The book itself, for a test that puts an order in without going through a link.
    Book& book() { return book_; }

    // The trading day, for a test that walks it by hand.
    Day& day() { return day_; }
    // The file the engine's book is also written to as it is said; none where the path is empty. A
    // line is
    //   sequence,book,where,side,price,quantity,order,shown,showing
    // the place of the request answered in the sequencer's order, 0 where none numbered it; the
    // book, the venue's instruments in the order it declares them, from 0; where the engine holds
    // the order (MatchingEngine.hpp, each()); B or S; the price in the engine's own units, a market
    // order's the engine's own sentinel; what is left of it; its number; 1 where the book shows it;
    // and how much of it the book shows - all of it, a reserve order's floor, or nothing - which is
    // what a book built from the venue's feed is held against.
    // Each answer ends with a line "# sequence,orders".
    void dump(const std::string& path) {
        if (path.empty()) return;
        dump_ = std::fopen(path.c_str(), "w");
        if (dump_ == nullptr) { std::printf("BOOK dump: cannot write %s\n", path.c_str()); return; }
        std::fprintf(dump_, "sequence,book,where,side,price,quantity,order,shown,showing\n");
    }

    // Its own book, said as of the message that asked: every order it holds, then the end of them.
    void told(std::uint64_t sequence) {
        std::uint64_t orders = 0;
        book_.each([&](const matching::Order& order, char where) { said(order, where, sequence, 0); ++orders; });
        sim::emit_book_end(emit_, orders, sequence);
        if (dump_ == nullptr) return;
        std::fprintf(dump_, "# %llu,%llu\n", static_cast<unsigned long long>(sequence), static_cast<unsigned long long>(orders));
        std::fflush(dump_);
    }

    ~Engine() { if (dump_ != nullptr) std::fclose(dump_); }


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
    sim::IpcEngineServer<Day, OnRefused>  server_;
    // A template, so what it says of an order shown is said only where the venue's orders say so.
    template <typename Held>
    void said(const Held& order, char where, std::uint64_t sequence, std::size_t book) {
        sim::emit_book_order(emit_, order, where, sequence);
        if (dump_ == nullptr) return;
        int shown = 1;
        if constexpr (requires { order.shown; }) shown = order.shown ? 1 : 0;
        unsigned long long showing = shown ? static_cast<unsigned long long>(order.qty) : 0;
        std::fprintf(dump_, "%llu,%zu,%c,%c,%lld,%llu,%llu,%d,%llu\n", static_cast<unsigned long long>(sequence), book, where,
                     order.side == matching::Side::Buy ? 'B' : 'S', static_cast<long long>(order.price),
                     static_cast<unsigned long long>(order.qty), static_cast<unsigned long long>(order.id), shown, showing);
    }

    std::FILE* dump_ = nullptr;   // the file its book is also written to, where its settings name one
};

}