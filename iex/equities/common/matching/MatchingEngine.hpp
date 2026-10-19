#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <deque>
#include <functional>
#include <map>

namespace matching {

enum class Side : std::uint8_t { Buy, Sell };

struct Order {
    std::uint64_t id;
    Side          side;
    std::int64_t  price;   // in ticks
    std::uint64_t qty;
};

struct Trade {
    std::uint64_t aggressor_id;
    std::uint64_t resting_id;
    std::int64_t  price;    // resting (passive) price
    std::uint64_t qty;
    std::uint64_t match_id; // numbers the fill, shared by both sides of it
};

// The cancel callback of an engine whose owner does not listen for cancels.
struct IgnoreCancel {
    void operator()(const Order&) const {}
};

// Continuous price-time-priority matching engine for one instrument.
//   on_trade(Trade)          — a fill was generated (aggressor vs resting)
//   on_rest(const Order&)    — residual quantity now rests in the book
//   on_canceled(const Order&) — a resting order was removed by a cancel
// Pure logic: no I/O, no threading — the owner drives it and forwards the callbacks
// to the order gateway and the market-data feeds.
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreCancel>
class MatchingEngine {
public:
    MatchingEngine(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled) {}

    void on_new_order(Order order) {
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level <= order.price; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level >= order.price; });

        if (order.qty > 0) rest(order);
    }

    // Remove a resting order by id, telling the owner what left the book. Returns true if it was found.
    bool on_cancel(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed)) return false;
        on_canceled_(removed);
        return true;
    }

private:
    // Best opposite level first (map ordering); FIFO within a level. Generates a Trade
    // per fill until the incoming order is exhausted or the book no longer crosses.
    template <typename Book, typename Crosses>
    void cross(Order& in, Book& opposite, Crosses crosses) {
        while (in.qty > 0 && !opposite.empty()) {
            auto best = opposite.begin();
            if (!crosses(best->first)) break;

            auto& queue = best->second;
            while (in.qty > 0 && !queue.empty()) {
                Order& resting = queue.front();
                std::uint64_t fill = std::min(in.qty, resting.qty);
                on_trade_(Trade{ in.id, resting.id, best->first, fill, ++matches_ });
                in.qty -= fill;
                resting.qty -= fill;
                if (resting.qty == 0) queue.pop_front();
            }
            if (queue.empty()) opposite.erase(best);
        }
    }

    void rest(const Order& o) {
        if (o.side == Side::Buy) bids_[o.price].push_back(o);
        else                     asks_[o.price].push_back(o);
        on_rest_(o);
    }

    template <typename Book>
    static bool erase(Book& book, std::uint64_t id, Order& removed) {
        for (auto it = book.begin(); it != book.end(); ++it) {
            auto& q = it->second;
            for (auto qi = q.begin(); qi != q.end(); ++qi) {
                if (qi->id == id) {
                    removed = *qi;
                    q.erase(qi);
                    if (q.empty()) book.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

    OnTrade    on_trade_;
    OnRest     on_rest_;
    OnCanceled on_canceled_;
    std::uint64_t matches_ = 0;
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> bids_;  // best = highest
    std::map<std::int64_t, std::deque<Order>>                            asks_;  // best = lowest
};}
