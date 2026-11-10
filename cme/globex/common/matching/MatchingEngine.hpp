#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <deque>
#include <functional>
#include <limits>
#include <map>
#include <vector>

namespace matching {

enum class Side : std::uint8_t { Buy, Sell };

struct Order {
    std::uint64_t id;
    Side          side;
    std::int64_t  price;   // in ticks
    std::uint64_t qty;
};

// The price a market order stands at: it trades at any price, so it is ahead of every limit on
// its side, and it is no price an auction could cross at.
constexpr std::int64_t market(Side side) {
    return side == Side::Buy ? std::numeric_limits<std::int64_t>::max() : std::numeric_limits<std::int64_t>::min();
}

// Which auction a trade ended. None is a trade made in continuous trading.
enum class Cross : std::uint8_t { None, Opening, Intraday, Closing };

struct Trade {
    std::uint64_t aggressor_id;
    std::uint64_t resting_id;
    std::int64_t  price;    // resting (passive) price
    std::uint64_t qty;
    std::uint64_t match_id; // numbers the fill, shared by both sides of it
    // The auction this trade ended. An uncross has no aggressor: both sides waited for it, so
    // aggressor_id is the buy side and resting_id the sell side, and price is the one it crossed at.
    Cross         cross = Cross::None;
};

// What an auction would cross at now: the price, how much would trade there, what would be left
// over on the heavier side, and whether that side is the buyers'. Nothing crosses when qty is zero,
// and neither side is heavier when surplus is.
struct Indicative {
    std::int64_t  price   = 0;
    std::uint64_t qty     = 0;
    std::uint64_t surplus = 0;
    bool          buyers  = false;
};

// The cancel callback of an engine whose owner does not listen for cancels.
struct IgnoreCancel {
    void operator()(const Order&) const {}
};

// The algorithms a book may allocate a price level by: each its steps in order, ending in time
// order. price_time is time order alone, which a book given nothing allocates by.
enum class Algorithm : std::uint8_t { price_time, fifo, allocation };

// A book's algorithm and the parameters its product is configured with. An allocation below
// pro_rata_minimum is no allocation; an order is TOP only with at least top_minimum lots, and takes
// no more than top_maximum lots as TOP - zero is no limit.
struct Allocation {
    Algorithm     algorithm        = Algorithm::price_time;
    std::uint64_t pro_rata_minimum = 1;
    std::uint64_t top_minimum      = 1;
    std::uint64_t top_maximum      = 0;
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

    // The algorithm this book allocates a level by, and the parameters its product is configured
    // with; a book given none allocates in time order.
    void allocate(Allocation allocation) { allocation_ = allocation; }
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
        uncrown(id);   // canceling the TOP order gives TOP to no one
        on_canceled_(removed);
        return true;
    }

    // An order held without crossing: an auction collects, and nothing trades until it uncrosses.
    // The book may stand crossed while it does.
    void hold(Order order) { rest(order); }

