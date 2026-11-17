#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <deque>
#include <functional>
#include <limits>
#include <map>
#include <optional>
#include <utility>

namespace matching {

enum class Side : std::uint8_t { Buy, Sell };

// How long an order lives, as the venue's time in force means it.
//   Session   — until the trading session it was entered in ends
//   Canceled  — until it is canceled, through any number of sessions
//   Immediate — what does not trade as it arrives is eliminated
//   Date      — until the session of a date it names ends
//   Whole     — trades in full as it arrives, or not at all
//   Extended  — until the post-market session ends
//   Hours     — until the venue's hours end
//   Timed     — until a time of its own, within the day
//   Opening   — until the opening cross completes
//   Closing   — until the closing cross completes
enum class Lifetime : std::uint8_t { Session, Canceled, Immediate, Date, Whole, Extended, Hours, Timed, Opening, Closing };

// Which auction a trade ended, or the one an order trades in alone. None is continuous trading.
enum class Cross : std::uint8_t { None, Opening, Intraday, Closing };

// The reference price a pegged order follows: none, the midpoint of the best bid and offer, the
// best price on its own side (Near) or the best on the other side (Far).
enum class Peg : std::uint8_t { None, Midpoint, Near, Far };

// How an order is handled beside its type: on the venue alone, routed, an intermarket sweep, one
// under the trade-at rule, or a limit stepping back while the quote is unstable. The venue has no
// away market and no quote of another's, so each trades as an unrouted order does.
enum class Instruction : std::uint8_t { Unrouted, Routed, Sweep, TradeAt, Discretionary };

// What is done of two orders of one owner that would trade with each other, as the arriving one says:
// the arriving order canceled (Newest), the resting one (Oldest), both (Both), the smaller and both
// where they are equal (Smaller), the smaller canceled and the larger lowered by its size
// (Decrement). Remover is said of an order resting: whatever the arriving one says; arriving, it
// prevents nothing.
enum class Prevention : std::uint8_t { None, Newest, Oldest, Both, Smaller };

// What a prevented match took of an order, as its owner is told: the shares that would have traded
// and their price, the order they would have traded with, the venue's code of what was done,
// whether this order rested, and whether the book showed the one that did. No shares: none was
// prevented of it.
struct Prevented {
    std::uint64_t qty      = 0;
    std::int64_t  price    = 0;
    std::uint64_t against  = 0;
    char          strategy = 0;
    bool          resting  = false;
    bool          shown    = false;
};

struct Order {
    std::uint64_t id;
    Side          side;
    std::int64_t  price;   // in ticks
    std::uint64_t qty;
    Lifetime      lifetime = Lifetime::Session;   // how long it lives, as its time in force means
    bool          shown = true;   // does the book show it: a hidden one rests and trades unseen, and what never rested was never shown
    Cross         only = Cross::None;   // the one cross it trades in, or none: it trades as it arrives and rests
    Peg           peg = Peg::None;   // the reference price it follows, where it is pegged: its price is then the engine's
    std::int64_t  limit = 0;   // a pegged order's limit, no further than which it follows - the market's where it has none
    std::int64_t  offset = 0;   // its difference from the price it follows, where it follows a side of the book
    std::int64_t  expires = 0;   // when it expires by a time of its own, in nanoseconds since the epoch: a GTT order's
    Instruction   instruction = Instruction::Unrouted;   // how it is handled beside its type
    std::uint64_t min_qty = 0;   // the fewest shares it trades at a time, where it names any: a Minimum Quantity order
    std::uint64_t group = 0;   // whose it is: two orders of one group, not 0, do not trade with each other
    Prevention    prevention = Prevention::None;   // what is done of two such orders that would trade, as the arriving one says
    Prevented     prevented{};   // what a prevented match took of it, on the cancel or the reduce that says so
};

// The price a market order stands at: it trades at any price, so it is ahead of every limit on
// its side, and it is no price an auction could cross at.
constexpr std::int64_t market(Side side) {
    return side == Side::Buy ? std::numeric_limits<std::int64_t>::max() : std::numeric_limits<std::int64_t>::min();
}

struct Trade {
    std::uint64_t aggressor_id;
    std::uint64_t resting_id;
    std::int64_t  price;    // resting (passive) price
    std::uint64_t qty;
    std::uint64_t match_id; // numbers the fill, shared by both sides of it
    // The auction this trade ended. An uncross has no aggressor: both sides waited for it, so
    // aggressor_id is the buy side and resting_id the sell side, and price is the one it crossed at.
    Cross         cross = Cross::None;
    bool          resting_shown   = true;    // did the book show the resting side
    bool          aggressor_shown = false;   // and the aggressor: only one that rested was, as both sides of an uncross did
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

// The replace callback of an engine whose owner does not listen for replaces.
struct IgnoreReplace {
    void operator()(std::uint64_t, const Order&) const {}
};

// Continuous price-time-priority matching engine for one instrument.
//   on_trade(Trade)             — a fill was generated (aggressor vs resting)
//   on_rest(const Order&)       — residual quantity now rests in the book
//   on_canceled(const Order&)   — a resting order was removed by a cancel
//   on_eliminated(const Order&) — what was left of an order never to rest was taken out
//   on_accepted(const Order&)   — the book took an order, as it now stands, before anything it makes of it
//   on_replaced(std::uint64_t, const Order&) — an order replaced: its old id, and what of it now rests
// Pure logic: no I/O, no threading — the owner drives it and forwards the callbacks
// to the order gateway and the market-data feeds.
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreCancel, typename OnEliminated = IgnoreCancel, typename OnAccepted = IgnoreCancel, typename OnReplaced = IgnoreReplace>
class MatchingEngine {
public:
    MatchingEngine(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(), on_eliminated_(), on_accepted_(), on_replaced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(), on_accepted_(), on_replaced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(), on_replaced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated, OnAccepted on_accepted)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(on_accepted), on_replaced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated, OnAccepted on_accepted, OnReplaced on_replaced)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(on_accepted), on_replaced_(on_replaced) {}


