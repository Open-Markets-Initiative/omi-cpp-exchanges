#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <deque>
#include <functional>
#include <limits>
#include <map>

namespace matching {

enum class Side : std::uint8_t { Buy, Sell };

// How long an order lives, as the venue's time in force means it.
//   Session   — until the trading session it was entered in ends
//   Canceled  — until it is canceled, through any number of sessions
//   Immediate — what does not trade as it arrives is eliminated
//   Date      — until the session of a date it names ends
enum class Lifetime : std::uint8_t { Session, Canceled, Immediate, Date };

struct Order {
    std::uint64_t id;
    Side          side;
    std::int64_t  price;   // in ticks
    std::uint64_t qty;
    Lifetime      lifetime = Lifetime::Session;   // how long it lives, as its time in force means
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

// Continuous price-time-priority matching engine for one instrument.
//   on_trade(Trade)             — a fill was generated (aggressor vs resting)
//   on_rest(const Order&)       — residual quantity now rests in the book
//   on_canceled(const Order&)   — a resting order was removed by a cancel
//   on_eliminated(const Order&) — what was left of an order never to rest was taken out
//   on_accepted(const Order&)   — the book took an order, as it now stands, before anything it makes of it
// Pure logic: no I/O, no threading — the owner drives it and forwards the callbacks
// to the order gateway and the market-data feeds.
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreCancel, typename OnEliminated = IgnoreCancel, typename OnAccepted = IgnoreCancel>
class MatchingEngine {
public:
    MatchingEngine(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(), on_eliminated_(), on_accepted_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(), on_accepted_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated, OnAccepted on_accepted)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(on_accepted) {}


    void on_new_order(Order order) {
        on_accepted_(order);   // taken, before anything it makes of it
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level <= order.price; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level >= order.price; });

        if (order.qty == 0) return;
        if (order.lifetime == Lifetime::Immediate || order.price == market(order.side)) on_eliminated_(order);   // never to rest: what is left goes
        else rest(order);
    }

    // Remove a resting order by id, telling the owner what left the book. Returns true if it was found.
    bool on_cancel(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed)) return false;
        on_canceled_(removed);
        return true;
    }

    // The session's end for what lives for it: every resting order of this lifetime leaves the
    // book, eliminated as one never to rest is, its owner told of each.
    void expire(Lifetime lifetime) {
        expired(bids_, lifetime);
        expired(asks_, lifetime);
    }

    template <typename Book>
    void expired(Book& book, Lifetime lifetime) {
        for (auto level = book.begin(); level != book.end(); ) {
            auto& queue = level->second;
            for (auto at = queue.begin(); at != queue.end(); ) {
                if (at->lifetime != lifetime) { ++at; continue; }
                const Order gone = *at;
                at = queue.erase(at);
                on_eliminated_(gone);
            }
            if (queue.empty()) level = book.erase(level);
            else ++level;
        }
    }

    // An order held without crossing: an auction collects, and nothing trades until it uncrosses.
    // The book may stand crossed while it does.
    // An order never to rest has nothing it could trade with as it arrives, so it is eliminated - the
    // simulator's reading of a time in force no venue document here states for an auction.
    void hold(Order order) { on_accepted_(order); if (order.lifetime == Lifetime::Immediate) on_eliminated_(order); else rest(order); }

    // The price an auction would cross at now, inside the band [floor, ceiling]. Of every price an
    // order is limited to - one beyond the band at the limit it is willing at, a buy above the
    // ceiling at the ceiling and a sell below the floor at the floor; a market order has no price
    // of its own:
    //   the most quantity executable,
    //   then the least surplus left on the heavier side,
    //   then the market's pressure: the higher price when buyers are left over, the lower when
    //   sellers are,
    //   then the nearest the settlement price, which is weighed whether or not an order rests
    //   there,
    //   then the higher price.
    // Checked against a venue's feed rather than assumed. Over one captured trading day, wherever the
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
        if (reference >= floor && reference <= ceiling) take(reference);  // the settlement price, whether or not an order rests there
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
        on_accepted_(order);   // taken, before anything it makes of it
        order.price = price;
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level == price; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level == price; });

        if (order.qty == 0) return;
        if (order.lifetime == Lifetime::Immediate || order.price == market(order.side)) on_eliminated_(order);   // never to rest: what is left goes
        else rest(order);
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
    OnEliminated on_eliminated_;
    OnAccepted   on_accepted_;
    std::uint64_t matches_ = 0;
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> bids_;  // best = highest
    std::map<std::int64_t, std::deque<Order>>                            asks_;  // best = lowest
};}
