#pragma once

#include "MatchingEngine.hpp"
#include "TradingPhases.hpp"
#include "protocols/ipc/IpcBus.hpp"

#include <array>
#include <cstddef>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace sim::components {

// The matching engine as a component: framed Ipc orders in from one link, framed Ipc events out
// through `emit`, and a book of its own that nothing else touches.
template <typename Emit, const auto& Traded>
class Engine {
    // The book's callbacks, as named types rather than lambdas.
    //
    // MatchingEngine takes its callbacks as template arguments so that a match calls through
    // nothing, and the venue deduces them from the lambdas it passes. A component cannot: a
    // lambda capturing `this` has no name to declare a member with. So each callback is a struct
    // holding the component it belongs to, which deduces the same way and costs the same
    // nothing - a std::function here would put an indirect call on every trade.
    struct OnTrade {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Trade& trade) const { self->matched(trade, book); }
    };

    struct OnRested {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order) const { sim::emit_rested(self->emit_, order, Traded[book]); }
    };

    struct OnCanceled {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order) const { sim::emit_canceled(self->emit_, order, Traded[book]); }
    };

    struct OnEliminated {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order) const { sim::emit_eliminated(self->emit_, order, Traded[book]); }
    };

    struct OnAccepted {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order) const { sim::emit_accepted(self->emit_, order, Traded[book]); }
    };

    struct OnReplaced {
        Engine*     self;
        std::size_t book;
        void operator()(std::uint64_t replaced, const matching::Order& order) const { sim::emit_replaced(self->emit_, replaced, order, Traded[book]); }
    };

    struct OnReduced {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order, std::uint64_t taken) const { sim::emit_reduced(self->emit_, order, taken, Traded[book]); }
    };

    using Book = matching::MatchingEngine<OnTrade, OnRested, OnCanceled, OnEliminated, OnAccepted, OnReplaced, OnReduced>;

    // The books, one for each instrument the engine trades and in the venue's order, and the
    // routing between them: an order goes to the book of the instrument it names, and a cancel
    // to the book of the instrument its order is for. An order for an instrument the engine
    // does not trade is said so and left alone: the venue has no message for refusing it yet.
    class Books {
    public:
        explicit Books(Engine* self) : books_(made(self, std::make_index_sequence<Traded.size()>{})) {
            books_[0].ticked(matching::Ticks{ { { 0, 1 }, { 10000, 100 } } });
            books_[1].ticked(matching::Ticks{ { { 0, 1 }, { 10000, 100 } } });
            books_[2].ticked(matching::Ticks{ { { 0, 1 }, { 10000, 100 } } });
        }

        void on_new_order(const matching::Order& order, std::string_view instrument) {
            if (Book* book = find(instrument)) book->on_new_order(order);
            else std::printf("UNTRADED instrument: order %llu left alone\n", (unsigned long long)order.id);
        }

        bool on_cancel(std::uint64_t id, std::string_view instrument) {
            Book* book = find(instrument);
            return book != nullptr && book->on_cancel(id);
        }

        Book& operator[](std::size_t at) { return books_[at]; }

    private:
        // A book for each instrument, each callback knowing which book it is.
        template <std::size_t... At>
        static std::array<Book, sizeof...(At)> made(Engine* self, std::index_sequence<At...>) {
            return { Book(OnTrade{ self, At }, OnRested{ self, At }, OnCanceled{ self, At }, OnEliminated{ self, At }, OnAccepted{ self, At }, OnReplaced{ self, At }, OnReduced{ self, At })... };
        }

        Book* find(std::string_view instrument) {
            for (std::size_t at = 0; at < books_.size(); ++at)
                if (Traded[at] == instrument) return &books_[at];
            return nullptr;
        }

        std::array<Book, Traded.size()> books_;
    };

    // The day's callbacks: an order a phase does not accept, what the venue announces as its
    // phases change, each auction it uncrosses, and what the auction it collects for would cross
    // at when asked, all framed onto the Ipc for the gateway and the feeds.
    struct OnRefused {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order) const { sim::emit_refused(self->emit_, order, sim::Refusal::Phase, Traded[book]); }
        void operator()(const matching::Order& order, sim::Refusal reason, std::string_view instrument) const { sim::emit_refused(self->emit_, order, reason, instrument); }
                  void pending(std::uint64_t id, std::string_view instrument) const { sim::emit_cancel_pending(self->emit_, id, instrument); }
                  void reduce(std::uint64_t id, std::string_view instrument) const { sim::emit_reduce_refused(self->emit_, id, instrument); }
                  void pending(std::uint64_t id) const { sim::emit_cancel_pending(self->emit_, id, Traded[book]); }
                  void reduce(std::uint64_t id) const { sim::emit_reduce_refused(self->emit_, id, Traded[book]); }
                  // why a phase of matching refused it, as its chart says
                  void operator()(const matching::Order& order, matching::Reason reason) const {
                      switch (reason) {
                          case matching::Reason::Lifetime: sim::emit_refused(self->emit_, order, sim::Refusal::Lifetime, Traded[book]); return;
                          case matching::Reason::Type: sim::emit_refused(self->emit_, order, sim::Refusal::Type, Traded[book]); return;
                          case matching::Reason::Cross: sim::emit_refused(self->emit_, order, sim::Refusal::Cross, Traded[book]); return;
                          case matching::Reason::Aggressive: sim::emit_refused(self->emit_, order, sim::Refusal::Aggressive, Traded[book]); return;
                          case matching::Reason::Peg: sim::emit_refused(self->emit_, order, sim::Refusal::Peg, Traded[book]); return;
                          case matching::Reason::Unreferenced: sim::emit_refused(self->emit_, order, sim::Refusal::Unreferenced, Traded[book]); return;
                          case matching::Reason::Phase: sim::emit_refused(self->emit_, order, sim::Refusal::Phase, Traded[book]); return;
                          case matching::Reason::Slid: sim::emit_refused(self->emit_, order, sim::Refusal::Slid, Traded[book]); return;
                          case matching::Reason::FatFinger: sim::emit_refused(self->emit_, order, sim::Refusal::FatFinger, Traded[book]); return;
                          case matching::Reason::LimitProtection: sim::emit_refused(self->emit_, order, sim::Refusal::LimitProtection, Traded[book]); return;
                      }
                  }
    };

    struct OnAnnounced {
        Engine* self;
        std::size_t book;
        void operator()(matching::Announcement announcement) const { sim::emit_announced(self->emit_, announcement, self->server_.phase_reason(), self->server_.phase_release(), self->days_[book].collar_reference(), self->days_[book].collar_upper(), self->days_[book].collar_lower(), self->days_[book].collar_extension(), Traded[book]); }
    };

    struct OnCrossed {
        Engine* self;
        std::size_t book;
        void operator()(matching::Cross cross, const matching::Indicative& at, std::uint64_t match) const { sim::emit_crossed(self->emit_, cross, at, match == 0 ? 0 : ++self->matched_, Traded[book]); }
    };
    struct OnIndicated {
        Engine* self;
        std::size_t book;
        void operator()(matching::Cross cross, const matching::Indicative& at) const { sim::emit_indicated(self->emit_, cross, at, Traded[book]); }
    };
    using Day = matching::TradingPhases<Book, OnRefused, OnAnnounced, OnCrossed, OnIndicated>;
    // A day for each book, in the venue's order: an order goes to the day of the instrument it
    // names, and each change of phase is made in every day.
    class Days {
    public:
        Days(Engine* self, Books& books) : days_(made(self, books, std::make_index_sequence<Traded.size()>{})) {}

        void on_new_order(const matching::Order& order, std::string_view instrument) {
            if (Day* day = find(instrument)) day->on_new_order(order);
            else std::printf("UNTRADED instrument: order %llu left alone\n", (unsigned long long)order.id);
        }

        bool on_cancel(std::uint64_t id, std::string_view instrument) {
            Day* day = find(instrument);
            return day != nullptr && day->on_cancel(id);
        }

                  // A replace, to the day of the instrument it names: the replacement, and the order it replaces.
                  void on_replace(const matching::Order& order, std::uint64_t replaced, std::string_view instrument) {
                      // A day whose chart describes no replace takes none: the venue's Ipc may carry
                      // one its basic day does not answer.
                      if constexpr (requires(Day& day) { day.on_replace(order, replaced); }) {
                          if (Day* day = find(instrument)) { day->on_replace(order, replaced); return; }
                          std::printf("UNTRADED instrument: replace of order %llu left alone\n", (unsigned long long)replaced);
                      } else {
                          (void)order; (void)instrument;
                          std::printf("UNANSWERED replace: the day's chart describes none, order %llu left alone\n", (unsigned long long)replaced);
                      }
                  }

                  // A reduce, to the day of the instrument it names: the order, and the size to leave it at.
                  bool on_reduce(std::uint64_t id, std::uint64_t qty, std::string_view instrument) {
                      Day* day = find(instrument);
                      return day != nullptr && day->on_reduce(id, qty);
                  }

        // An order or a cancel that names no instrument - a test's - is for the venue's first.
        void on_new_order(const matching::Order& order) { days_[0].on_new_order(order); }
        bool on_cancel(std::uint64_t id) { return days_[0].on_cancel(id); }

        bool preopen() { bool moved = false; for (auto& day : days_) moved = day.preopen() || moved; return moved; }
        bool openinglockin() { bool moved = false; for (auto& day : days_) moved = day.openinglockin() || moved; return moved; }
        bool halt() { bool moved = false; for (auto& day : days_) moved = day.halt() || moved; return moved; }
        bool open() { bool moved = false; for (auto& day : days_) moved = day.open() || moved; return moved; }
        bool openinglockout() { bool moved = false; for (auto& day : days_) moved = day.openinglockout() || moved; return moved; }
        bool openinglate() { bool moved = false; for (auto& day : days_) moved = day.openinglate() || moved; return moved; }
        bool preclose() { bool moved = false; for (auto& day : days_) moved = day.preclose() || moved; return moved; }
        bool quote() { bool moved = false; for (auto& day : days_) moved = day.quote() || moved; return moved; }
        bool postpone() { bool moved = false; for (auto& day : days_) moved = day.postpone() || moved; return moved; }
        bool resume() { bool moved = false; for (auto& day : days_) moved = day.resume() || moved; return moved; }
        bool closinglockout() { bool moved = false; for (auto& day : days_) moved = day.closinglockout() || moved; return moved; }
        bool close() { bool moved = false; for (auto& day : days_) moved = day.close() || moved; return moved; }
        bool closingfreeze() { bool moved = false; for (auto& day : days_) moved = day.closingfreeze() || moved; return moved; }
        bool extendedtradingcloseend() { bool moved = false; for (auto& day : days_) moved = day.extendedtradingcloseend() || moved; return moved; }
        bool end() { bool moved = false; for (auto& day : days_) moved = day.end() || moved; return moved; }
        void indicate(bool early) { for (auto& day : days_) day.indicate(early); }
        void at(std::int64_t now) { for (auto& day : days_) day.at(now); }
        void clock(std::int64_t now) { for (auto& day : days_) day.clock(now); }

        Day& operator[](std::size_t at) { return days_[at]; }

                  // Each book's count of changes, and a held halt of each book the change moved, where the change that
                  // starts it names no instrument: a book it found already halted is not held anew.
                  std::array<std::uint64_t, Traded.size()> moves() const { std::array<std::uint64_t, Traded.size()> made{}; for (std::size_t at = 0; at < made.size(); ++at) made[at] = days_[at].phases(); return made; }
                  void holding(matching::Hold hold, const std::array<std::uint64_t, Traded.size()>& before) { for (std::size_t at = 0; at < days_.size(); ++at) days_[at].holding(hold, before[at]); }

        // The day of one instrument, where a change is that instrument's alone; none where the venue lists no such one.
        Day* of(std::string_view instrument) { return find(instrument); }

    private:
        // A day for each book, each callback knowing which book it is.
        template <std::size_t... At>
        static std::array<Day, sizeof...(At)> made(Engine* self, Books& books, std::index_sequence<At...>) {
            return { Day(books[At], OnRefused{ self, At }, OnAnnounced{ self, At }, OnCrossed{ self, At }, OnIndicated{ self, At })... };
        }

        Day* find(std::string_view instrument) {
            for (std::size_t at = 0; at < days_.size(); ++at)
                if (Traded[at] == instrument) return &days_[at];
            return nullptr;
        }

        std::array<Day, Traded.size()> days_;
    };

