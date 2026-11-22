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
#include <vector>

namespace matching {

enum class Side : std::uint8_t { Buy, Sell };

// How long an order lives, as the venue's time in force means it.
//   Session   — until the trading session it was entered in ends
//   Canceled  — until it is canceled, through any number of sessions
//   Immediate — what does not trade as it arrives is eliminated
//   Date      — until the session of a date it names ends
//   Extended  — until the post-market session ends
//   Timed     — until a time of its own, within the day
enum class Lifetime : std::uint8_t { Session, Canceled, Immediate, Date, Extended, Timed };

// Which auction a trade ended, or the one an order trades in alone. None is continuous trading;
// Extended the Extended Trading Close, after the Closing Cross at its price;
// ExtendedLife the Midpoint Extended Life book, matched apart at the midpoint, in no auction.
enum class Cross : std::uint8_t { None, Opening, Intraday, Closing, Extended, ExtendedLife };

// The reference price a pegged order follows: none, the midpoint of the best bid and offer, the
// best price on its own side (Near) or the best on the other side (Far).
enum class Peg : std::uint8_t { None, Midpoint, Far, Near };

// What is done of two orders of one owner that would trade with each other, as the arriving one says:
// the arriving order canceled (Newest), the resting one (Oldest), both (Both), the smaller and both
// where they are equal (Smaller), the smaller canceled and the larger lowered by its size
// (Decrement). Remover is said of an order resting: whatever the arriving one says; arriving, it
// prevents nothing.
enum class Prevention : std::uint8_t { None, Decrement, Oldest, Newest, Remover };

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

// Why the engine eliminated an order, where the day says: Unstated is as its time in force or
// its cross says, and each other is the reason a change of phase gave.
enum class Elimination : std::uint8_t { Unstated, Halt, Protection };

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
    std::uint64_t min_qty = 0;   // the fewest shares it trades at a time, where it names any: a Minimum Quantity order
    bool          post_only = false;   // does it only post: it trades as it arrives only where that betters its limit by an increment
    bool          trade_now = false;   // resting unseen and locked by an order that posts, does it take it: Nasdaq's Trade Now
    bool          short_sale = false;   // is it a short sale, held to the short sale price test while its security is under one
    std::uint64_t floor = 0;   // the size it shows at a time, where it keeps the rest behind it: a reserve order's
    bool          attributed = false;   // is it shown as its owner's: an attributable order, published with the identifier below
    std::array<char, 4> attribution{};   // whose it is shown as, padded as its owner sent it
    std::uint64_t group = 0;   // whose it is: two orders of one group, not 0, do not trade with each other
    std::uint8_t  level = 0;   // the level it is kept at, 0 for every level: two are kept apart at one level
    Prevention    prevention = Prevention::None;   // what is done of two such orders that would trade, as the arriving one says
    char          strategy = 0;   // the venue's own code of that
    Prevented     prevented{};   // what a prevented match took of it, on the cancel or the reduce that says so
    std::uint64_t display = 0;   // what of it the book shows now, where it shows a floor
    std::uint64_t renamed = 0;   // the id it stood under before what it shows was refreshed, as it is told resting again; 0 otherwise
    bool          withdraws = false;   // a post-only order of a port that has one canceled rather than repriced
    bool          rejects_aggressive = false;   // a late order for a cross of a port that has one priced beyond its cross's reference prices rejected rather than repriced
    std::uint32_t protection_percent = 0;   // how far through the best opposite price its port lets it be priced, in hundredths of a percent of it; 0 for none
    std::int64_t  protection_amount = 0;   // and as an amount; 0 for none, the tighter of the two applying where both are given
    bool          repriced = false;   // told resting again because the engine repriced it, under a new id
    std::int64_t  discretion = 0;   // its discretionary price, the far end of the range it trades within unseen; 0 for none
    std::uint64_t random = 0;   // the range its displayed size is drawn from about its floor, where it has one: a Random Reserve
    bool          imbalance_only = false;   // for its cross, it trades only against the cross's own orders: an OIO or IO Order
    bool          extends = false;   // what the Closing Cross leaves of it, at or within its limit, joins the Extended Trading Close: an ETC Eligible LOC Order
    std::int64_t  life_due = 0;   // a Midpoint Extended Life order's: when its holding period ends, by the day's clock; 0 while it has not begun
    bool          life_continuous = false;   // and does it trade also with the continuous book's Trade Now midpoint pegs: a M-ELO+CB
    std::int64_t  at_midpoint = 0;   // a Trade Now midpoint peg's: since when it has rested at the midpoint, by the day's clock; 0 while it is not there
    std::uint64_t entered = 0;   // when it took its place at its price, in the engine's own count: what ranks it in time there
    Elimination   eliminated = Elimination::Unstated;   // why the engine eliminated it, where the day gave a reason
};

// How much of a resting order the book shows: its floor's worth where it shows one, all of it otherwise.
inline std::uint64_t showing(const Order& o) {
    return o.floor > 0 ? std::min(o.display, o.qty) : o.qty;
}

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
    bool          resting_midpoint = false;   // did the resting side follow the midpoint
    bool          offset = false;   // is the resting side an imbalance-only order, offsetting the aggressor - the cross's own
};

// What an auction would cross at now: the price, how much would trade there, what would be left
// over on the heavier side, and whether that side is the buyers'. Nothing crosses when qty is zero,
// and neither side is heavier when surplus is.
struct Indicative {
    std::int64_t  price   = 0;
    std::uint64_t qty     = 0;
    std::uint64_t surplus = 0;
    bool          buyers  = false;
    std::int64_t  near_price = 0;       // the Near price, over the cross's own orders and the book trading on; 0 for none
    std::int64_t  far_price  = 0;       // the Far price, over the cross's own orders alone; 0 for none
    bool          early      = false;   // asked for early, so stating neither
};

// The prices a book steps by: each range goes by its size from its start on, the range a price is
// in being the one starting nearest below it, or at it. A book given none steps by one.
struct Ticks {
    std::vector<std::pair<std::int64_t, std::int64_t>> ranges;   // (start, size)

    // The start and size of the range a price is in.
    std::pair<std::int64_t, std::int64_t> at(std::int64_t price) const {
        std::pair<std::int64_t, std::int64_t> in{ 0, 1 };
        bool found = false;
        for (const auto& range : ranges) {
            if (range.second > 0 && range.first <= price && (!found || range.first >= in.first)) {
                in = range;
                found = true;
            }
        }
        return in;
    }

    // The price midway between two, given their sum, rounded twice: to a price, then to the tick, an
    // even split down: 154.90 and 155.00 cross at 154.90 where the tick is 0.10. Half a price goes up
    // where the two leave nothing over, and down where buyers are over at one and sellers at the
    // other (`both`). Saudi Exchange's Procedures round 1.055 to 1.06 in their Table 4, which is the
    // second case; its feed says otherwise. Of the 32 auctions an average decided on its 2026-06-22,
    // 10-29, 11-03 and 11-05 ITCH days whose whole book the feed showed, four fell on half a price:
    // 15.715 said 15.72 and 22.445 said 22.45 with nothing over, 9.075 said 9.07 and 9.825 said 9.82
    // with both. Two each way: measured, and to be measured again on every day captured.
    std::int64_t midway(std::int64_t sum, bool both) const {
        const std::int64_t middle = sum >= 0 ? (sum + (both ? 0 : 1)) / 2 : sum / 2;
        const auto [start, size] = at(middle);
        const std::int64_t below = middle - ((middle - start) % size + size) % size;
        const std::int64_t above = below + size;
        if (middle - below <= above - middle) {
            return below;
        }
        return above;
    }

    // The price a number of ticks above a price (ticks above zero) or below it (below zero), each
    // tick the size of the range it is taken in: Saudi Exchange's 85.00 five ticks down, where the
    // tick is 0.05, is 84.75.
    std::int64_t step(std::int64_t price, int ticks) const {
        for (; ticks > 0; --ticks) {
            price += at(price).second;
        }
        for (; ticks < 0; ++ticks) {
            price -= at(price - 1).second;
        }
        return price;
    }

    // The lowest tick at or above a price, and the highest at or below it: Saudi Exchange's static
    // limits are rounded inward so, 31.10 less 10% being 27.99 and its lower limit 28.00.
    std::int64_t above(std::int64_t price) const {
        const std::int64_t below_it = below(price);
        return below_it == price ? price : below_it + at(below_it).second;
    }

    std::int64_t below(std::int64_t price) const {
        const auto [start, size] = at(price);
        return price - ((price - start) % size + size) % size;
    }
};

// The cancel callback of an engine whose owner does not listen for cancels.
struct IgnoreCancel {
    void operator()(const Order&) const {}
};

// The replace callback of an engine whose owner does not listen for replaces.
struct IgnoreReplace {
    void operator()(std::uint64_t, const Order&) const {}
};

// The reduce callback of an engine whose owner does not listen for reduces.
struct IgnoreReduce {
    void operator()(const Order&, std::uint64_t) const {}
};