    // The price an auction would cross at now, inside the band [floor, ceiling]. Of every price an
    // order is limited to - one beyond the band at the limit it is willing at, a buy above the
    // ceiling at the ceiling and a sell below the floor at the floor; a market order has no price
    // of its own:
    //   the most quantity executable,
    //   then the least surplus left on the heavier side,
    //   then the market's pressure: the higher price when buyers are left over, the lower when
    //   sellers are,
    //   then the nearest the reference price,
    //   then the higher price.
    // Checked against a venue's feed rather than assumed. On Tadawul's 2026-06-22 day, wherever the
    // book its feed showed was all the auction held, this is the price and quantity it published in
    // 9,481 of 9,484 indicatives (opening, intraday and closing; band 10% of the reference, rounded
    // in to a tick). Two that differ balance exactly, with no surplus to press either way, and the
    // venue's choice between those prices is not this rule's; the third is not explained. One day
    // was not enough to say more.
    Indicative indicative(std::int64_t reference,
                          std::int64_t floor   = std::numeric_limits<std::int64_t>::min(),
                          std::int64_t ceiling = std::numeric_limits<std::int64_t>::max()) const {
        Indicative best{};
        auto take = [&](std::int64_t price) {
            std::uint64_t bought = 0, sold = 0;
            for (const auto& [level, orders] : bids_) { if (level < price) break; for (const auto& o : orders) bought += o.qty; }
            for (const auto& [level, orders] : asks_) { if (level > price) break; for (const auto& o : orders) sold += o.qty; }
            const std::uint64_t qty = std::min(bought, sold);
            if (qty == 0) return;
            const std::uint64_t surplus = bought > sold ? bought - sold : sold - bought;
            const bool buyers = bought > sold;
            const auto distance = [&](std::int64_t p) { return p > reference ? p - reference : reference - p; };
            const bool better =
                qty != best.qty         ? qty > best.qty :
                surplus != best.surplus ? surplus < best.surplus :
                surplus != 0 && buyers == best.buyers && price != best.price ? (buyers ? price > best.price : price < best.price) :
                distance(price) != distance(best.price) ? distance(price) < distance(best.price) :
                                          price > best.price;
            if (best.qty == 0 || better) best = Indicative{ price, qty, surplus, buyers };
        };
        for (const auto& level : bids_) {
            if (level.first != market(Side::Buy) && level.first >= floor) take(std::min(level.first, ceiling));
        }
        for (const auto& level : asks_) {
            if (level.first != market(Side::Sell) && level.first <= ceiling) take(std::max(level.first, floor));
        }
        return best;
    }

    // The next match number of the day's, for an execution that is no fill of the book's: the one
    // print an uncross makes of all its fills.
    std::uint64_t numbered() { return ++matches_; }

    // The uncross: everything an auction collected that crosses trades at one price, the best bids
    // against the best asks, first come first filled within a price. Returns what it crossed at.
    Indicative uncross(Cross cross, std::int64_t reference,
                       std::int64_t floor   = std::numeric_limits<std::int64_t>::min(),
                       std::int64_t ceiling = std::numeric_limits<std::int64_t>::max()) {
        const auto at = indicative(reference, floor, ceiling);
        std::uint64_t left = at.qty;

        while (left > 0 && !bids_.empty() && !asks_.empty()) {
            auto bid = bids_.begin();
            auto ask = asks_.begin();
            if (bid->first < at.price || ask->first > at.price) break;

            Order& buy  = bid->second.front();
            Order& sell = ask->second.front();
            const std::uint64_t fill = std::min({ buy.qty, sell.qty, left });

            on_trade_(Trade{ buy.id, sell.id, at.price, fill, ++matches_, cross });
            buy.qty  -= fill;
            sell.qty -= fill;
            left     -= fill;

            if (buy.qty == 0)  bid->second.pop_front();
            if (sell.qty == 0) ask->second.pop_front();
            if (bid->second.empty()) bids_.erase(bid);
            if (ask->second.empty()) asks_.erase(ask);
        }
        return at;
    }