    // Would an order trade in full as it arrives: does that much rest on the other side, at its
    // price or better?
    bool whole(const Order& order) const {
        std::uint64_t there = 0;
        if (order.side == Side::Buy) {
            for (const auto& [level, orders] : asks_) { if (level > order.price) break; for (const auto& o : orders) if (!prevents(order, o)) there += o.qty; }
        } else {
            for (const auto& [level, orders] : bids_) { if (level < order.price) break; for (const auto& o : orders) if (!prevents(order, o)) there += o.qty; }
        }
        return there >= order.qty;
    }

    void on_new_order(Order order) {
        if (replacing_ == 0) on_accepted_(order);   // taken, before anything it makes of it - a replacement is said as the replace
        repeg();   // the book's pegged orders follow the midpoint as it stands before this one
        if (order.peg != Peg::None) {
            order.shown = false;   // a pegged order is never shown
            const auto price = pegged(order, quote());
            if (!price) {   // no midpoint to follow: it waits out of the book, or goes, where it is never to rest
                if (order.lifetime == Lifetime::Immediate || order.lifetime == Lifetime::Whole) { on_eliminated_(order); return; }
                parked_.push_back(order);
                pegged_.push_back(order.id);
                return;
            }
            order.price = *price;
        }
        if (order.lifetime == Lifetime::Whole && !whole(order)) { order.shown = false; on_eliminated_(order); return; }   // in full or not at all
        if (order.min_qty > 0) {
            if (order.shown) order.lifetime = Lifetime::Immediate;   // one marked to be displayed is taken as IOC (Rule 4703(e))
            order.shown = false;   // and none is ever displayed
            const bool met = order.side == Side::Buy
                ? available(order, asks_, [&](std::int64_t level) { return level <= order.price; }) >= order.min_qty
                : available(order, bids_, [&](std::int64_t level) { return level >= order.price; }) >= order.min_qty;
            if (!met) {
                if (order.lifetime == Lifetime::Immediate) { on_eliminated_(order); return; }
                order.price = passive(order);
                rest(order);
                return;
            }
        }
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level <= order.price; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level >= order.price; });

