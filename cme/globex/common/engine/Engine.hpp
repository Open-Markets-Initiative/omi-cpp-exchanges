#pragma once

#include "MatchingEngine.hpp"
#include "protocols/ipc/IpcBus.hpp"

#include <array>
#include <cstddef>
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
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Trade& trade) const { self->matched(trade, book); }
    };

    struct OnRested {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order) const { sim::emit_rested(self->emit_, order, sim::instruments[book]); }
    };

    struct OnCanceled {
        Engine*     self;
        std::size_t book;
        void operator()(const matching::Order& order) const { sim::emit_canceled(self->emit_, order, sim::instruments[book]); }
    };

    using Book = matching::MatchingEngine<OnTrade, OnRested, OnCanceled>;

    // The books, one for each instrument the venue declares and in its order, and the routing
    // between them: an order goes to the book of the instrument it names, and a cancel to the
    // book of the instrument its order is for. An order for an instrument the venue does not
    // trade is said so and left alone: the venue has no message for refusing it yet.
    class Books {
    public:
        explicit Books(Engine* self)
            : books_{ Book(OnTrade{self, 0}, OnRested{self, 0}, OnCanceled{self, 0}), Book(OnTrade{self, 1}, OnRested{self, 1}, OnCanceled{self, 1}) } {}

        void on_new_order(const matching::Order& order, std::int32_t instrument) {
            if (Book* book = find(instrument)) book->on_new_order(order);
            else std::printf("UNTRADED instrument: order %llu left alone\n", (unsigned long long)order.id);
        }

        bool on_cancel(std::uint64_t id, std::int32_t instrument) {
            Book* book = find(instrument);
            return book != nullptr && book->on_cancel(id);
        }

        Book& operator[](std::size_t at) { return books_[at]; }

    private:
        Book* find(std::int32_t instrument) {
            for (std::size_t at = 0; at < books_.size(); ++at)
                if (sim::instruments[at] == instrument) return &books_[at];
            return nullptr;
        }

        std::array<Book, 2> books_;
    };

public:
    Engine(Emit emit, std::atomic<std::uint64_t>& trades)
        : emit_(std::move(emit)),
          trades_(trades),
          books_(this),
          server_(books_) { books_[0].allocate(matching::Allocation{ matching::Algorithm::fifo, 1, 1, 0 }); /* ESZ6 */  books_[1].allocate(matching::Allocation{ matching::Algorithm::allocation, 2, 1, 0 }); /* SR3Z6 */ }

    // The callbacks hold the component itself, so it stays where it was built.
    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&)                 = delete;
    Engine& operator=(Engine&&)      = delete;

    // One pass: at most one order off the link, matched and its events emitted. False when there
    // was nothing there, which is what tells a component thread to back off.
    template <typename Inbound>
    bool step(Inbound& inbound) {
        return inbound.read([this](std::string_view message) { server_.on_message(message); });
    }

    // A book, for a test that puts an order in without going through a link: the venue's first
    // where it names none.
    Book& book(std::size_t at = 0) { return books_[at]; }

private:
    void matched(const matching::Trade& trade, std::size_t book) {
        trades_.fetch_add(1, std::memory_order_relaxed);
        std::printf("TRADE aggressor=%llu resting=%llu px=%lld qty=%llu\n",
                    (unsigned long long)trade.aggressor_id, (unsigned long long)trade.resting_id,
                    (long long)trade.price, (unsigned long long)trade.qty);
        sim::emit_trade(emit_, trade, sim::instruments[book]);
    }

    Emit                        emit_;
    std::atomic<std::uint64_t>& trades_;
    Books                       books_;
    sim::IpcEngineServer<Books>  server_;
};

}