public:
    Engine(Emit emit, std::atomic<std::uint64_t>& trades)
        : emit_(std::move(emit)),
          trades_(trades),
          books_(this),
          days_(this, books_),
          server_(days_, OnRefused{ this, 0 }) { for (std::size_t at = 0; at < Traded.size(); ++at) days_[at].reference(sim::reference_of(Traded[at]));  days_[0].collar(10000, 9500, 10500, 1500); days_[1].collar(10000, 9500, 10500, 1500); days_[2].collar(10000, 9500, 10500, 1500);}

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
        return inbound.read([this](std::string_view message) { server_.on_message(message); if (server_.book_asked()) told(server_.book_sequence()); if (const auto asked = server_.break_asked()) broke(asked->first, asked->second); });
    }

    // A book, for a test that puts an order in without going through a link: the venue's first
    // where it names none.
    Book& book(std::size_t at = 0) { return books_[at]; }

    // The trading day, for a test that walks it by hand.
    Days& day() { return days_; }
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
        for (std::size_t book = 0; book < 3; ++book) books_[book].each([&](const matching::Order& order, char where) { said(order, where, sequence, book); ++orders; });
        sim::emit_book_end(emit_, orders, sequence);
        if (dump_ == nullptr) return;
        std::fprintf(dump_, "# %llu,%llu\n", static_cast<unsigned long long>(sequence), static_cast<unsigned long long>(orders));
        std::fflush(dump_);
    }

    ~Engine() { if (dump_ != nullptr) std::fclose(dump_); }