// Continuous price-time-priority matching engine for one instrument.
//   on_trade(Trade)             — a fill was generated (aggressor vs resting)
//   on_rest(const Order&)       — residual quantity now rests in the book
//   on_canceled(const Order&)   — a resting order was removed by a cancel
//   on_eliminated(const Order&) — what was left of an order never to rest was taken out
//   on_accepted(const Order&)   — the book took an order, as it now stands, before anything it makes of it
//   on_replaced(std::uint64_t, const Order&) — an order replaced: its old id, and what of it now rests
//   on_reduced(const Order&, std::uint64_t) — an order lowered where it stands: what of it rests, and the shares taken
// Pure logic: no I/O, no threading — the owner drives it and forwards the callbacks
// to the order gateway and the market-data feeds.
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreCancel, typename OnEliminated = IgnoreCancel, typename OnAccepted = IgnoreCancel, typename OnReplaced = IgnoreReplace, typename OnReduced = IgnoreReduce>
class MatchingEngine {
public:
    MatchingEngine(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(), on_eliminated_(), on_accepted_(), on_replaced_(), on_reduced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(), on_accepted_(), on_replaced_(), on_reduced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(), on_replaced_(), on_reduced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated, OnAccepted on_accepted)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(on_accepted), on_replaced_(), on_reduced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated, OnAccepted on_accepted, OnReplaced on_replaced)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(on_accepted), on_replaced_(on_replaced), on_reduced_() {}

    MatchingEngine(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnEliminated on_eliminated, OnAccepted on_accepted, OnReplaced on_replaced, OnReduced on_reduced)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_eliminated_(on_eliminated), on_accepted_(on_accepted), on_replaced_(on_replaced), on_reduced_(on_reduced) {}


    void on_new_order(Order order) {
        reshort();   // the short sales the test repriced follow the bid as it stands before this one
        if (order.lifetime != Lifetime::Immediate) {   // a short sale under the test is taken at its Permitted Price
            const std::int64_t entered = order.price;
            permit(order);
            if (order.price != entered) shorted_.emplace_back(order.id, entered);   // and follows a declining bid down to its limit
        }
        if (replacing_ == 0) on_accepted_(taken(order));   // taken, before anything it makes of it - a replacement is said as the replace
        if (order.lifetime == Lifetime::Immediate) permit(order);   // an IOC one trades at a Permitted Price and higher, and is not repriced
        repeg();   // the book's pegged orders follow the midpoint as it stands before this one
        if (order.peg != Peg::None) {
            order.shown = false;   // a pegged order is never shown
            const auto price = pegged(order, quote());
            if (!price) {   // no midpoint to follow: it waits out of the book, or goes, where it is never to rest
                if (order.lifetime == Lifetime::Immediate) { on_eliminated_(order); return; }
                parked_.push_back(order);
                pegged_.push_back(order.id);
                return;
            }
            order.price = *price;
        }
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
        std::int64_t reach = order.post_only ? improving(order) : order.price;   // a post-only order trades only where that betters its limit by an increment
        if (order.discretion != 0 && order.lifetime == Lifetime::Immediate) reach = order.discretion;   // an IOC with discretion trades within its range at once (Rule 4703(g))
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level <= reach; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level >= reach; });
        if (order.post_only && order.qty > 0) order.price = posting(order);   // what is left posts, inside what the book shows

        if (order.qty == 0) return;
        if (order.lifetime == Lifetime::Immediate || order.price == market(order.side)) {
            order.shown = false;   // never to rest, so never shown: what is left goes
            on_eliminated_(order);
        }
        else {
            rest(order); discern(order);
            trade_now(order.side);   // what rests unseen across from it with Trade Now, locked by it, takes it
        }
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

    // And rejoins the book once it is done, at the place its stamp gives it: it keeps its place in
    // time there, as Nasdaq's book would.
    void rejoin(const std::deque<Order>& aside) {
        for (const Order& o : aside) {
            if (o.side == Side::Buy) placed(bids_[o.price], o);
            else                     placed(asks_[o.price], o);
        }
    }

    // Are an arriving order and a resting one kept from trading with each other: of one group, the
    // resting one kept from any, at one level or either at every level, and the arriving one saying what is done of them?
    bool prevents(const Order& in, const Order& resting) const {
        if (in.group == 0 || in.group != resting.group) return false;
        if (resting.prevention == Prevention::None || in.prevention == Prevention::None || in.prevention == Prevention::Remover) return false;
        if (in.level != resting.level && in.level != 0 && resting.level != 0) return false;
        return true;
    }

    // What a prevented match takes of an order, told its owner: all that is left of it, as a cancel,
    // or a part, as a reduce. An arriving order never rested, so the book never showed it.
    void prevented(Order order, std::uint64_t taken, std::uint64_t would, std::int64_t price, const Order& remover, std::uint64_t against, bool resting, bool shown) {
        order.prevented = Prevented{ would, price, against, remover.strategy, resting, shown };
        if (!resting) order.shown = false;
        if (taken >= order.qty) { on_canceled_(order); return; }
        order.qty -= taken;
        on_reduced_(order, taken);
    }

    // Would a post-only order arriving be repriced to post - it trades nothing as it arrives, and its
    // limit would lock or cross an order the book shows? A port may have such an order canceled instead
    // (Nasdaq Equity 4, Rule 4702(b)(4): "adjusted or be cancelled back to the Participant, depending on
    // the Participant's choice"). One that never rests is not: an IOC post-only order "will be cancelled
    // rather than posted if the Order cannot execute", as any IOC is.
    bool slides(const Order& o) const { return o.post_only && !(o.lifetime == Lifetime::Immediate) && !trading(o) && posting(o) != o.price; }

    // What a reserve order shows at a time: its floor, or - a Random Reserve's - a number of round lots
    // drawn from its floor less its range up to that plus twice its range less one round lot. A range of
    // none, or of no less than the floor - "any share amount less than the nominal displayed size" - draws
    // nothing: the floor is shown. Drawn from the id it is shown under.
    static std::uint64_t drawn(const Order& o) {
        if (o.random == 0 || o.random >= o.floor) return o.floor;
        constexpr std::uint64_t lot = 100;   // a normal unit of trading
        const std::uint64_t low   = o.floor - o.random;
        const std::uint64_t span  = 2 * o.random > lot ? 2 * o.random - lot : 0;
        const std::uint64_t sizes = span / lot + 1;   // the round lots from the least to the most
        std::uint64_t mixed = o.id * 0x9E3779B97F4A7C15ULL;
        mixed ^= mixed >> 30;
        mixed *= 0xBF58476D1CE4E5B9ULL;
        mixed ^= mixed >> 27;
        mixed *= 0x94D049BB133111EBULL;
        mixed ^= mixed >> 31;
        return low + (mixed % sizes) * lot;
    }

    // Discretion (Nasdaq Equity 4, Rule 4703(g)), as an order rests: if it carries a discretionary price,
    // what rests within its range it takes at once; and so does every order of the other side whose range
    // reaches it - each as the Discretionary IOC the venue generates for it, its best first.
    void discern(const Order& rested) {
        std::vector<std::uint64_t> discretionary;
        if (rested.discretion != 0) discretionary.push_back(rested.id);
        const auto reaching = [&](const auto& book) {
            for (const auto& [level, orders] : book) {
                for (const Order& o : orders) {
                    if (o.discretion == 0) continue;
                    if (o.side == Side::Buy ? o.discretion >= rested.price : o.discretion <= rested.price) discretionary.push_back(o.id);
                }
            }
        };
        if (rested.side == Side::Buy) reaching(asks_);
        else                          reaching(bids_);
        for (const std::uint64_t id : discretionary) within(id);
    }

    // A discretionary order takes what rests within its range: a Discretionary IOC "with a price equal to
    // the highest price for the Order with Discretion to buy (lowest price for the Order with Discretion to
    // sell) within the discretionary price range and a size equal to the applicable size of the available
    // liquidity", its shares out of the order, which keeps its place with what is left.
    void within(std::uint64_t id) {
        const auto find = [&](auto& book) -> Order* {
            for (auto& [level, orders] : book) {
                for (Order& o : orders) {
                    if (o.id == id) return &o;
                }
            }
            return nullptr;
        };
        Order* found = find(bids_);
        if (found == nullptr) found = find(asks_);
        if (found == nullptr || found->discretion == 0 || found->floor > 0) return;
        Order ioc = *found;
        const std::int64_t reach = found->discretion;
        const std::uint64_t had = ioc.qty;
        discerning_ = true;   // its fills are of an order the book may show, which they take shares out of
        if (ioc.side == Side::Buy) cross(ioc, asks_, [&](std::int64_t level) { return level <= reach; });
        else                       cross(ioc, bids_, [&](std::int64_t level) { return level >= reach; });
        discerning_ = false;
        const std::uint64_t taken = had - ioc.qty;
        if (taken == 0) return;
        const auto lower = [&](auto& book) {
            for (auto& [level, orders] : book) {
                for (Order& o : orders) {
                    if (o.id != id) continue;
                    o.qty -= std::min(taken, o.qty);
                    if (o.qty == 0) { Order gone{}; erase(book, id, gone); }   // all of it taken
                    return;
                }
            }
        };
        if (ioc.side == Side::Buy) lower(bids_);
        else                       lower(asks_);
    }

    // The imbalance-only orders for a cross, set aside while it is made: they take no part in its
    // price, nor in the match of its own orders.
    std::deque<Order> set_offsets(Cross cross) {
        std::deque<Order> offsets;
        const auto take = [&](auto& book) {
            for (auto level = book.begin(); level != book.end(); ) {
                auto& queue = level->second;
                for (auto at = queue.begin(); at != queue.end(); ) {
                    if (!at->imbalance_only || at->only != cross) { ++at; continue; }
                    offsets.push_back(*at);
                    at = queue.erase(at);
                }
                if (queue.empty()) level = book.erase(level);
                else ++level;
            }
        };
        take(bids_);
        take(asks_);
        return offsets;
    }

    // The best limit on each side of the book, a market order stating none.
    struct Stood {
        bool         has_bid = false;
        std::int64_t bid     = 0;
        bool         has_ask = false;
        std::int64_t ask     = 0;
    };

    Stood stood() const {
        Stood best{};
        for (const auto& [level, orders] : bids_) { if (level == market(Side::Buy) || orders.empty()) continue; best.has_bid = true; best.bid = level; break; }
        for (const auto& [level, orders] : asks_) { if (level == market(Side::Sell) || orders.empty()) continue; best.has_ask = true; best.ask = level; break; }
        return best;
    }

    // Imbalance-only orders offset what is left of the cross's own orders - those for it alone, an
    // MOC's or a LOC's, an MOO's or a LOO's - at the price it crossed at: an IO Order "may be executed
    // only in the Nasdaq Closing Cross and only against MOC Orders or LOC Orders", an OIO Order only in
    // the Opening Cross and "only against MOO Orders, LOO Orders" (Equity 4, Rule 4702(b)(13), (10)).
    // One to buy (sell) stands at its limit, or at the best bid (offer) of the book where that is lower
    // (higher), and offsets where that reaches the cross's price; first entered, first filled. Each
    // fill is the cross's own order's, as aggressor, against it, as resting, at the cross's price. What
    // is left of them is eliminated, as what is left of any order for the cross is.
    std::uint64_t offset(std::deque<Order>& offsets, Cross cross, std::int64_t price, const Stood& standing) {
        std::uint64_t traded = 0;
        const auto reaches = [&](const Order& io) {
            if (io.side == Side::Buy) return (standing.has_bid && standing.bid < io.price ? standing.bid : io.price) >= price;
            return (standing.has_ask && standing.ask > io.price ? standing.ask : io.price) <= price;
        };
        const auto offsetting = [&](auto& book, Side by, auto within) {
            for (auto level = book.begin(); level != book.end(); ) {
                if (!within(level->first)) break;
                auto& queue = level->second;
                for (auto at = queue.begin(); at != queue.end(); ) {
                    if (at->only != cross) { ++at; continue; }   // only the cross's own orders are offset
                    for (auto io = offsets.begin(); io != offsets.end() && at->qty > 0; ) {
                        if (io->side != by || !reaches(*io)) { ++io; continue; }
                        const std::uint64_t fill = std::min(at->qty, io->qty);
                        Trade t{};
                        t.aggressor_id = at->id;   // the cross's own order
                        t.resting_id   = io->id;   // and the imbalance-only order offsetting it
                        t.price        = price;
                        t.qty          = fill;
                        t.match_id     = ++matches_;
                        t.cross        = cross;
                        t.offset       = true;
                        t.resting_shown   = false;   // neither side was ever shown
                        t.aggressor_shown = false;
                        on_trade_(t);
                        at->qty -= fill;
                        io->qty -= fill;
                        traded  += fill;
                        if (io->qty == 0) io = offsets.erase(io);
                        else ++io;
                    }
                    if (at->qty == 0) at = queue.erase(at);
                    else ++at;
                }
                if (queue.empty()) level = book.erase(level);
                else ++level;
            }
        };
        offsetting(bids_, Side::Sell, [&](std::int64_t level) { return level >= price; });   // the cross's buys, by imbalance-only sells
        offsetting(asks_, Side::Buy,  [&](std::int64_t level) { return level <= price; });   // and its sells, by imbalance-only buys
        for (const Order& left : offsets) on_eliminated_(left);   // what is left of them, as of any order for the cross
        return traded;
    }

    // The worst price a post-only order trades at as it arrives: an increment better than its limit
    // (Rule 4702(b)(4): price improvement of $0.01 a share or more, from $1.00).
    std::int64_t improving(const Order& o) const {
        if (o.price == market(o.side)) return o.price;   // unpriced: it has no limit to better
        const std::int64_t step = ticks_.at(o.price).second;
        return o.side == Side::Buy ? o.price - step : o.price + step;
    }

    // Would a post-only order trade as it arrives: does anything rest an increment better than its limit?
    bool trading(const Order& o) const {
        const std::int64_t reach = improving(o);
        return o.side == Side::Buy ? !asks_.empty() && asks_.begin()->first <= reach
                                   : !bids_.empty() && bids_.begin()->first >= reach;
    }

    // An order as its owner is told it was taken: a post-only one that trades nothing as it arrives
    // is taken at the price it posts at, which may better the one it was entered with (OUCH 5.0:
    // "the accepted price could potentially be different than the entered price if the order was
    // re-priced by the exchange on entry"). One that trades first is taken as entered; what is left
    // of it posts as any post-only order does, and its owner is not told the price it posts at.
    Order taken(const Order& o) const {
        Order as = o;
        if (o.post_only && !trading(o)) as.price = posting(o);
        return as;
    }

    // The price what is left of a post-only order posts at: its limit, or - where that would lock or
    // cross an order the book shows - the tick next inside the best such order. Orders the book
    // does not show are not in its way, and it may rest locking them.
    std::int64_t posting(const Order& o) const {
        if (o.price == market(o.side)) return o.price;
        if (o.side == Side::Buy) {
            for (const auto& [level, orders] : asks_) {
                if (level > o.price) break;
                if (std::none_of(orders.begin(), orders.end(), [](const Order& resting) { return resting.shown; })) continue;
                return ticks_.below(level - 1);
            }
        } else {
            for (const auto& [level, orders] : bids_) {
                if (level < o.price) break;
                if (std::none_of(orders.begin(), orders.end(), [](const Order& resting) { return resting.shown; })) continue;
                return ticks_.above(level + 1);
            }
        }
        return o.price;
    }

    // Trade Now (Nasdaq Equity 4, Rule 4703(m)): "a resting Order that is locked or crossed, as
    // applicable, at its non-displayed price by the posted price of an incoming Displayed Order ...
    // or another Order or Orders" executes "against a locking or crossing Order(s) as a liquidity
    // taker automatically when such Orders become marketable", and "Any remaining shares of the
    // resting Order will remain posted on the Nasdaq Book with the same priority". Looked for as an
    // order posts: each order the book does not show on the other side that carries the attribute,
    // best price first and in time order, crosses what now rests at its price or better - where it
    // names a minimum, only once that much is there - at the prices of the orders it takes.
    // JUDGEMENT: it is looked for only as an order entered posts, not as one is priced anew.
    void trade_now(Side posted) {
        if (posted == Side::Sell) take_now(bids_, asks_, [](std::int64_t limit, std::int64_t level) { return level <= limit; });
        else                      take_now(asks_, bids_, [](std::int64_t limit, std::int64_t level) { return level >= limit; });
    }

    template <typename Takers, typename Book, typename Reaches>
    void take_now(Takers& takers, Book& opposite, Reaches reaches) {
        for (auto level = takers.begin(); level != takers.end() && !opposite.empty() && reaches(level->first, opposite.begin()->first); ) {
            auto& queue = level->second;
            for (auto at = queue.begin(); at != queue.end() && !opposite.empty(); ) {
                if (!at->trade_now || at->shown) { ++at; continue; }
                Order in = *at;   // it takes as an arriving order would, and stays where it rests
                const std::int64_t limit = in.price;
                const auto crosses = [&](std::int64_t price) { return reaches(limit, price); };
                if (in.min_qty > 0 && available(in, opposite, crosses) < in.min_qty) { ++at; continue; }   // not yet as much as its minimum
                cross(in, opposite, crosses);
                at->qty = in.qty;
                at->min_qty = std::min(at->min_qty, at->qty);   // fewer left than its minimum: what is left is its minimum
                if (at->qty == 0) at = queue.erase(at);
                else ++at;
            }
            if (queue.empty()) level = takers.erase(level);
            else ++level;
        }
    }

    // Is the security under the Short Sale Price Test, as the venue declares it?
    void restricted(bool on) { restricted_ = on; }

    // The Short Sale Price Test (Nasdaq Equity 4, Rule 4763(b), (e)): a short sale neither trades nor
    // is shown at or below the best bid the book shows, so one priced there - or unpriced - is priced
    // an increment above it, its Permitted Price. One exempt from the test is not a short sale here.
    void permit(Order& order) const {
        if (!restricted_ || !order.short_sale || order.side != Side::Sell) return;
        for (const auto& [level, orders] : bids_) {
            if (level == market(Side::Buy)) continue;
            if (std::none_of(orders.begin(), orders.end(), [](const Order& resting) { return resting.shown; })) continue;
            const std::int64_t permitted = ticks_.above(level + 1);
            if (order.price < permitted) order.price = permitted;
            return;
        }
    }

    // The short sales the test repriced, following a declining best bid: "To reflect declines in the
    // national best bid, the Exchange will continue to re-price a short sale order at the lowest
    // Permitted Price down to the order's original limit price" (Nasdaq Equity 4, Rule 4763(e)). One
    // whose Permitted Price is now lower than it stands is repriced there, no lower than its limit - a
    // new priority, under a new id, told as resting again. Never up: a short sale shown above the bid is
    // left where it is shown. Done as an order arrives, as a peg follows the midpoint.
    void reshort() {
        if (!restricted_ || shorted_.empty()) return;
        std::int64_t permitted = 0;
        bool bid = false;
        for (const auto& [level, orders] : bids_) {
            if (level == market(Side::Buy)) continue;
            if (std::none_of(orders.begin(), orders.end(), [](const Order& resting) { return resting.shown; })) continue;
            permitted = ticks_.above(level + 1);
            bid = true;
            break;
        }
        if (!bid) return;   // no bid the book shows: nothing to follow
        for (auto at = shorted_.begin(); at != shorted_.end(); ) {
            bool there = false;
            std::int64_t stands = 0;
            for (const auto& [level, orders] : asks_) {
                for (const Order& resting : orders) {
                    if (resting.id == at->first) { there = true; stands = level; break; }
                }
                if (there) break;
            }
            if (!there) { at = shorted_.erase(at); continue; }   // traded away, canceled or expired
            const std::int64_t to = std::max(at->second, permitted);
            if (to >= stands) {   // no lower to go
                if (to == at->second) at = shorted_.erase(at);
                else ++at;
                continue;
            }
            Order o{};
            erase(asks_, at->first, o);
            Order told = o;   // rests again under a new id, naming the one it stood under, repriced
            told.renamed  = o.id;
            told.id       = (std::uint64_t{1} << 62) + ++refreshes_;   // no id an order is entered under
            told.repriced = true;
            told.price    = to;
            o = told;
            o.renamed  = 0;
            o.repriced = false;
            place(asks_[o.price], o);
            on_rest_(told);   // its owner told its new price and id, and the feed the order replaced
            if (to == at->second) at = shorted_.erase(at);   // at its limit: it goes no lower
            else { at->first = told.id; ++at; }
        }
    }

    // Reserve Size (Nasdaq Equity 4, Rule 4703(h)). An order posts showing its floor where it has
    // more than that, and without one otherwise; one the book does not show has none.
    static Order posted(Order o) {
        if (o.floor == 0) return o;
        if (!o.shown || o.qty <= o.floor) { o.floor = 0; o.display = 0; }
        else o.display = std::min(drawn(o), o.qty);   // its floor, or a Random Reserve's draw
        return o;
    }

    // Has a reserve order nothing left showing, with more behind it?
    static bool spent(const Order& o) { return o.floor > 0 && o.display == 0 && o.qty > 0; }

    // A reserve order shown again from what is behind it: under a new id, which says the one it
    // stood under, and showing its floor - or all that is left, without one, where that is no more.
    Order refreshed(const Order& o) {
        Order next = o;
        next.renamed = o.id;
        next.id = (std::uint64_t{1} << 62) + ++refreshes_;   // no id an order is entered under
        if (next.qty <= next.floor) { next.floor = 0; next.display = 0; }
        else next.display = std::min(drawn(next), next.qty);   // its floor, or a Random Reserve's draw
        return next;
    }

    // The order at a place in a level shown again: it leaves its place for the back of the shown
    // orders there, as a new displayed order would stand, and its owner and the feed are told it
    // rests under its new id. Returns the place the next order now stands at.
    template <typename Queue>
    typename Queue::iterator renewed(Queue& queue, typename Queue::iterator at) {
        const Order next = refreshed(*at);
        const auto stood = at - queue.begin();
        queue.erase(at);
        Order kept = next;
        kept.renamed = 0;
        place(queue, kept);
        on_rest_(next);
        return queue.begin() + stood;
    }

    // And one shown again where it stands, as an auction leaves it.
    void renew(Order& o) {
        if (!spent(o)) return;
        const Order next = refreshed(o);
        o = next;
        o.renamed = 0;
        on_rest_(next);
    }
    // A cancel of an order on the book that trades on, not of one an auction collects apart: what
    // the auction holds is locked in. Returns false, and nothing changes, for one it holds.
    bool on_cancel_trading(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed) && !waited(id, removed) && !lived(id, removed)) return false;
        on_canceled_(removed);
        return true;
    }

    // A replace of an order on the book that trades on, raised as one; one an auction holds apart is
    // locked in, and refused. Returns false, and nothing changes, for one it holds.
    bool take_replaced_trading(std::uint64_t id) {
        Order removed{};
        replaced_waiting_ = false;
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed)) {
            if (!waited(id, removed)) return false;   // nor waiting aside for its cross
            replaced_waiting_ = true;
        }
        replacing_ = id;
        expires_replaced_ = removed.expires;
        return true;
    }
    // Is an order one the auction holds apart, waiting for its cross?
    bool holds(std::uint64_t id) const {
        return held(held_bids_, id) || held(held_asks_, id) || std::any_of(waiting_.begin(), waiting_.end(), [id](const Order& o) { return o.id == id; });
    }

    template <typename Book>
    static bool held(const Book& book, std::uint64_t id) {
        for (const auto& level : book) {
            for (const auto& o : level.second) {
                if (o.id == id) return true;
            }
        }
        return false;
    }

    // A cancel held back until the cross is done, of an order the auction holds: 1 where it is held
    // back now, 2 where it already was, 0 for an order the auction does not hold.
    int defer_cancel(std::uint64_t id) {
        if (!holds(id)) return 0;
        if (std::find(pending_.begin(), pending_.end(), id) != pending_.end()) return 2;
        pending_.push_back(id);
        return 1;
    }

    // The cross done: each cancel held back is carried out, of what of its order the cross left.
    void cancel_pending() {
        for (const std::uint64_t id : pending_) on_cancel(id);
        pending_.clear();
    }
    bool on_cancel(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !erase(held_bids_, id, removed) && !erase(held_asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed) && !waited(id, removed) && !extended_out(id, removed) && !lived(id, removed)) return false;
        on_canceled_(removed);
        return true;
    }

    // A replace, raised as one event: the order replaced, as what of it now rests. Its old order is
    // taken out without a word, and the replacement given as the phase it arrives in gives an
    // order; where it rests, that rest is the replace. Returns false, and nothing changes, when no
    // such order rests.
    bool take_replaced(std::uint64_t id) {
        Order removed{};
        replaced_waiting_ = false;
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !erase(held_bids_, id, removed) && !erase(held_asks_, id, removed) && !unparked(id, removed) && !unqueued(id, removed) && !extended_out(id, removed) && !lived(id, removed)) {
            if (!waited(id, removed)) return false;   // nor waiting aside for its cross
            replaced_waiting_ = true;
        }
        replacing_ = id;
        expires_replaced_ = removed.expires;
        return true;
    }

    // When the order a replace took out expires: its replacement keeps it, as IEX's ExpireTime is
    // the original's on a replace (IEX FIX Specification v2.91, p. 78).
    std::int64_t expires_replaced() const { return expires_replaced_; }

    // Was the order a replace took out waiting aside for its cross, its cross not collecting yet? Its
    // replacement then waits there again, behind what waits already, and the replace is told as one:
    // a MOC entered in the morning may be replaced before the Closing Cross collects (OUCH 5.0 refuses
    // a replace only in a cross's late period, which a phase's guard states).
    bool replaced_waiting() const { return replaced_waiting_; }
    void rewait(Order order) {
        waiting_.push_back(order);
        replaced_waiting_ = false;
        const std::uint64_t id = replacing_;
        replacing_ = 0;
        on_replaced_(id, order);
    }

    // A replacement that rested nothing - it traded away as it arrived, or was never to rest - is the
    // order replaced with nothing left of it.
    void replaced(Order order) {
        if (replacing_ == 0) return;
        order.qty = 0;
        const std::uint64_t id = replacing_;
        replacing_ = 0;
        on_replaced_(id, order);
    }

    // A reduce: the order lowered where it stands, keeping its place in time. Lowered to nothing it
    // is canceled; to as much as it has open, or more, nothing changes. Returns false when no such
    // order rests.
    bool reduce(std::uint64_t id, std::uint64_t qty) {
        if (qty == 0) return on_cancel(id);
        return reduced(bids_, id, qty) || reduced(asks_, id, qty) || reduced(held_bids_, id, qty) || reduced(held_asks_, id, qty) || lowered(parked_, id, qty) || lowered(queued_pegs_, id, qty) || extended_reduced(id, qty) || life_reduced(id, qty);
    }

    // A reduce of an order on the book that trades on, not of one an auction collects apart: what the
    // auction holds is locked in. Returns false, and nothing changes, for one it holds.
    bool reduce_trading(std::uint64_t id, std::uint64_t qty) {
        if (qty == 0) return on_cancel_trading(id);
        return reduced(bids_, id, qty) || reduced(asks_, id, qty) || life_reduced(id, qty);
    }

    // A pegged order lowered where it waits out of the book, as one on the book is lowered.
    bool lowered(std::deque<Order>& waiting, std::uint64_t id, std::uint64_t qty) {
        for (auto& o : waiting) {
            if (o.id != id) continue;
            if (qty >= o.qty) return true;
            const std::uint64_t taken = o.qty - qty;
            o.qty = qty;
            on_reduced_(o, taken);
            return true;
        }
        return false;
    }

    template <typename Book>
    bool reduced(Book& book, std::uint64_t id, std::uint64_t qty) {
        for (auto& level : book) {
            for (auto& o : level.second) {
                if (o.id != id) continue;
                if (qty >= o.qty) return true;
                const std::uint64_t taken = o.qty - qty;
                if (o.floor > 0) {   // from behind its floor first, which the book never showed
                    const std::uint64_t behind = std::min(taken, o.qty - o.display);
                    o.qty = qty;
                    if (behind > 0) { Order unseen = o; unseen.shown = false; on_reduced_(unseen, behind); }
                    if (taken > behind) { o.display -= taken - behind; on_reduced_(o, taken - behind); }
                    if (o.qty <= o.display) { o.floor = 0; o.display = 0; }   // nothing behind it now: it stands as any shown order
                    return true;
                }
                o.qty = qty;
                on_reduced_(o, taken);
                return true;
            }
        }
        return false;
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

    // A cross that will not come for the book, as its day says: every order for it alone, waiting
    // aside or held for it, leaves the book, eliminated for the reason given, and the cross is
    // done for the day, so an order for it arriving later is refused - an orderbook halted through
    // the Opening Cross takes no part in it.
    void expire_cross(Cross cross, Elimination reason = Elimination::Unstated) {
        if (cross == Cross::Extended) { end_extended(reason); return; }   // the Extended Trading Close ends
        if (cross == Cross::ExtendedLife) { end_lives(reason); return; }   // the Midpoint Extended Life orders' hours end
        if (cross != Cross::Intraday) done_[static_cast<std::size_t>(cross)] = true;
        for (auto at = waiting_.begin(); at != waiting_.end(); ) {
            if (at->only != cross) { ++at; continue; }
            Order gone = *at;
            gone.eliminated = reason;
            at = waiting_.erase(at);
            on_eliminated_(gone);
        }
        forgone(bids_, cross, reason);
        forgone(asks_, cross, reason);
        forgone(held_bids_, cross, reason);
        forgone(held_asks_, cross, reason);
    }

    template <typename Book>
    void forgone(Book& book, Cross cross, Elimination reason) {
        for (auto level = book.begin(); level != book.end(); ) {
            auto& queue = level->second;
            for (auto at = queue.begin(); at != queue.end(); ) {
                if (at->only != cross) { ++at; continue; }
                Order gone = *at;
                gone.eliminated = reason;
                at = queue.erase(at);
                on_eliminated_(gone);
            }
            if (queue.empty()) level = book.erase(level);
            else ++level;
        }
    }

    // The Extended Trading Close: after a Closing Cross that traded, its orders match continuously at the
    // Nasdaq Official Closing Price, in the order each came into it, until its end (Nasdaq Equity 4, Rule
    // 4755). Is it running?
    bool extending() const { return extending_; }

    // It starts, at the Closing Cross's price: what the cross left of each order marked to join it, at or
    // within its limit, joins it, in the order the cross ranked them - an ETC Eligible LOC Order (Rule
    // 4702(b)(12)(A)).
    void extend_from(std::int64_t price) {
        extending_ = true;
        extended_price_ = price;
        extended_matched_ = 0;
        joined(bids_, [price](const Order& o) { return o.price >= price; });
        joined(asks_, [price](const Order& o) { return o.price <= price; });
        resume_extended();
    }

    // What the Closing Cross left of each order marked to join it, within its limit, out of the book and into the
    // Extended Trading Close, best price first and in time at each, as the cross ranked them.
    template <typename Book, typename Within>
    void joined(Book& book, Within within) {
        for (auto level = book.begin(); level != book.end(); ) {
            auto& queue = level->second;
            for (auto at = queue.begin(); at != queue.end(); ) {
                if (at->only != Cross::Closing || !at->extends || at->price == market(at->side) || !within(*at)) { ++at; continue; }
                Order joining = *at;
                at = queue.erase(at);
                joining.only = Cross::Extended;
                joining.price = extended_price_;
                joining.lifetime = Lifetime::Session;
                joining.shown = false;
                extended_.push_back(joining);
            }
            if (queue.empty()) level = book.erase(level);
            else ++level;
        }
    }
    // An order for it, arriving while it runs: it joins behind what is there, at its price whatever price it
    // states, and lives until it ends whatever its time in force says - an ETC Order's price "shall be the
    // Nasdaq Official Closing price" and its "Time-in-Force is ETC" (Rule 4702(b)(17)(B)). Never shown. A
    // replacement is told as the replace.
    void extend(Order order) {
        order.only = Cross::Extended;
        order.price = extended_price_;
        order.lifetime = Lifetime::Session;
        order.shown = false;
        if (replacing_ != 0) { const std::uint64_t id = replacing_; replacing_ = 0; on_replaced_(id, order); }
        else on_accepted_(order);
        extended_.push_back(order);
        resume_extended();
    }

    // Its orders match what they may now, unless it is suspended: each, in the order it came in, against those
    // before it on the other side, the earlier first, at its price. A Minimum Quantity order trades only with an
    // order whose size meets its minimum - "each of which must have a size that satisfies the minimum quantity
    // condition" (Rule 4702(b)(17)(B)). Asked whenever the book trading on may have changed.
    void resume_extended() {
        if (!extending_ || suspended()) return;
        for (std::size_t in = 1; in < extended_.size(); ++in) {
            for (std::size_t at = 0; at < in && extended_[in].qty > 0; ++at) {
                Order& later = extended_[in];
                Order& earlier = extended_[at];
                if (earlier.qty == 0 || earlier.side == later.side) continue;
                if (earlier.min_qty > later.qty || later.min_qty > earlier.qty) continue;   // a size short of a minimum
                const std::uint64_t fill = std::min(later.qty, earlier.qty);
                Trade trade{ later.id, earlier.id, extended_price_, fill, ++matches_, Cross::Extended };
                trade.resting_shown = false;   // neither side was ever shown
                on_trade_(trade);
                later.qty -= fill;
                earlier.qty -= fill;
                extended_matched_ += fill;
            }
        }
        extended_.erase(std::remove_if(extended_.begin(), extended_.end(), [](const Order& o) { return o.qty == 0; }), extended_.end());
    }

    // Is it suspended: does the book trading on hold a bid above its price, or an offer below it - "an Order in
    // that same security resting on the Nasdaq Continuous Book in After Hours Trading with a bid (offer) price
    // that is higher than (lower than) the Nasdaq Official Closing Price" (Rule 4755(b)(3)(i))?
    bool suspended() const {
        for (const auto& [level, orders] : bids_) { if (level == market(Side::Buy) || orders.empty()) continue; if (level > extended_price_) return true; break; }
        for (const auto& [level, orders] : asks_) { if (level == market(Side::Sell) || orders.empty()) continue; if (level < extended_price_) return true; break; }
        return false;
    }

    // What it says asked what it would cross at: the shares it has matched so far as paired, the shares left
    // unmatched on the larger side - Minimum Quantity orders aside - as its imbalance, and its price as its Far,
    // Near and Current Reference Price (Rule 4755(a)(8)).
    Indicative extended_indicated() const {
        std::uint64_t buys = 0, sells = 0;
        for (const Order& o : extended_) {
            if (o.min_qty > 0) continue;
            if (o.side == Side::Buy) buys += o.qty; else sells += o.qty;
        }
        Indicative at{};
        at.price   = extended_price_;
        at.qty     = extended_matched_;
        at.surplus = buys > sells ? buys - sells : sells - buys;
        at.buyers  = buys > sells;
        at.near_price = extended_price_;
        at.far_price  = extended_price_;
        return at;
    }

    // It ends: what is left of every order in it is eliminated, suspended or not - "any unexecuted portion of
    // the Order will be canceled" (Rule 4702(b)(17)(A); Rule 4755(b)(3), (4)).
    void end_extended(Elimination reason) {
        extending_ = false;
        for (Order gone : extended_) { gone.eliminated = reason; on_eliminated_(gone); }
        extended_.clear();
    }

    // Take an order out of the Extended Trading Close. Returns true if it was there.
    bool extended_out(std::uint64_t id, Order& removed) {
        for (auto at = extended_.begin(); at != extended_.end(); ++at) {
            if (at->id != id) continue;
            removed = *at;
            extended_.erase(at);
            return true;
        }
        return false;
    }

    // An order of the Extended Trading Close lowered where it stands, keeping its time. Returns true if it was there.
    bool extended_reduced(std::uint64_t id, std::uint64_t qty) {
        for (auto& o : extended_) {
            if (o.id != id) continue;
            if (qty >= o.qty) return true;
            const std::uint64_t taken = o.qty - qty;
            o.qty = qty;
            on_reduced_(o, taken);
            return true;
        }
        return false;
    }
    // The Midpoint Extended Life orders (Nasdaq Equity 4, Rule 4702(b)(14), (15)): never shown, priced at the
    // midpoint of the best bid and offer the book shows - its own, standing in for the NBBO, of which the
    // simulator has no source - and each eligible to trade only once its holding period has passed, then
    // only with the others of its kind, and a M-ELO+CB also with the continuous book's Trade Now
    // midpoint pegs. They trade in Regular Market Hours alone, matched each time the day's clock
    // is handed, so a holding period ends up to a tick of that clock late.

    // An order for it, accepted: its holding period begun where the book's hours have started them and the
    // midpoint is within its limit, and held otherwise - pre-market, halted, or priced away. An IOC one that
    // cannot begin it now is eliminated: "if the Order is ineligible to begin the Holding Period upon entry,
    // then the System will cancel it immediately" (Rule 4702(b)(14)(B)). A replacement is told as the replace,
    // and begins a holding period anew: a modification "other than to decrease the size of the Order" restarts
    // it (Rule 4702(b)(14)(A)).
    void live(Order order, std::int64_t now) {
        order.only = Cross::ExtendedLife;
        order.shown = false;
        order.life_due = 0;
        if (replacing_ != 0) { const std::uint64_t id = replacing_; replacing_ = 0; on_replaced_(id, order); }
        else on_accepted_(order);
        if (lives_active_) {
            const auto twice = life_midpoint();
            if (twice && life_within(order, *twice)) order.life_due = now + life_holding_;
        }
        if (order.lifetime == Lifetime::Immediate && order.life_due == 0) { on_eliminated_(order); return; }
        lives_.push_back(order);
    }

    // The book's hours start them: from the open, each held order begins its holding period where the
    // midpoint is within its limit; as a halt is lifted, each whose period had not passed begins the resumed
    // one, "running from the time when trading resumes", and one already eligible stays so (Rule
    // 4702(b)(14)(A)).
    void start_lives(std::int64_t now, bool resumed) {
        lives_active_ = true;
        const auto twice = life_midpoint();
        for (Order& o : lives_) {
            if (resumed && o.life_due != 0 && o.life_due <= now) continue;   // eligible already
            o.life_due = twice && life_within(o, *twice) ? now + (resumed ? life_resumed_ : life_holding_) : 0;
        }
    }

    // A halt stops them: "Midpoint Extended Life Orders in existence at the time a halt is initiated will be
    // ineligible to execute and held by the System until trading has resumed" (Rule 4702(b)(14)(A)).
    void stop_lives() { lives_active_ = false; }

    // Their hours end: "A Midpoint Extended Life Order remaining unexecuted after 4:00 p.m. ET will be
    // cancelled by the System" (Rule 4702(b)(14)(B)). What is left of each is eliminated.
    void end_lives(Elimination reason) {
        lives_active_ = false;
        for (Order gone : lives_) { gone.eliminated = reason; on_eliminated_(gone); }
        lives_.clear();
    }

    // The day's clock handed: each held order whose limit the midpoint is now within begins its holding
    // period; then, unless an order resting unseen is priced beyond the midpoint, those eligible trade at it,
    // ranked by when each became eligible, a later one taking from those before it on the other side, the
    // earlier first, and what is left of a M-ELO+CB then with the continuous book's
    // Trade Now midpoint pegs; and what is left of an eligible IOC one is eliminated, "if no such
    // resting interest is available, or shares of the Order remain unexecuted" (Rule 4702(b)(14)(B)).
    void lives(std::int64_t now) {
        if (!lives_active_) return;
        const auto twice = life_midpoint();
        if (!twice) return;   // no midpoint: nothing begins, and nothing trades
        life_rested(bids_, *twice, now);
        life_rested(asks_, *twice, now);
        for (Order& o : lives_) {
            if (o.life_due == 0 && life_within(o, *twice)) o.life_due = now + life_holding_;
        }
        if (!life_blocked(*twice)) {
            const std::int64_t price = *twice / 2;   // a midpoint between two prices taken at the lower
            std::vector<std::size_t> ranked;
            for (std::size_t at = 0; at < lives_.size(); ++at) {
                if (life_eligible(lives_[at], *twice, now)) ranked.push_back(at);
            }
            std::stable_sort(ranked.begin(), ranked.end(), [this](std::size_t a, std::size_t b) { return lives_[a].life_due < lives_[b].life_due; });
            for (std::size_t in = 1; in < ranked.size(); ++in) {
                for (std::size_t at = 0; at < in && lives_[ranked[in]].qty > 0; ++at) {
                    Order& later = lives_[ranked[in]];
                    Order& earlier = lives_[ranked[at]];
                    if (earlier.qty == 0 || earlier.side == later.side) continue;
                    if (earlier.min_qty > later.qty || later.min_qty > earlier.qty) continue;   // a size short of a minimum
                    const std::uint64_t fill = std::min(later.qty, earlier.qty);
                    Trade trade{ later.id, earlier.id, price, fill, ++matches_, Cross::ExtendedLife };
                    trade.resting_shown = false;   // neither side was ever shown
                    on_trade_(trade);
                    later.qty -= fill;
                    earlier.qty -= fill;
                }
            }
            for (const std::size_t at : ranked) {
                if (lives_[at].qty > 0 && lives_[at].life_continuous) life_pegs(lives_[at], price, *twice, now);
            }
        }
        for (auto at = lives_.begin(); at != lives_.end(); ) {
            const bool spent = at->lifetime == Lifetime::Immediate && at->life_due != 0 && at->life_due <= now;   // an IOC one, its period passed
            if (at->qty > 0 && !spent) { ++at; continue; }
            if (at->qty > 0) on_eliminated_(*at);
            at = lives_.erase(at);
        }
    }

    // Twice the midpoint of the best bid and offer the book shows, so that half a price is kept; none where a
    // side shows nothing.
    std::optional<std::int64_t> life_midpoint() const {
        const auto best = [](const auto& book) -> std::optional<std::int64_t> {
            for (const auto& [level, orders] : book) {
                if (level == market(Side::Buy) || level == market(Side::Sell)) continue;   // a market order shows no price
                for (const Order& o : orders) if (o.shown) return level;
            }
            return std::nullopt;
        };
        const auto bid = best(bids_);
        const auto ask = best(asks_);
        if (!bid || !ask) return std::nullopt;
        return *bid + *ask;
    }

    // Is the midpoint within an order's limit - a buy's at or below it, a sell's at or above it? One with no
    // limit takes any.
    static bool life_within(const Order& o, std::int64_t twice) {
        if (o.price == market(o.side)) return true;
        return o.side == Side::Buy ? twice <= 2 * o.price : twice >= 2 * o.price;
    }

    // May an order trade now: its holding period passed, and the midpoint within its limit? One that passed
    // it keeps its rank while the midpoint is away from its limit (Rule 4702(b)(14)(A)).
    static bool life_eligible(const Order& o, std::int64_t twice, std::int64_t now) {
        return o.qty > 0 && o.life_due != 0 && o.life_due <= now && life_within(o, twice);
    }

    // Is an order resting unseen priced beyond the midpoint - a hidden bid above it, or a hidden offer below?
    // Then none trades: they "will not execute if there is a resting non-displayed Order priced more
    // aggressively than the midpoint between the NBBO" (Rule 4702(b)(14)(A)).
    bool life_blocked(std::int64_t twice) const {
        for (const auto& [level, orders] : bids_) {
            if (level == market(Side::Buy)) continue;
            if (2 * level <= twice) break;
            for (const Order& o : orders) if (!o.shown) return true;
        }
        for (const auto& [level, orders] : asks_) {
            if (level == market(Side::Sell)) continue;
            if (2 * level >= twice) break;
            for (const Order& o : orders) if (!o.shown) return true;
        }
        return false;
    }

    // Take an order out of the Midpoint Extended Life book. Returns true if it was there.
    bool lived(std::uint64_t id, Order& removed) {
        for (auto at = lives_.begin(); at != lives_.end(); ++at) {
            if (at->id != id) continue;
            removed = *at;
            lives_.erase(at);
            return true;
        }
        return false;
    }

    // A Midpoint Extended Life order lowered where it stands, keeping its holding period and its rank: a
    // modification restarts it "other than to decrease the size of the Order" (Rule 4702(b)(14)(A)). Returns
    // true if it was there.
    bool life_reduced(std::uint64_t id, std::uint64_t qty) {
        for (auto& o : lives_) {
            if (o.id != id) continue;
            if (qty >= o.qty) return true;
            const std::uint64_t taken = o.qty - qty;
            o.qty = qty;
            on_reduced_(o, taken);
            return true;
        }
        return false;
    }

    // What is left of an eligible M-ELO+CB, once the others of its kind have traded with it: the Trade Now
    // midpoint pegs on the other side resting at the midpoint, in the order the book ranks them there, each
    // that has rested at it a holding period and whose size meets its minimum - "(i) the Midpoint Pegging
    // Order has the Trade Now Attribute enabled; (ii) no other order is resting on the Continuous Book that
    // has a more aggressive price than the current midpoint of the NBBO; (iii) the Midpoint Pegging Order has
    // rested on the Exchange's Continuous Book for a minimum of the duration of the Holding Period ... ; and
    // (iv) the Midpoint Pegging Order satisfies any minimum quantity requirement of the M-ELO+CB" (Rule
    // 4702(b)(15)). It takes them as the one arriving.
    void life_pegs(Order& taker, std::int64_t price, std::int64_t twice, std::int64_t now) {
        if (2 * price != twice) return;   // a midpoint between two prices, at which no peg stands
        if (taker.side == Side::Buy) life_pegged(taker, asks_, price, now);
        else                         life_pegged(taker, bids_, price, now);
    }

    template <typename Book>
    void life_pegged(Order& taker, Book& book, std::int64_t price, std::int64_t now) {
        const auto level = book.find(price);
        if (level == book.end()) return;
        auto& queue = level->second;
        for (auto at = queue.begin(); at != queue.end() && taker.qty > 0; ) {
            const bool rested = at->at_midpoint != 0 && now - at->at_midpoint >= life_holding_;
            if (at->peg != Peg::Midpoint || !at->trade_now || at->shown || !rested || at->qty < taker.min_qty || at->min_qty > taker.qty) { ++at; continue; }
            const std::uint64_t fill = std::min(taker.qty, at->qty);
            Trade trade{ taker.id, at->id, price, fill, ++matches_, Cross::ExtendedLife };
            trade.resting_shown = false;   // neither side was ever shown
            trade.resting_midpoint = true;   // the resting side followed the midpoint
            on_trade_(trade);
            taker.qty -= fill;
            at->qty -= fill;
            at->min_qty = std::min(at->min_qty, at->qty);   // fewer left than its minimum: what is left is its minimum
            if (at->qty == 0) at = queue.erase(at);
            else ++at;
        }
        if (queue.empty()) book.erase(level);
    }

    // Since when each Trade Now midpoint peg has rested at the midpoint: stamped as the day's clock finds it
    // there, and cleared where its limit holds it away from it - it must have rested "after the NBBO midpoint
    // falls within the limit set by the participant" (Rule 4702(b)(15)(iii)).
    template <typename Book>
    static void life_rested(Book& book, std::int64_t twice, std::int64_t now) {
        for (auto& [level, orders] : book) {
            for (Order& o : orders) {
                if (o.peg != Peg::Midpoint || !o.trade_now) continue;
                if (2 * level != twice) o.at_midpoint = 0;
                else if (o.at_midpoint == 0) o.at_midpoint = now;
            }
        }
    }
    // The best limit price the book trading on has on a side, of what it shows where it hides some, its
    // market orders aside; none where it has none - what a cross's benchmark or price test measures from:
    // Nasdaq's "Nasdaq best bid ... or Nasdaq best offer" (Equity 4, Rule 4752(d)(2)(F)(iii)).
    std::optional<std::int64_t> quoted(Side side) const {
        if (side == Side::Buy) {
            for (const auto& [level, orders] : bids_) { if (level != market(Side::Buy) && std::any_of(orders.begin(), orders.end(), [](const Order& o) { return o.shown; })) return level; }
            return std::nullopt;
        }
        for (const auto& [level, orders] : asks_) { if (level != market(Side::Sell) && std::any_of(orders.begin(), orders.end(), [](const Order& o) { return o.shown; })) return level; }
        return std::nullopt;
    }

    // The last price the book traded at, as an order arrived or in a cross, 0 before it has.
    std::int64_t traded() const { return traded_; }

    // The shares of market orders on a side that a cross would leave unexecuted at what it would cross
    // at: a market order comes before every price on its side, so those beyond the shares the cross
    // would match - all of them where nothing would cross. Nasdaq's "all market orders would not be
    // executed in the cross" (Equity 4, Rule 4120(b)(4)(A)(i)a.5.b) and its Market Order Imbalance, "the
    // number of shares of Eligible Interest entered through market orders that would not be matched with
    // other order shares" (Rule 4753(a)(2)).
    std::uint64_t market_left(Side side, const Indicative& at) const {
        std::uint64_t shares = 0;
        if (side == Side::Buy) {
            for (const auto* book : { &bids_, &held_bids_ }) { const auto level = book->find(market(Side::Buy)); if (level == book->end()) continue; for (const auto& o : level->second) shares += (o.min_qty > 0 ? 0 : o.qty); }
        } else {
            for (const auto* book : { &asks_, &held_asks_ }) { const auto level = book->find(market(Side::Sell)); if (level == book->end()) continue; for (const auto& o : level->second) shares += (o.min_qty > 0 ? 0 : o.qty); }
        }
        return shares > at.qty ? shares - at.qty : 0;
    }

    // A cross that does not take place, though it collected: every order it collected - held for it, or
    // for it alone and waiting aside - is canceled back to its owner for the reason given, and the cross
    // is done for the day; the book trades on with what it held apart. Nasdaq's Opening Cross that passes
    // none of its Opening Cross Price Tests (Equity 4, Rule 4752(d)(2)(F)).
    void abandon(Cross cross, Elimination reason) {
        if (cross != Cross::Intraday) done_[static_cast<std::size_t>(cross)] = true;
        for (auto at = waiting_.begin(); at != waiting_.end(); ) {
            if (at->only != cross) { ++at; continue; }
            Order gone = *at;
            gone.eliminated = reason;
            at = waiting_.erase(at);
            on_canceled_(gone);
        }
        const auto cancel = [&](auto& book) {
            for (const auto& [level, orders] : book) {
                for (Order gone : orders) {
                    gone.eliminated = reason;
                    on_canceled_(gone);
                }
            }
            book.clear();
        };
        cancel(held_bids_);
        cancel(held_asks_);
        if (cross == Cross::Opening) open_pegs();   // the book trades on: what was queued until the open follows the midpoint
    }

    // The ticks this book's prices step by - its instrument's tick table - which an auction's
    // average is rounded to and a market order sweeps through. A book handed none steps by one.
    void ticked(Ticks ticks) { ticks_ = std::move(ticks); }

    // A price on its tick: the tick at or above it (toward above zero), at or below it (below zero),
    // or the nearer of the two, a midpoint up (zero) - as Nasdaq rounds a reference price a late LOO
    // or LOC is repriced to (Equity 4, Rule 4702(b)(9)(A), (12)(A)). A price on its tick is itself.
    std::int64_t on_tick(std::int64_t price, int toward) const {
        const std::int64_t below = ticks_.below(price);
        if (below == price) return price;
        const std::int64_t above = ticks_.above(price);
        if (toward > 0) return above;
        if (toward < 0) return below;
        return price - below < above - price ? below : above;
    }

    // An order held without crossing: an auction collects, and nothing trades until it uncrosses.
    // The book may stand crossed while it does.
    // An order never to rest has nothing it could trade with as it arrives, so it is eliminated - the
    // simulator's reading of a time in force no venue document here states for an auction.
    void hold(Order order) { if (replacing_ == 0) on_accepted_(order); if (order.peg != Peg::None) { queue_peg(order); return; } keep(order); }

    // The price an auction would cross at now, inside the band [floor, ceiling]. Of every price an
    // order is limited to - one beyond the band at the limit it is willing at, a buy above the
    // ceiling at the ceiling and a sell below the floor at the floor; a market order has no price
    // of its own:
    // A state check's: an order the venue showed, placed in the book as the engine places a shown order
    // and not matched - a captured day's book is the venue's, and stood as it stood.
    void place_shown(const Order& o) {
        if (o.side == Side::Buy) { place(bids_[o.price], o); }
        else                     { place(asks_[o.price], o); }
    }

    // A state check's: shares the venue took of an order, found where it stands - its side and its
    // price - and gone from the book once none are left. False for an order not there.
    bool take_shown(Side side, std::int64_t price, std::uint64_t id, std::uint64_t shares) {
        return side == Side::Buy ? taken_shown(bids_, price, id, shares) : taken_shown(asks_, price, id, shares);
    }

    // The order an arriving order on the other side would meet first: the first at the best price on
    // this side, as cross() takes them. Null for a side with nothing on it.
    const Order* front(Side side) const {
        if (side == Side::Buy) { return bids_.empty() ? nullptr : &bids_.begin()->second.front(); }
        return asks_.empty() ? nullptr : &asks_.begin()->second.front();
    }

    // The best price on a side, and how many orders stand ahead of one at its own price; the order's
    // own place where it stands, and its count of orders where it is not there.
    std::optional<std::int64_t> best(Side side) const {
        if (side == Side::Buy) { return bids_.empty() ? std::nullopt : std::optional<std::int64_t>(bids_.begin()->first); }
        return asks_.empty() ? std::nullopt : std::optional<std::int64_t>(asks_.begin()->first);
    }
    std::size_t ahead(Side side, std::int64_t price, std::uint64_t id) const {
        return side == Side::Buy ? before(bids_, price, id) : before(asks_, price, id);
    }

private:
    template <typename Book>
    static bool taken_shown(Book& book, std::int64_t price, std::uint64_t id, std::uint64_t shares) {
        const auto level = book.find(price);
        if (level == book.end()) { return false; }
        auto& queue = level->second;
        for (auto at = queue.begin(); at != queue.end(); ++at) {
            if (at->id != id) { continue; }
            if (at->qty <= shares) { queue.erase(at); }
            else                   { at->qty -= shares; }
            if (queue.empty()) { book.erase(level); }
            return true;
        }
        return false;
    }

    template <typename Book>
    static std::size_t before(const Book& book, std::int64_t price, std::uint64_t id) {
        const auto level = book.find(price);
        if (level == book.end()) { return 0; }
        std::size_t count = 0;
        for (const Order& o : level->second) {
            if (o.id == id) { break; }
            ++count;
        }
        return count;
    }

public:
    // An auction's indication in the three prices of an Order Imbalance Indicator (Nasdaq Equity 4, Rule
    // 4752(a)(3); Rule 4754(a)(7)): the Near, what the cross would clear at over its own orders and the
    // book trading on together - the indicative; the Far, over its own orders alone; and the Current
    // Reference Price, over its own orders alone among the prices at or within the best bid and offer
    // of the book trading on, drawn to their midpoint, whose shares paired and left over it states.
    // A Halt Cross's three are its one indicative: its Far and Near "shall be the same as the Current
    // Reference Price" (Rule 4753(a)(3)(E)), which keeps within no bid and offer. Asked for early, it
    // states no Far and no Near: the early indicator "will exclude information about indicative
    // prices" (Rule 4752(a)(1)). A side of the book with no limit order leaves its bound open; with
    // not both, the price is drawn to the reference.
    // The Near and the Far are drawn as their cross is drawn (drawn()); the Current Reference Price
    // to its midpoint.
    Indicative indicated(Cross cross, bool early, std::int64_t reference,
                         std::int64_t floor   = std::numeric_limits<std::int64_t>::min(),
                         std::int64_t ceiling = std::numeric_limits<std::int64_t>::max()) const {
        const Indicative whole = indicative(reference, floor, ceiling, drawn(cross, reference));   // the Near: over the cross's own orders and the book
        Indicative at = whole;
        Indicative own = whole;   // the Far: over the cross's own orders alone
        if (cross != Cross::Intraday) {
            bool has_bid = false, has_ask = false;
            std::int64_t bid = 0, ask = 0;
            for (const auto& [level, orders] : bids_) { if (level == market(Side::Buy) || orders.empty()) continue; has_bid = true; bid = level; break; }
            for (const auto& [level, orders] : asks_) { if (level == market(Side::Sell) || orders.empty()) continue; has_ask = true; ask = level; break; }
            own = collected(floor, ceiling, drawn(cross, reference));
            at = collected(has_bid ? std::max(floor, bid) : floor, has_ask ? std::min(ceiling, ask) : ceiling,
                           has_bid && has_ask ? bid + ask : 2 * reference);
        }
        if (cross == Cross::Intraday) {   // a Market Order Imbalance: no Current Reference Price, and the shares of market orders it would leave its imbalance
            const std::uint64_t bought = market_left(Side::Buy, whole), sold = market_left(Side::Sell, whole);
            if (bought > 0 || sold > 0) {
                at.price = 0;
                at.surplus = std::max(bought, sold);
                at.buyers = bought > sold;
                at.near_price = 0;
                at.far_price = 0;
                at.early = early;
                return at;
            }
        }
        at.near_price = early ? 0 : whole.price;
        at.far_price  = early ? 0 : own.price;
        at.early      = early;
        return at;
    }

    // What the cross's own orders alone would cross at - those it collected, apart from the book
    // trading on - among the prices from low to high, by the uncross's steps, drawn to `toward`, which
    // is given at twice the price so a midpoint between two ticks is one. Nothing where low is above
    // high: a bid and offer standing crossed leave no price within them.
    Indicative collected(std::int64_t low, std::int64_t high, std::int64_t toward) const {
        Indicative best{};
        if (low > high) return best;
        auto take = [&](std::int64_t price) {
            std::uint64_t bought = 0, sold = 0;
            for (const auto& [level, orders] : held_bids_) { if (level < price) break; for (const auto& o : orders) bought += (o.min_qty > 0 ? 0 : o.qty); }
            for (const auto& [level, orders] : held_asks_) { if (level > price) break; for (const auto& o : orders) sold += (o.min_qty > 0 ? 0 : o.qty); }
            const std::uint64_t qty = std::min(bought, sold);
            if (qty == 0) return;
            const std::uint64_t surplus = bought > sold ? bought - sold : sold - bought;
            const bool buyers = bought > sold;
            const auto distance = [&](std::int64_t p) { return 2 * p > toward ? 2 * p - toward : toward - 2 * p; };
            const bool better =
                qty != best.qty         ? qty > best.qty :
                surplus != best.surplus ? surplus < best.surplus :
                surplus != 0 && buyers == best.buyers && price != best.price ? (buyers ? price > best.price : price < best.price) :
                distance(price) != distance(best.price) ? distance(price) < distance(best.price) :
                                          price > best.price;
            if (best.qty == 0 || better) best = Indicative{ price, qty, surplus, buyers };
        };
        for (const auto& level : held_bids_) {
            if (level.first != market(Side::Buy) && level.first >= low) take(std::min(level.first, high));
        }
        for (const auto& level : held_asks_) {
            if (level.first != market(Side::Sell) && level.first <= high) take(std::max(level.first, low));
        }
        return best;
    }
    // What a cross is drawn to where the steps before leave prices equal, at twice the price so a
    // midpoint between two ticks is one - indicative() takes it as toward, 0 drawing to the reference:
    //   the Opening cross, the nearest the midpoint of the best bid and offer of the book
    //   trading on - its reference where that book has not both,
    //   the Intraday cross, the nearest the last price the book traded at from its Opening
    //   Cross until its Closing Cross - its reference where it has not traded since,
    //   the Closing cross, the nearest the midpoint of the best bid and offer of the book
    //   trading on - its reference where that book has not both,
    //   any other, the nearest the reference price.
    std::int64_t drawn(Cross cross, std::int64_t reference) const {
        if (cross == Cross::Intraday) return 2 * (last_sale_ != 0 ? last_sale_ : reference);
        if (cross == Cross::Opening || cross == Cross::Closing) {
            bool has_bid = false, has_ask = false;
            std::int64_t bid = 0, ask = 0;
            for (const auto& [level, orders] : bids_) { if (level == market(Side::Buy) || orders.empty()) continue; has_bid = true; bid = level; break; }
            for (const auto& [level, orders] : asks_) { if (level == market(Side::Sell) || orders.empty()) continue; has_ask = true; ask = level; break; }
            if (has_bid && has_ask) return bid + ask;
        }
        return 2 * reference;
    }

    // What a cross would cross at, drawn as that cross is.
    Indicative indicative(Cross cross, std::int64_t reference,
                          std::int64_t floor   = std::numeric_limits<std::int64_t>::min(),
                          std::int64_t ceiling = std::numeric_limits<std::int64_t>::max()) const {
        return indicative(reference, floor, ceiling, drawn(cross, reference));
    }

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
                          std::int64_t ceiling = std::numeric_limits<std::int64_t>::max(),
                          std::int64_t toward  = 0) const {
        Indicative best{};
        auto take = [&](std::int64_t price) {
            std::uint64_t bought = 0, sold = 0;
            for (const auto* book : { &bids_, &held_bids_ }) for (const auto& [level, orders] : *book) { if (level < price) break; for (const auto& o : orders) bought += (o.min_qty > 0 ? 0 : o.qty); }
            for (const auto* book : { &asks_, &held_asks_ }) for (const auto& [level, orders] : *book) { if (level > price) break; for (const auto& o : orders) sold += (o.min_qty > 0 ? 0 : o.qty); }
            const std::uint64_t qty = std::min(bought, sold);
            if (qty == 0) return;
            const std::uint64_t surplus = bought > sold ? bought - sold : sold - bought;
            const bool buyers = bought > sold;
            const std::int64_t twice = toward != 0 ? toward : 2 * reference;   // what it is drawn to, at twice the price
            const auto distance = [&](std::int64_t p) { return 2 * p > twice ? 2 * p - twice : twice - 2 * p; };
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
        const std::int64_t toward = drawn(cross, reference);   // what it is drawn to, of the book as it stands before the cross
        release();   // the auction consolidates what it collected with the book
        std::deque<Order> aside = set_aside();   // a Minimum Quantity order takes no part in a cross
        std::deque<Order> offsets = set_offsets(cross);   // an imbalance-only order takes no part in the cross's price, nor in its match
        const Stood standing = stood();   // the book's best limits before it crosses, which one is priced to
        const auto at = indicative(reference, floor, ceiling, toward);
        std::uint64_t left = at.qty;

        while (left > 0 && !bids_.empty() && !asks_.empty()) {
            auto bid = bids_.begin();
            auto ask = asks_.begin();
            if (bid->first < at.price || ask->first > at.price) break;

            Order& buy  = bid->second.front();
            Order& sell = ask->second.front();
            const std::uint64_t fill = std::min({ buy.qty, sell.qty, left });

            for (std::uint64_t done = 0; done < fill; ) {
                const bool bought = buy.shown && showing(buy) > 0, sold = sell.shown && showing(sell) > 0;
                std::uint64_t part = fill - done;
                if (bought) part = std::min(part, showing(buy));
                if (sold)   part = std::min(part, showing(sell));
                Trade trade{ buy.id, sell.id, at.price, part, ++matches_, cross };
                trade.aggressor_shown = bought;   // both sides rested while it collected
                trade.resting_shown   = sold;
                on_trade_(trade);
                if (bought && buy.floor > 0)  buy.display  -= part;
                if (sold && sell.floor > 0)   sell.display -= part;
                done += part;
            }
            if (cross == Cross::Opening || (done(Cross::Opening) && !done(Cross::Closing))) last_sale_ = at.price;   // a trade in its regular hours: the last sale a cross may be drawn to
            traded_ = at.price;   // the last price it traded at, which a cross's price may be tested against
            buy.qty  -= fill;
            sell.qty -= fill;
            left     -= fill;
            if (buy.qty > 0)  renew(buy);
            if (sell.qty > 0) renew(sell);

            if (buy.qty == 0)  bid->second.pop_front();
            if (sell.qty == 0) ask->second.pop_front();
            if (bid->second.empty()) bids_.erase(bid);
            if (ask->second.empty()) asks_.erase(ask);
        }
        if (cross == Cross::Opening) open_pegs();   // the book trades on: what was queued until the open follows the midpoint
        Indicative crossed = at;
        if (at.qty > 0) crossed.qty += offset(offsets, cross, at.price, standing);   // what is left of the cross's own orders, offset
        else for (const Order& left : offsets) on_eliminated_(left);   // nothing crossed: nothing to offset
        rejoin(aside);   // and rejoins the book once it is done
        return crossed;
    }

    // An order that may trade only at one price - trade at last, at the closing auction's: it
    // crosses what rests at exactly that price, and the rest of it rests there.
    void on_new_order_at(Order order, std::int64_t price) {
        if (replacing_ == 0) on_accepted_(taken(order));   // taken, before anything it makes of it - a replacement is said as the replace
        order.price = price;
        if (order.side == Side::Buy)
            cross(order, asks_, [&](std::int64_t level) { return level == price; });
        else
            cross(order, bids_, [&](std::int64_t level) { return level == price; });

        if (order.qty == 0) return;
        if (order.lifetime == Lifetime::Immediate || order.price == market(order.side)) {
            order.shown = false;   // never to rest, so never shown: what is left goes
            on_eliminated_(order);
        }
        else {
            rest(order); discern(order);
            trade_now(order.side);   // what rests unseen across from it with Trade Now, locked by it, takes it
        }
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
                        case Prevention::Decrement: off = would; away = would; break;
                        case Prevention::Oldest:    away = resting.qty; break;
                        case Prevention::Newest:    off = in.qty; break;
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
                std::uint64_t fill = std::min(in.qty, showing(resting));
                Trade trade{ in.id, resting.id, best->first, fill, ++matches_ };
                trade.resting_shown = resting.shown;   // the aggressor was never shown: it had not rested
                trade.aggressor_shown = discerning_ && in.shown;   // a discretionary order's, which the book may show, takes shares out of it
                trade.resting_midpoint = resting.peg == Peg::Midpoint;   // it added its liquidity at the midpoint
                on_trade_(trade);
                if (done(Cross::Opening) && !done(Cross::Closing)) last_sale_ = best->first;   // a trade in its regular hours: the last sale a cross may be drawn to
                traded_ = best->first;   // the last price it traded at, which a cross's price may be tested against
                in.qty -= fill;
                resting.qty -= fill;
                if (resting.floor > 0) resting.display -= std::min(resting.display, fill);
                resting.min_qty = std::min(resting.min_qty, resting.qty);   // fewer left than its minimum: what is left is its minimum
                if (resting.qty == 0) at = queue.erase(at);
                else if (spent(resting)) at = renewed(queue, at);   // what it showed is spent: shown again, behind the shown orders here
                else ++at;
            }
            if (queue.empty()) best = opposite.erase(best);
            else ++best;
        }
    }

    void rest(const Order& given) {
        const Order o = posted(given);   // showing its floor, where it has one and more than it
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
    void keep(const Order& given) {
        const Order o = posted(given);   // showing its floor, where it has one and more than it
        if (o.side == Side::Buy) place(held_bids_[o.price], o);
        else                     place(held_asks_[o.price], o);
        if (replacing_ == 0) { on_rest_(o); return; }
        const std::uint64_t id = replacing_;
        replacing_ = 0;
        on_replaced_(id, o);
    }

    // The auction matches: what it collected joins the book, each order at the place its stamp gives
    // it among those resting at its price, as the engine ranks every order by when it took its place.
    void release() {
        for (const auto& [level, orders] : held_bids_) for (const Order& o : orders) placed(bids_[o.price], o);
        for (const auto& [level, orders] : held_asks_) for (const Order& o : orders) placed(asks_[o.price], o);
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
            Order told = o;   // rests again under a new id, naming the one it stood under, repriced
            told.renamed = o.id;
            told.id = (std::uint64_t{1} << 62) + ++refreshes_;   // no id an order is entered under
            told.repriced = true;
            *at = told.id;
            o = told;
            o.renamed = 0;
            o.repriced = false;
            if (o.side == Side::Buy) place(bids_[o.price], o);
            else                     place(asks_[o.price], o);
            on_rest_(told);   // its owner told its new price and id
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
    // Each is stamped as it takes its place: it ranks in time from now.
    template <typename Queue>
    void place(Queue& queue, Order o) {
        o.entered = ++entries_;
        if (!o.shown) { queue.push_back(o); return; }
        queue.insert(std::find_if(queue.begin(), queue.end(), [](const Order& resting) { return !resting.shown; }), o);
    }

    // An order moving back to the book - released from an auction, rejoining from aside - at the place
    // its stamp gives it: ahead of the orders of its kind, shown or hidden, that took their place after
    // it, and behind those that took theirs before.
    template <typename Queue>
    static void placed(Queue& queue, const Order& o) {
        queue.insert(std::find_if(queue.begin(), queue.end(), [&o](const Order& resting) {
            return o.shown ? (!resting.shown || resting.entered > o.entered) : (!resting.shown && resting.entered > o.entered);
        }), o);
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
    OnReduced    on_reduced_;
    std::deque<Order> waiting_;               // orders for one cross alone, until it collects
    std::array<bool, 6> done_{};              // the crosses done for the day, by Cross
    std::deque<Order> extended_;              // the Extended Trading Close's orders, in the order each came into it
    bool          extending_ = false;   // is the Extended Trading Close running
    std::int64_t  extended_price_ = 0;   // the price it trades at: the Closing Cross's
    std::uint64_t extended_matched_ = 0;   // the shares it has matched so far
    std::deque<Order> lives_;                 // the Midpoint Extended Life orders, in the order each was accepted
    bool          lives_active_ = false;   // may they trade: in Regular Market Hours, with no halt
    std::int64_t  life_holding_ = 1250000;   // how long one waits before it may trade, in nanoseconds, as the venue declares it
    std::int64_t  life_resumed_ = 12000000;   // and as a halt is lifted, for one whose period had not passed
    bool          replaced_waiting_ = false;   // did the order a replace took out wait aside for its cross
    bool          restricted_ = false;   // is the security under the short sale price test
    bool          discerning_ = false;   // is the book crossing a discretionary order as its Discretionary IOC
    std::deque<std::pair<std::uint64_t, std::int64_t>> shorted_;   // the short sales the test repriced, by id, with the limit each was entered at
    std::uint64_t refreshes_ = 0;   // the reserve orders shown again, each numbering the id it is shown under
    std::uint64_t entries_ = 0;   // the places orders have taken: each order taking one is numbered by it
    std::uint64_t matches_ = 0;
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> bids_;  // best = highest
    std::map<std::int64_t, std::deque<Order>>                            asks_;  // best = lowest
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> held_bids_;  // what an auction collects, apart from the book that trades on
    std::map<std::int64_t, std::deque<Order>>                            held_asks_;
    std::deque<std::uint64_t> pegged_;   // the pegged orders, in the order they were posted, each following the midpoint
    std::deque<Order>         parked_;   // pegged orders with no midpoint to follow, waiting out of the book
    std::deque<Order>         queued_pegs_;   // pegged orders queued until the open, out of its auction
    Ticks ticks_;                                                        // the prices it steps by, an average rounded to them
    std::int64_t  expires_replaced_ = 0;   // when the order a replace took out expires, which its replacement keeps
    std::deque<std::uint64_t> pending_;   // the cancels held back until the cross is done, of orders the auction holds
    std::int64_t  last_sale_ = 0;   // the last price it traded at from its Opening Cross until its Closing Cross, 0 before it has
    std::int64_t  traded_ = 0;   // the last price it traded at, 0 before it has
};}
