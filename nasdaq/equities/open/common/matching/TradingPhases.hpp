#pragma once

// Generated from the Nasdaq Equities Open engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <algorithm>
#include <cstdint>
#include <limits>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Continuous };
// Why a phase refuses an order, where the chart says: each word its refusals give.
enum class Reason : std::uint8_t { Lifetime, Type, Slid, FatFinger, LimitProtection };

// The trading day. An order is given to the phase the venue is in, which
// crosses it, takes it out of the book, or refuses it, as the chart says.
template <typename Engine, typename OnRefused>
class TradingPhases {
public:
    TradingPhases(Engine& engine, OnRefused on_refused)
        : engine_(engine), on_refused_(on_refused) {}

    Phase phase() const { return phase_; }

    // How many phase changes the day has been through: a venue that halted and
    // resumed is not one that never opened.
    std::uint64_t phases() const { return phases_; }

    // The time, in nanoseconds, as the owner hands it with each message it gives the
    // day: what a timer set now is due from. The day keeps no clock of its own.
    void at(std::int64_t now) { now_ = now; }

    // The time come, as the owner's clock hands it: each timer due by then fires, in
    // the order the chart sets them, and is not set again until the chart sets it.
    void clock(std::int64_t now) {
        now_ = now;
        engine_.lapse(now);   // what expires by a time of its own, its time come
    }

    void on_new_order(Order order) {
        switch (phase_) {
            case Phase::Continuous: if (order.lifetime == matching::Lifetime::Timed) { on_refused_(order, Reason::Lifetime); return; } if ((order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (order.withdraws && engine_.slides(order)) { on_refused_(order, Reason::Slid); return; } engine_.on_new_order(order); return;
        }
    }

    // A cancel, answering whether the order was in the book to take out. A phase
    // that refuses cancels holds nothing to take, so it answers no.
    bool on_cancel(std::uint64_t id) {
        switch (phase_) {
            case Phase::Continuous: return engine_.on_cancel(id);
        }
        return false;
    }
    // A reduce, answering whether the order was in the book: lowered where it stands in a phase
    // that takes cancels, as a cancel is taken. A phase that refuses cancels answers no.
    bool on_reduce(std::uint64_t id, std::uint64_t qty) {
        switch (phase_) {
            case Phase::Continuous: return engine_.reduce(id, qty);
        }
        return false;
    }

    // A replace: the order as it now stands, under the id it already has. It is taken
    // out and given again as the phase it arrives in says, so it loses its place; one
    // that was not in the book to take out is refused.
    // A replace: the replacement as it is to stand, under its own id, and the order it replaces.
    // One that only lowers the quantity, at the price the order rests at, is lowered where it
    // stands and keeps its place and its id. Any other takes the order out and gives the
    // replacement as the phase it arrives in gives an order: what of it rests is the replace,
    // and one that rests nothing is the replace with nothing left. A refusal leaves the order
    // as it rests.
    void on_replace(Order order, [[maybe_unused]] std::uint64_t replaced) {
        switch (phase_) {
            case Phase::Continuous: if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.on_new_order(order); engine_.replaced(order); return;
        }
    }

private:
    bool moved(Phase from, Phase to) {
        if (phase_ != from) return false;
        phase_ = to;
        ++phases_;
        return true;
    }

    // A limit order priced further through the book's best opposite price than its port lets it
    // be: Nasdaq's Fat Finger Protection, "evaluating whether the limit price is set too
    // aggressively through the NBBO (percentage-based and dollar-based controls are
    // available)" (Equity 6, Section 5(m)) - the tighter of the two where both are given. None
    // for a port giving neither, a market or pegged order, or where the other side shows nothing.
    bool fat_fingered(const Order& o) const {
        if (o.protection_percent == 0 && o.protection_amount == 0) return false;
        if (o.price == market(o.side)) return false;   // a market order is no limit order
        if (o.peg != Peg::None) return false;   // its price is the engine's
        const auto far = engine_.quoted(o.side == Side::Buy ? Side::Sell : Side::Buy);
        if (!far) return false;
        std::int64_t room = std::numeric_limits<std::int64_t>::max();
        if (o.protection_percent != 0) room = std::min(room, *far * static_cast<std::int64_t>(o.protection_percent) / 10000);
        if (o.protection_amount != 0) room = std::min(room, o.protection_amount);
        return o.side == Side::Buy ? o.price - *far > room : *far - o.price > room;
    }

    // A limit order priced beyond the venue's own limit from the book's best opposite price, whatever
    // its port: Nasdaq's Limit Order Protection, "The LOP Limit shall be the greater of 10% of the
    // LOP Reference Price or $0.50", the offer for a buy and the bid for a sell, and a sell is not
    // held to it while "the National Best Bid ... is equal to or less than $0.50" (Equity 4, Rule
    // 4757(c)) - the limit as the venue declares it. It "does not apply to Orders with Market and
    // Primary Pegging", and "A Midpoint Pegging Order with a discretion price would not be
    // subject to LOP": a Midpoint Peg without one is held to it by the limit it states.
    bool unprotected(const Order& o) const {
        if (o.price == market(o.side)) return false;   // a market order is no limit order, nor a peg stating no limit
        if (o.peg != Peg::None && (o.peg != Peg::Midpoint || o.discretion != 0)) return false;
        const auto reference = engine_.quoted(o.side == Side::Buy ? Side::Sell : Side::Buy);
        if (!reference) return false;   // no established reference price
        if (o.side == Side::Sell && *reference <= 5000) return false;   // a bid at or below which a sell is not held to it
        const std::int64_t room = std::max<std::int64_t>(*reference * 1000 / 10000, 5000);   // 10% of it, and 5000 at the least
        return o.side == Side::Buy ? o.price - *reference > room : *reference - o.price > room;
    }

    Engine&       engine_;
    OnRefused     on_refused_;
    Phase         phase_ = Phase::Continuous;
    std::uint64_t phases_ = 0;
    std::int64_t  now_ = 0;   // the time the owner last handed, in nanoseconds
};
}