    // An order that may trade only at one price - trade at last, at the closing auction's: it
    // crosses what rests at exactly that price, and the rest of it rests there.
    void on_new_order_at(Order order, std::int64_t price) {
        order.price = price;
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level == price; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level == price; });

        if (order.qty > 0) rest(order);
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
            level(in, queue, best->first);
            if (queue.empty()) opposite.erase(best);
        }
    }

    void rest(const Order& o) {
        crown(o);
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

    // One price level a match reaches, allocated as the book's algorithm says: each step takes what
    // it can of what the aggressor has left, and time order takes the rest. An aggressor at least as
    // large as all that rests here takes the level in time order whatever the algorithm, as Cme's
    // FIFO exception states.
    void level(Order& in, std::deque<Order>& queue, std::int64_t price) {
        std::uint64_t resting = 0;
        for (const auto& o : queue) resting += o.qty;
        if (in.qty < resting) {
            switch (allocation_.algorithm) {
            case Algorithm::allocation: top(in, queue, price); pro_rata(in, queue, price); break;
            default: break;
            }
        }
        fifo(in, queue, price);
    }

    void fill(Order& in, Order& resting, std::int64_t price, std::uint64_t qty) {
        on_trade_(Trade{ in.id, resting.id, price, qty, ++matches_ });
        in.qty -= qty;
        resting.qty -= qty;
    }

    // Time order, earliest first.
    void fifo(Order& in, std::deque<Order>& queue, std::int64_t price) {
        while (in.qty > 0 && !queue.empty()) {
            Order& resting = queue.front();
            fill(in, resting, price, std::min(in.qty, resting.qty));
            if (resting.qty == 0) { uncrown(resting.id); queue.pop_front(); }
        }
    }

    // What a step filled completely leaves the level.
    void sweep(std::deque<Order>& queue) {
        for (auto at = queue.begin(); at != queue.end();) {
            if (at->qty == 0) { uncrown(at->id); at = queue.erase(at); }
            else ++at;
        }
    }

    // Pro Rata: each order's working quantity over the level's, times what the aggressor has left,
    // rounded down - every share worked out before any is filled - and a share below the product's
    // minimum is no share. What rounding leaves goes on to the next step.
    void pro_rata(Order& in, std::deque<Order>& queue, std::int64_t price) {
        if (in.qty == 0) return;
        std::uint64_t working = 0;
        for (const auto& o : queue) working += o.qty;
        if (working == 0) return;
        const std::uint64_t match = std::min(in.qty, working);
        std::vector<std::uint64_t> shares;
        shares.reserve(queue.size());
        for (const auto& o : queue) {
            // Widened for the product: two quantities may not fit a 64 bit product between them.
            const auto share = static_cast<std::uint64_t>(static_cast<unsigned __int128>(o.qty) * match / working);
            shares.push_back(share >= allocation_.pro_rata_minimum ? share : 0);
        }
        for (std::size_t at = 0; at < queue.size() && in.qty > 0; ++at)
            if (shares[at] > 0) fill(in, queue[at], price, std::min(shares[at], in.qty));
        sweep(queue);
    }

    // TOP: the order that bettered its side of the market, with at least the product's minimum,
    // takes what it can before anyone else at its price, up to the product's maximum.
    void top(Order& in, std::deque<Order>& queue, std::int64_t price) {
        std::uint64_t& crowned = in.side == Side::Buy ? top_ask_ : top_bid_;   // the resting side's
        std::uint64_t& taken   = in.side == Side::Buy ? top_ask_taken_ : top_bid_taken_;
        if (crowned == 0 || in.qty == 0) return;
        for (auto& o : queue) {
            if (o.id != crowned) continue;
            const std::uint64_t room = allocation_.top_maximum == 0 ? o.qty : std::min(o.qty, allocation_.top_maximum - taken);
            const std::uint64_t qty = std::min(in.qty, room);
            fill(in, o, price, qty);
            taken += qty;
            // TOP is held until the order is filled, or has taken the product's maximum.
            if (allocation_.top_maximum != 0 && taken >= allocation_.top_maximum) crowned = 0;
            break;
        }
        sweep(queue);
    }

    // An order resting at a price that betters its side - a new best bid or offer - is TOP, given at
    // least the product's minimum: one order a side, and a new TOP takes it from the one before.
    void crown(const Order& o) {
        if (!(allocation_.algorithm == Algorithm::allocation) || o.qty < allocation_.top_minimum) return;
        const bool betters = o.side == Side::Buy
            ? bids_.empty() || o.price > bids_.begin()->first
            : asks_.empty() || o.price < asks_.begin()->first;
        if (!betters) return;
        (o.side == Side::Buy ? top_bid_ : top_ask_) = o.id;
        (o.side == Side::Buy ? top_bid_taken_ : top_ask_taken_) = 0;
    }

    // An order out of the book is TOP no longer, and gives TOP to no one.
    void uncrown(std::uint64_t id) {
        if (top_bid_ == id) top_bid_ = 0;
        if (top_ask_ == id) top_ask_ = 0;
    }

    Allocation    allocation_;
    std::uint64_t top_bid_ = 0, top_ask_ = 0;               // the TOP order of each side, 0 for none
    std::uint64_t top_bid_taken_ = 0, top_ask_taken_ = 0;   // what each has taken as TOP
    OnTrade    on_trade_;
    OnRest     on_rest_;
    OnCanceled on_canceled_;
    std::uint64_t matches_ = 0;
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> bids_;  // best = highest
    std::map<std::int64_t, std::deque<Order>>                            asks_;  // best = lowest
};}