private:
    void matched(const matching::Trade& trade, std::size_t book) {
        trades_.fetch_add(1, std::memory_order_relaxed);
        std::printf("TRADE aggressor=%llu resting=%llu px=%lld qty=%llu\n",
                    (unsigned long long)trade.aggressor_id, (unsigned long long)trade.resting_id,
                    (long long)trade.price, (unsigned long long)trade.qty);
        // The venue's own number for the match: a book numbers only its own, and the venue counts once
                  // over all its orderbooks, so a number names one match venue-wide.
                  matching::Trade numbered = trade;
                  numbered.match_id = ++matched_;
                  sim::emit_trade(emit_, numbered, Traded[book]);
                  remember(numbered, book);
    }

    // Each match the engine made today, by the number its fills were told under: its two orders and
    // its book. A match is broken once, and forgotten then; one never broken is kept the day.
    struct Made {
        std::uint64_t aggressor;
        std::uint64_t resting;
        std::size_t   book;
    };

    void remember(const matching::Trade& trade, std::size_t book) {
        made_[trade.match_id] = Made{ trade.aggressor_id, trade.resting_id, book };
    }

    // A match broken, as a supervisor asked: told with its two orders, the book unchanged. A number
    // the engine gave no match - or one already broken - is refused, and nothing is said.
    void broke(std::uint64_t match, char reason) {
        const auto found = made_.find(match);
        if (found == made_.end()) {
            std::printf("BREAK refused: no match %llu\n", static_cast<unsigned long long>(match));
            return;
        }
        std::printf("BREAK match=%llu reason=%c\n", static_cast<unsigned long long>(match), reason);
        sim::emit_trade_broken(emit_, match, found->second.aggressor, found->second.resting, reason, Traded[found->second.book]);
        made_.erase(found);
    }

    std::unordered_map<std::uint64_t, Made> made_;

    Emit                        emit_;
    std::atomic<std::uint64_t>& trades_;
    Books                       books_;
    std::uint64_t               matched_ = 0;   // the last match number it gave, one count over all its books
    Days                        days_;
    sim::IpcEngineServer<Days, OnRefused>  server_;
    // A template, so what it says of an order shown is said only where the venue's orders say so.
    template <typename Held>
    void said(const Held& order, char where, std::uint64_t sequence, std::size_t book) {
        sim::emit_book_order(emit_, order, where, sequence, Traded[book]);
        if (dump_ == nullptr) return;
        int shown = 1;
        if constexpr (requires { order.shown; }) shown = order.shown ? 1 : 0;
        unsigned long long showing = shown ? static_cast<unsigned long long>(order.qty) : 0;
        if (shown) showing = static_cast<unsigned long long>(matching::showing(order));   // a reserve order shows its floor's worth
        std::fprintf(dump_, "%llu,%zu,%c,%c,%lld,%llu,%llu,%d,%llu\n", static_cast<unsigned long long>(sequence), book, where,
                     order.side == matching::Side::Buy ? 'B' : 'S', static_cast<long long>(order.price),
                     static_cast<unsigned long long>(order.qty), static_cast<unsigned long long>(order.id), shown, showing);
    }

    std::FILE* dump_ = nullptr;   // the file its book is also written to, where its settings name one
};

}