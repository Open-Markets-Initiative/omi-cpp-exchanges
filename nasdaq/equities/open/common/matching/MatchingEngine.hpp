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

struct Order {
    std::uint64_t id;
    Side          side;
    std::int64_t  price;   // in ticks
    std::uint64_t qty;
    Lifetime      lifetime = Lifetime::Session;   // how long it lives, as its time in force means
    bool          shown = true;   // does the book show it: a hidden one rests and trades unseen, and what never rested was never shown
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
    std::uint32_t protection_percent = 0;   // how far through the best opposite price its port lets it be priced, in hundredths of a percent of it; 0 for none
    std::int64_t  protection_amount = 0;   // and as an amount; 0 for none, the tighter of the two applying where both are given
    bool          repriced = false;   // told resting again because the engine repriced it, under a new id
    std::int64_t  discretion = 0;   // its discretionary price, the far end of the range it trades within unseen; 0 for none
    std::uint64_t random = 0;   // the range its displayed size is drawn from about its floor, where it has one: a Random Reserve
    bool          imbalance_only = false;   // for its cross, it trades only against the cross's own orders: an OIO or IO Order
    std::uint64_t entered = 0;   // when it took its place at its price, in the engine's own count: what ranks it in time there
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
    // the best down and then asks from the best up, each price in time order; 'P' a pegged order with nothing to follow, out of the book.
    template <typename Visit>
    void each(Visit visit) const {
        for (const auto& [level, orders] : bids_) for (const Order& o : orders) visit(o, 'B');
        for (const auto& [level, orders] : asks_) for (const Order& o : orders) visit(o, 'B');
        for (const Order& o : parked_) visit(o, 'P');
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
    bool on_cancel(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !unparked(id, removed)) return false;
        on_canceled_(removed);
        return true;
    }

    // A replace, raised as one event: the order replaced, as what of it now rests. Its old order is
    // taken out without a word, and the replacement given as the phase it arrives in gives an
    // order; where it rests, that rest is the replace. Returns false, and nothing changes, when no
    // such order rests.
    bool take_replaced(std::uint64_t id) {
        Order removed{};
        if (!erase(bids_, id, removed) && !erase(asks_, id, removed) && !unparked(id, removed)) return false;
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

    // A reduce: the order lowered where it stands, keeping its place in time. Lowered to nothing it
    // is canceled; to as much as it has open, or more, nothing changes. Returns false when no such
    // order rests.
    bool reduce(std::uint64_t id, std::uint64_t qty) {
        if (qty == 0) return on_cancel(id);
        return reduced(bids_, id, qty) || reduced(asks_, id, qty) || lowered(parked_, id, qty);
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

    // The ticks this book's prices step by - its instrument's tick table - which an auction's
    // average is rounded to and a market order sweeps through. A book handed none steps by one.
    void ticked(Ticks ticks) { ticks_ = std::move(ticks); }

    // An order held without crossing: an auction collects, and nothing trades until it uncrosses.
    // The book may stand crossed while it does.
    // An order never to rest has nothing it could trade with as it arrives, so it is eliminated - the
    // simulator's reading of a time in force no venue document here states for an auction.
    void hold(Order order) { if (replacing_ == 0) on_accepted_(order); if (order.lifetime == Lifetime::Immediate) { order.shown = false; on_eliminated_(order); } else rest(order); }

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
            for (const auto& [level, orders] : bids_) { if (level < price) break; for (const auto& o : orders) bought += (o.min_qty > 0 ? 0 : o.qty); }
            for (const auto& [level, orders] : asks_) { if (level > price) break; for (const auto& o : orders) sold += (o.min_qty > 0 ? 0 : o.qty); }
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
        std::deque<Order> aside = set_aside();   // a Minimum Quantity order takes no part in a cross
        std::deque<Order> offsets = set_offsets(cross);   // an imbalance-only order takes no part in the cross's price, nor in its match
        const Stood standing = stood();   // the book's best limits before it crosses, which one is priced to
        const auto at = indicative(reference, floor, ceiling);
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
    bool          restricted_ = false;   // is the security under the short sale price test
    bool          discerning_ = false;   // is the book crossing a discretionary order as its Discretionary IOC
    std::deque<std::pair<std::uint64_t, std::int64_t>> shorted_;   // the short sales the test repriced, by id, with the limit each was entered at
    std::uint64_t refreshes_ = 0;   // the reserve orders shown again, each numbering the id it is shown under
    std::uint64_t entries_ = 0;   // the places orders have taken: each order taking one is numbered by it
    std::uint64_t matches_ = 0;
    std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>> bids_;  // best = highest
    std::map<std::int64_t, std::deque<Order>>                            asks_;  // best = lowest
    std::deque<std::uint64_t> pegged_;   // the pegged orders, in the order they were posted, each following the midpoint
    std::deque<Order>         parked_;   // pegged orders with no midpoint to follow, waiting out of the book
    Ticks ticks_;                                                        // the prices it steps by, an average rounded to them
    std::int64_t  expires_replaced_ = 0;   // when the order a replace took out expires, which its replacement keeps
};}