        if (order.qty == 0) return;
        if (order.lifetime == Lifetime::Immediate || order.lifetime == Lifetime::Whole || order.price == market(order.side)) {
            order.shown = false;   // never to rest, so never shown: what is left goes
            on_eliminated_(order);
        }
        else rest(order);
    }

    // Remove a resting order by id, telling the owner what left the book. Returns true if it was found.
    // Every order the engine holds, one at a time, and where it holds it: 'B' on the book, bids from
    // the best down and then asks from the best up, each price in time order; 'H' held apart for
    // an auction; 'P' a pegged order with nothing to follow, out of the book;
    // 'Q' a pegged order queued until the open; 'W' waiting for the one cross it is for.
    template <typename Visit>
    void each(Visit visit) const {
        for (const auto& [level, orders] : bids_) for (const Order& o : orders) visit(o, 'B');
        for (const auto& [level, orders] : asks_) for (const Order& o : orders) visit(o, 'B');
        for (const auto& [level, orders] : held_bids_) for (const Order& o : orders) visit(o, 'H');
        for (const auto& [level, orders] : held_asks_) for (const Order& o : orders) visit(o, 'H');
        for (const Order& o : parked_) visit(o, 'P');
        for (const Order& o : queued_pegs_) visit(o, 'Q');
        for (const Order& o : waiting_) visit(o, 'W');
    }


    // How much would trade with an arriving order at its price or better: every order there it is
    // not too small for.
    template <typename Book, typename Crosses>
    std::uint64_t available(const Order& in, const Book& opposite, Crosses crosses) const {
        std::uint64_t there = 0;
        for (const auto& [level, orders] : opposite) {
            if (!crosses(level)) break;
            for (const auto& o : orders) if (o.min_qty <= in.qty && !prevents(in, o)) there += o.qty;
        }
        return there;
    }

    // An order whose minimum is not met as it arrives posts unseen at its price - or, where that
    // would cross the book, a tick short of the best price on the other side: it may not rest
    // crossing it. Nasdaq reprices so where each order must meet the minimum (Rule 4703(e)); one
    // tick of the engine's price is the simulator's increment, not Nasdaq's.
    std::int64_t passive(const Order& o) const {
        if (o.side == Side::Buy) return asks_.empty() || asks_.begin()->first > o.price ? o.price : asks_.begin()->first - 1;
        return bids_.empty() || bids_.begin()->first < o.price ? o.price : bids_.begin()->first + 1;
    }

    // A Minimum Quantity order takes no part in a cross, nor in its price (Rule 4703(e)): set aside
    // while the cross is made.
    std::deque<Order> set_aside() {
        std::deque<Order> aside;
        const auto take = [&](auto& book) {
            for (auto level = book.begin(); level != book.end(); ) {
                auto& queue = level->second;
                for (auto at = queue.begin(); at != queue.end(); ) {
                    if (at->min_qty == 0) { ++at; continue; }
                    aside.push_back(*at);
                    at = queue.erase(at);
                }
                if (queue.empty()) level = book.erase(level);
                else ++level;
            }
        };
        take(bids_);
        take(asks_);
        return aside;
    }

    // And rejoins the book once it is done, behind what rests at its price: it loses its place in
    // time there, which Nasdaq's book would not.
    void rejoin(const std::deque<Order>& aside) {
        for (const Order& o : aside) {
            if (o.side == Side::Buy) place(bids_[o.price], o);
            else                     place(asks_[o.price], o);
        }
    }

    // Are an arriving order and a resting one kept from trading with each other: of one group, the
    // resting one kept from any and the arriving one saying what is done of them?
    bool prevents(const Order& in, const Order& resting) const {
        if (in.group == 0 || in.group != resting.group) return false;
        if (resting.prevention == Prevention::None || in.prevention == Prevention::None) return false;
        return true;
    }

    // What a prevented match takes of an order, told its owner: all that is left of it, as a cancel. An arriving order never rested, so the book never showed it.
    void prevented(Order order, std::uint64_t taken, std::uint64_t would, std::int64_t price, const Order& remover, std::uint64_t against, bool resting, bool shown) {
        order.prevented = Prevented{ would, price, against, 0, resting, shown };
        if (!resting) order.shown = false;
        (void)taken; (void)remover;
        on_canceled_(order);
    }
    // A cancel of an order on the book that trades on, not of one an auction collects apart: what
    // the auction holds is locked in. Returns false, and nothing changes, for one it holds.
    bool on_cancel_trading(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed) && !waited(id, removed)) return false;
        on_canceled_(removed);
        return true;
    }

    // A replace of an order on the book that trades on, raised as one; one an auction holds apart is
    // locked in, and refused. Returns false, and nothing changes, for one it holds.
    bool take_replaced_trading(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed)) return false;
        replacing_ = id;
        expires_replaced_ = removed.expires;
        return true;
    }    bool on_cancel(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !erase(held_bids_, id, removed) && !erase(held_asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed) && !waited(id, removed)) return false;
        on_canceled_(removed);
        return true;
    }

    // A replace, raised as one event: the order replaced, as what of it now rests. Its old order is
    // taken out without a word, and the replacement given as the phase it arrives in gives an
    // order; where it rests, that rest is the replace. Returns false, and nothing changes, when no
    // such order rests.
    bool take_replaced(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !erase(held_bids_, id, removed) && !erase(held_asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed)) return false;
        replacing_ = id;
        expires_replaced_ = removed.expires;
        return true;
    }

    // When the order a replace took out expires: its replacement keeps it, as IEX's ExpireTime is
    // the original's on a replace (IEX FIX Specification v2.91, p. 78).
    std::int64_t expires_replaced() const { return expires_replaced_; }

    // A replacement that rested nothing - it traded away as it arrived, or was never to rest - is the
    // order replaced with nothing left of it.
    void replaced(Order order) {
        if (replacing_ == 0) return;
        order.qty = 0;
        const std::uint64_t id = replacing_;
        replacing_ = 0;
        on_replaced_(id, order);
    }

    // The venue's time come: every order that expires by a time of its own and whose time has come
    // by now leaves the book, eliminated as one never to rest is, its owner told of each.
    void lapse(std::int64_t now) {
        lapsed(bids_, now);
        lapsed(asks_, now);
        lapsed(held_bids_, now);
        lapsed(held_asks_, now);
        for (auto at = parked_.begin(); at != parked_.end(); ) {   // pegged, waiting for a midpoint
            if (at->lifetime != Lifetime::Timed || at->expires > now) { ++at; continue; }
            const Order gone = *at;
            at = parked_.erase(at);
            on_eliminated_(gone);
        }
    }

    template <typename Book>
    void lapsed(Book& book, std::int64_t now) {
        for (auto level = book.begin(); level != book.end(); ) {
            auto& queue = level->second;
            for (auto at = queue.begin(); at != queue.end(); ) {
                if (at->lifetime != Lifetime::Timed || at->expires > now) { ++at; continue; }
                const Order gone = *at;
                at = queue.erase(at);
                on_eliminated_(gone);
            }
            if (queue.empty()) level = book.erase(level);
            else ++level;
        }
    }

    // The session's end for what lives for it: every resting order of this lifetime leaves the
    // book, eliminated as one never to rest is, its owner told of each.
    void expire(Lifetime lifetime) {
        expired(bids_, lifetime);
        expired(asks_, lifetime);
        expired(held_bids_, lifetime);
        expired(held_asks_, lifetime);
        for (auto at = waiting_.begin(); at != waiting_.end(); ) {   // waiting for a cross that never came
            if (at->lifetime != lifetime) { ++at; continue; }
            const Order gone = *at;
            at = waiting_.erase(at);
            on_eliminated_(gone);
        }
        for (auto at = parked_.begin(); at != parked_.end(); ) {   // pegged, waiting for a midpoint
            if (at->lifetime != lifetime) { ++at; continue; }
            const Order gone = *at;
            at = parked_.erase(at);
            on_eliminated_(gone);
        }
        for (auto at = queued_pegs_.begin(); at != queued_pegs_.end(); ) {   // pegged, queued until the open
            if (at->lifetime != lifetime) { ++at; continue; }
            const Order gone = *at;
            at = queued_pegs_.erase(at);
            on_eliminated_(gone);
        }
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

    // The end of the hours a pegged order follows the midpoint in: every pegged order leaves the
    // book, whatever its lifetime, eliminated as one never to rest is, its owner told of each.
    void expire_pegged() {
        unpegged(bids_);
        unpegged(asks_);
        unpegged(held_bids_);
        unpegged(held_asks_);
        for (const Order& gone : parked_) on_eliminated_(gone);   // pegged, waiting for a midpoint
        parked_.clear();
        for (const Order& gone : queued_pegs_) on_eliminated_(gone);   // pegged, queued until the open
        queued_pegs_.clear();
        pegged_.clear();
    }

    template <typename Book>
    void unpegged(Book& book) {
        for (auto level = book.begin(); level != book.end(); ) {
            auto& queue = level->second;
            for (auto at = queue.begin(); at != queue.end(); ) {
                if (at->peg == Peg::None) { ++at; continue; }
                const Order gone = *at;
                at = queue.erase(at);
                on_eliminated_(gone);
            }
            if (queue.empty()) level = book.erase(level);
            else ++level;
        }
    }

    // The end of the hours an order of this instruction trades in: every one leaves the book,
    // whatever its lifetime, eliminated as one never to rest is, its owner told of each.
    void expire_instructed(Instruction instruction) {
        instructed(bids_, instruction);
        instructed(asks_, instruction);
        instructed(held_bids_, instruction);
        instructed(held_asks_, instruction);
        uninstructed(waiting_, instruction);   // waiting for its cross
        uninstructed(parked_, instruction);   // pegged, waiting for a midpoint
        uninstructed(queued_pegs_, instruction);   // pegged, queued until the open
    }

    template <typename Book>
    void instructed(Book& book, Instruction instruction) {
        for (auto level = book.begin(); level != book.end(); ) {
            uninstructed(level->second, instruction);
            if (level->second.empty()) level = book.erase(level);
            else ++level;
        }
    }

    void uninstructed(std::deque<Order>& queue, Instruction instruction) {
        for (auto at = queue.begin(); at != queue.end(); ) {
            if (at->instruction != instruction) { ++at; continue; }
            const Order gone = *at;
            at = queue.erase(at);
            on_eliminated_(gone);
        }
    }
    // What an auction left of its market orders leaves the book as the auction ends: a market
    // order has no price of its own to rest at, so each is eliminated, its owner told.
    void eliminate_market() {
        unpriced(bids_, market(Side::Buy));
        unpriced(asks_, market(Side::Sell));
    }

    template <typename Book>
    void unpriced(Book& book, std::int64_t price) {
        const auto level = book.find(price);
        if (level == book.end()) {
            return;
        }
        const std::deque<Order> gone = std::move(level->second);
        book.erase(level);
        for (const Order& order : gone) {
            on_eliminated_(order);
        }
    }

    // An order for one cross alone, before that cross collects: taken, and held aside, where nothing
    // trades with it and the book does not show it.
    void wait(Order order) {
        on_accepted_(order);
        waiting_.push_back(order);
    }

    // A cross collects: every order waiting for it joins the book, uncrossed, in the order it came.
    void join(Cross cross) {
        for (auto at = waiting_.begin(); at != waiting_.end(); ) {
            if (at->only != cross) { ++at; continue; }
            keep(*at);
            at = waiting_.erase(at);
        }
    }

    // A cross completes: what is left of each order for it alone is eliminated - OUCH 5.0's "any
    // unexecuted portion of this order will automatically be canceled immediately after the cross
    // completes". The opening and the closing cross are done for the day; a halt's may come again.
    void ended(Cross cross) {
        if (cross != Cross::Intraday) done_[static_cast<std::size_t>(cross)] = true;
        alone(bids_, cross);
        alone(asks_, cross);
    }

    // Is a cross done for the day, so an order for it alone has nothing left to trade in?
    bool done(Cross cross) const { return done_[static_cast<std::size_t>(cross)]; }

    template <typename Book>
    void alone(Book& book, Cross cross) {
        for (auto level = book.begin(); level != book.end(); ) {
            auto& queue = level->second;
            for (auto at = queue.begin(); at != queue.end(); ) {
                if (at->only != cross) { ++at; continue; }
                const Order gone = *at;
                at = queue.erase(at);
                on_eliminated_(gone);
            }
            if (queue.empty()) level = book.erase(level);
            else ++level;
        }
    }

    // Take an order out of those waiting aside. Returns true if it was there.
    bool waited(std::uint64_t id, Order& removed) {
        for (auto at = waiting_.begin(); at != waiting_.end(); ++at) {
            if (at->id != id) continue;
            removed = *at;
            waiting_.erase(at);
            return true;
        }
        return false;
    }

    // An order held without crossing: an auction collects, and nothing trades until it uncrosses.
    // The book may stand crossed while it does.
    // An order never to rest has nothing it could trade with as it arrives, so it is eliminated - the
    // simulator's reading of a time in force no venue document here states for an auction.
    void hold(Order order) { if (replacing_ == 0) on_accepted_(order); if (order.peg != Peg::None) { queue_peg(order); return; } if (order.lifetime == Lifetime::Immediate || order.lifetime == Lifetime::Whole) { order.shown = false; on_eliminated_(order); } else keep(order); }

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
            for (const auto* book : { &bids_, &held_bids_ }) for (const auto& [level, orders] : *book) { if (level < price) break; for (const auto& o : orders) bought += (o.min_qty > 0 ? 0 : o.qty); }
            for (const auto* book : { &asks_, &held_asks_ }) for (const auto& [level, orders] : *book) { if (level > price) break; for (const auto& o : orders) sold += (o.min_qty > 0 ? 0 : o.qty); }
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
        for (const auto* side : { &bids_, &held_bids_ }) for (const auto& level : *side) {
            if (level.first != market(Side::Buy) && level.first >= floor) take(std::min(level.first, ceiling));
        }
        for (const auto* side : { &asks_, &held_asks_ }) for (const auto& level : *side) {
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
        release();   // the auction consolidates what it collected with the book
        std::deque<Order> aside = set_aside();   // a Minimum Quantity order takes no part in a cross
        const auto at = indicative(reference, floor, ceiling);
        std::uint64_t left = at.qty;

        while (left > 0 && !bids_.empty() && !asks_.empty()) {
            auto bid = bids_.begin();
            auto ask = asks_.begin();
            if (bid->first < at.price || ask->first > at.price) break;

            Order& buy  = bid->second.front();
            Order& sell = ask->second.front();
            const std::uint64_t fill = std::min({ buy.qty, sell.qty, left });

            Trade trade{ buy.id, sell.id, at.price, fill, ++matches_, cross };
            trade.aggressor_shown = buy.shown;   // both sides rested while it collected
            trade.resting_shown   = sell.shown;
            on_trade_(trade);
            buy.qty  -= fill;
            sell.qty -= fill;
            left     -= fill;

            if (buy.qty == 0)  bid->second.pop_front();
            if (sell.qty == 0) ask->second.pop_front();
            if (bid->second.empty()) bids_.erase(bid);
            if (ask->second.empty()) asks_.erase(ask);
        }
        if (cross == Cross::Opening) open_pegs();   // the book trades on: what was queued until the open follows the midpoint
        rejoin(aside);   // and rejoins the book once it is done
        return at;
    }

    // An order that may trade only at one price - trade at last, at the closing auction's: it
    // crosses what rests at exactly that price, and the rest of it rests there.
    void on_new_order_at(Order order, std::int64_t price) {
        if (replacing_ == 0) on_accepted_(order);   // taken, before anything it makes of it - a replacement is said as the replace
        order.price = price;
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level == price; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level == price; });

        if (order.qty == 0) return;
        if (order.lifetime == Lifetime::Immediate || order.lifetime == Lifetime::Whole || order.price == market(order.side)) {
            order.shown = false;   // never to rest, so never shown: what is left goes
            on_eliminated_(order);
        }
        else rest(order);
    }

private:
    // Best opposite level first (map ordering); FIFO within a level. Generates a Trade per fill
    // until the incoming order is exhausted or the book no longer crosses. A resting Minimum
    // Quantity order trades only with an order of at least its minimum (Nasdaq Equity 4, Rule
    // 4703(e)): the arriving one passes over it, and over a level that holds only such orders.
    template <typename Book, typename Crosses>
    void cross(Order& in, Book& opposite, Crosses crosses) {
        const std::uint64_t size = in.qty;
        for (auto best = opposite.begin(); in.qty > 0 && best != opposite.end(); ) {
            if (!crosses(best->first)) break;

            auto& queue = best->second;
            for (auto at = queue.begin(); in.qty > 0 && at != queue.end(); ) {
                Order& resting = *at;
                if (resting.min_qty > size) { ++at; continue; }   // too small for its minimum
                if (prevents(in, resting)) {   // one owner's: they do not trade
                    const std::uint64_t would = std::min(in.qty, resting.qty);
                    std::uint64_t off = 0, away = 0;   // the shares the arriving order gives up, and the resting one
                    switch (in.prevention) {
                        case Prevention::Newest:    off = in.qty; break;
                        case Prevention::Oldest:    away = resting.qty; break;
                        case Prevention::Both:      off = in.qty; away = resting.qty; break;
                        case Prevention::Smaller:   off = in.qty <= resting.qty ? in.qty : 0; away = resting.qty <= in.qty ? resting.qty : 0; break;
                        default: break;
                    }
                    if (away > 0) prevented(resting, away, would, best->first, in, in.id, true, resting.shown);
                    if (off > 0)  prevented(in, off, would, best->first, in, resting.id, false, resting.shown);
                    in.qty      -= off;
                    resting.qty -= away;
                    if (resting.qty == 0) { at = queue.erase(at); continue; }
                    resting.min_qty = std::min(resting.min_qty, resting.qty);
                    ++at;
                    continue;
                }
                std::uint64_t fill = std::min(in.qty, resting.qty);
                Trade trade{ in.id, resting.id, best->first, fill, ++matches_ };
                trade.resting_shown = resting.shown;   // the aggressor was never shown: it had not rested
                on_trade_(trade);
                in.qty -= fill;
                resting.qty -= fill;
                resting.min_qty = std::min(resting.min_qty, resting.qty);   // fewer left than its minimum: what is left is its minimum
                if (resting.qty == 0) at = queue.erase(at);
                else ++at;
            }
            if (queue.empty()) best = opposite.erase(best);
            else ++best;
        }
    }

    void rest(const Order& o) {
        if (o.side == Side::Buy) place(bids_[o.price], o);
        else                     place(asks_[o.price], o);
        if (o.peg != Peg::None) pegged_.push_back(o.id);   // it follows the midpoint from now on
        if (replacing_ == 0) { on_rest_(o); return; }
        const std::uint64_t id = replacing_;
        replacing_ = 0;
        on_replaced_(id, o);
    }

    // An order an auction collects while the book trades on: kept apart, where nothing trades with
    // it, until the auction matches. Its owner is told it rests, as of any order held.
    void keep(const Order& o) {
        if (o.side == Side::Buy) place(held_bids_[o.price], o);
        else                     place(held_asks_[o.price], o);
        if (replacing_ == 0) { on_rest_(o); return; }
        const std::uint64_t id = replacing_;
        replacing_ = 0;
        on_replaced_(id, o);
    }

    // The auction matches: what it collected joins the book, each order behind those resting at its
    // price. IEX releases queued orders to the book in time priority (IEX FIX Specification v2.91,
    // p. 3), and an order here carries no time to rank it by against one that rested as it arrived:
    // the simulator's reading.
    void release() {
        for (const auto& [level, orders] : held_bids_) for (const Order& o : orders) place(bids_[o.price], o);
        for (const auto& [level, orders] : held_asks_) for (const Order& o : orders) place(asks_[o.price], o);
        held_bids_.clear();
        held_asks_.clear();
    }

    // The best bid and offer the book shows, each none where its side shows nothing: the book's own,
    // standing in for the NBBO a pegged order follows, of which the simulator has no source.
    struct Quote {
        std::optional<std::int64_t> bid;
        std::optional<std::int64_t> ask;
    };

    Quote quote() const {
        const auto best = [](const auto& book) -> std::optional<std::int64_t> {
            for (const auto& [level, orders] : book) {
                if (level == market(Side::Buy) || level == market(Side::Sell)) continue;   // a market order shows no price
                for (const Order& o : orders) if (o.shown) return level;
            }
            return std::nullopt;
        };
        return { best(bids_), best(asks_) };
    }

    // Where a pegged order stands, no further than its limit; none where the book shows nothing for
    // it to follow. At the midpoint: one between two prices is taken at the price away from the other
    // side - a buy's below, a sell's above - as the book's prices are whole. By the best price on its
    // own side (Near) or on the other (Far), and its difference from it: never beyond the other
    // side's best - a buy no higher than the best offer, a sell no lower than the best bid - which is
    // IEX's Offset Peg "booked at the contra-side protected quote" and its Market Peg's offset
    // "disregarded" where either would book more aggressively (IEX FIX Specification v2.91, pp. 36, 39).
    static std::optional<std::int64_t> pegged(const Order& o, const Quote& shown) {
        const bool buy = o.side == Side::Buy;
        const auto& own = buy ? shown.bid : shown.ask;
        const auto& other = buy ? shown.ask : shown.bid;
        const auto limited = [&](std::int64_t price) {
            if (other) price = buy ? std::min(price, *other) : std::max(price, *other);
            return buy ? std::min(price, o.limit) : std::max(price, o.limit);
        };
        switch (o.peg) {
            case Peg::Midpoint:
                if (!own || !other) return std::nullopt;
                return buy ? std::min((*own + *other) / 2, o.limit) : std::max((*own + *other + 1) / 2, o.limit);
            case Peg::Near:
                if (!own) return std::nullopt;
                return limited(*own + o.offset);
            case Peg::Far:
                if (!other) return std::nullopt;
                return limited(*other + o.offset);
            default:
                return std::nullopt;
        }
    }

    // The pegged orders follow the midpoint, each in the order it was posted. One the midpoint moves
    // leaves the book and posts again at its new price, behind what rests there - IEX gives a repriced
    // order new time priority (Rule 11.190, Supplementary Material .01) - trading first with what it
    // now reaches, as an arriving order does. One with no midpoint to follow waits out of the book.
    void repeg() {
        const auto shown = quote();
        for (auto at = pegged_.begin(); at != pegged_.end(); ) {
            Order o{};
            if (!unparked(*at, o)) {
                const Order* resting = found(bids_, *at);
                if (resting == nullptr) resting = found(asks_, *at);
                if (resting == nullptr) { at = pegged_.erase(at); continue; }   // traded away, canceled or expired
                if (const auto price = pegged(*resting, shown); price && *price == resting->price) { ++at; continue; }
                if (!erase(bids_, *at, o)) erase(asks_, *at, o);
            }
            const auto price = pegged(o, shown);
            if (!price) { parked_.push_back(o); ++at; continue; }
            o.price = *price;
            if (o.side == Side::Buy) cross(o, asks_, [&](std::int64_t level) { return level <= o.price; });
            else                     cross(o, bids_, [&](std::int64_t level) { return level >= o.price; });
            if (o.qty == 0) { at = pegged_.erase(at); continue; }
            if (o.side == Side::Buy) place(bids_[o.price], o);
            else                     place(asks_[o.price], o);
            ++at;
        }
    }

    // Take a pegged order out of those waiting for a midpoint. Returns true if it was there.
    bool unparked(std::uint64_t id, Order& removed) {
        for (auto at = parked_.begin(); at != parked_.end(); ++at) {
            if (at->id != id) continue;
            removed = *at;
            parked_.erase(at);
            return true;
        }
        return false;
    }

    template <typename Book>
    static const Order* found(const Book& book, std::uint64_t id) {
        for (const auto& [level, orders] : book)
            for (const Order& o : orders)
                if (o.id == id) return &o;
        return nullptr;
    }

    // A pegged order queued before the book trades on: kept out of the auction, unseen and unpriced,
    // until the opening uncross; then it follows the midpoint as one arriving would.
    void queue_peg(Order o) {
        o.shown = false;
        queued_pegs_.push_back(o);
        if (replacing_ == 0) return;
        const std::uint64_t id = replacing_;
        replacing_ = 0;
        on_replaced_(id, o);
    }

    // The opening uncross done: each pegged order queued until the open follows the midpoint, in the
    // order it came, trading first with what it reaches.
    void open_pegs() {
        for (const Order& o : queued_pegs_) {
            parked_.push_back(o);
            pegged_.push_back(o.id);
        }
        queued_pegs_.clear();
        repeg();
    }

    // Take a pegged order out of those queued until the open. Returns true if it was there.
    bool unqueued(std::uint64_t id, Order& removed) {
        for (auto at = queued_pegs_.begin(); at != queued_pegs_.end(); ++at) {
            if (at->id != id) continue;
            removed = *at;
            queued_pegs_.erase(at);
            return true;
        }
        return false;
    }

    // Where an order joins its price: by display, then time. "Equally priced Orders with a Display
    // Attribute will be ranked in time priority", ahead of those without, which rank in time among
    // themselves (Nasdaq Rule 4757(a)(1), its price/display/time algorithm). A shown order goes in
    // after the last shown one at its price, ahead of every hidden one; a hidden one goes at the back.
    template <typename Queue>
    static void place(Queue& queue, const Order& o) {
        if (!o.shown) { queue.push_back(o); return; }
        queue.insert(std::find_if(queue.begin(), queue.end(), [](const Order& resting) { return !resting.shown; }), o);
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
    OnReplaced   on_replaced_;
    std::uint64_t replacing_ = 0;   // the order a replace takes the place of, while its replacement is given; 0 when none is
    std::deque<Order> waiting_;               // orders for one cross alone, until it collects
    std::array<bool, 4> done_{};              // the crosses done for the day, by Cross
    std::uint64_t matches_ = 0;
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> bids_;  // best = highest
    std::map<std::int64_t, std::deque<Order>>                            asks_;  // best = lowest
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> held_bids_;  // what an auction collects, apart from the book that trades on
    std::map<std::int64_t, std::deque<Order>>                            held_asks_;
    std::deque<std::uint64_t> pegged_;   // the pegged orders, in the order they were posted, each following the midpoint
    std::deque<Order>         parked_;   // pegged orders with no midpoint to follow, waiting out of the book
    std::deque<Order>         queued_pegs_;   // pegged orders queued until the open, out of its auction
    std::int64_t  expires_replaced_ = 0;   // when the order a replace took out expires, which its replacement keeps
};}
