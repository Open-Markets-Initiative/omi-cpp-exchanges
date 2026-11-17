#pragma once

// Generated from the Nasdaq Equities Session engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <cstdint>
#include <limits>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Closed, PreOpen, OpeningLockIn, OpeningLockOut, OpeningLate, Continuous, Halted, Quoting, PreClose, ClosingLockOut, ClosingFreeze, PostMarket };
// Why a phase refuses an order, where the chart says: each word its refusals give.
enum class Reason : std::uint8_t { Lifetime, Type, Cross, Peg, Phase };

// What the venue announces as its phases change. The owner publishes each
// through every feed it serves, as that feed's own markings say it.
enum class Announcement : std::uint8_t { StartOfMessages, StartOfSystemHours, StartOfMarketHours, Halted, Quoting, Resumed, EndOfMarketHours, EndOfSystemHours, EndOfMessages };

// The trading day. An order is given to the phase the venue is in, which
// crosses it, takes it out of the book, or refuses it, as the chart says.
template <typename Engine, typename OnRefused, typename OnAnnounced, typename OnCrossed, typename OnIndicated>
class TradingPhases {
public:
    TradingPhases(Engine& engine, OnRefused on_refused, OnAnnounced on_announced, OnCrossed on_crossed, OnIndicated on_indicated)
        : engine_(engine), on_refused_(on_refused), on_announced_(on_announced), on_crossed_(on_crossed), on_indicated_(on_indicated) {}

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

    // The price an uncross is drawn towards when two prices would cross as much: the
    // orderbook's reference, which the owner sets from what the venue publishes.
    void reference(std::int64_t price) { reference_ = price; }

    // The prices an uncross may cross at, the owner's to set: the venue's own limits
    // where it states them, else what its rules put around the reference.
    void band(std::int64_t floor, std::int64_t ceiling) { floor_ = floor; ceiling_ = ceiling; }

    // What the closing auction crossed at, which trade at last trades at.
    std::int64_t closing_price() const { return close_; }

    // Is an auction collecting? An owner publishes the indicative price while one is.
    bool collecting() const { return phase_ == Phase::PreOpen || phase_ == Phase::OpeningLockIn || phase_ == Phase::OpeningLockOut || phase_ == Phase::OpeningLate || phase_ == Phase::Halted || phase_ == Phase::Quoting || phase_ == Phase::PreClose || phase_ == Phase::ClosingLockOut || phase_ == Phase::ClosingFreeze; }

    // What the auction being collected for would cross at now, told to the owner with which
    // auction it is; nothing while none is collecting.
    void indicate() {
        switch (phase_) {
            case Phase::PreOpen: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::OpeningLockIn: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::OpeningLockOut: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::OpeningLate: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::Halted: on_indicated_(Cross::Intraday, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::Quoting: on_indicated_(Cross::Intraday, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::PreClose: on_indicated_(Cross::Closing, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::ClosingLockOut: on_indicated_(Cross::Closing, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::ClosingFreeze: on_indicated_(Cross::Closing, engine_.indicative(reference_, floor_, ceiling_)); return;
            default: return;
        }
    }

    void on_new_order(Order order) {
        switch (phase_) {
            case Phase::Closed: on_refused_(order); return;
            case Phase::PreOpen: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLockIn: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLockOut: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Opening && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLate: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Opening && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Opening && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::Continuous: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
            case Phase::Halted: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } engine_.hold(order); return;
            case Phase::Quoting: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } engine_.hold(order); return;
            case Phase::PreClose: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
            case Phase::ClosingLockOut: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Closing && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
            case Phase::ClosingFreeze: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Closing) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
            case Phase::PostMarket: if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } if (order.lifetime == matching::Lifetime::Session) { on_refused_(order, Reason::Phase); return; } engine_.on_new_order(order); return;
        }
    }

    // A cancel, answering whether the order was in the book to take out. A phase
    // that refuses cancels holds nothing to take, so it answers no.
    bool on_cancel(std::uint64_t id) {
        switch (phase_) {
            case Phase::Closed: return false;
            case Phase::PreOpen: return engine_.on_cancel(id);
            case Phase::OpeningLockIn: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::OpeningLockOut: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::OpeningLate: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::Continuous: return engine_.on_cancel(id);
            case Phase::Halted: return engine_.on_cancel(id);
            case Phase::Quoting: return engine_.on_cancel(id);
            case Phase::PreClose: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::ClosingLockOut: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::ClosingFreeze: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::PostMarket: return engine_.on_cancel(id);
        }
        return false;
    }
    // A reduce, answering whether the order was in the book: lowered where it stands in a phase
    // that takes cancels, as a cancel is taken. A phase that refuses cancels answers no.
    bool on_reduce(std::uint64_t id, std::uint64_t qty) {
        switch (phase_) {
            case Phase::Closed: return false;
            case Phase::PreOpen: return engine_.reduce(id, qty);
            case Phase::OpeningLockIn: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::OpeningLockOut: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::OpeningLate: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::Continuous: return engine_.reduce(id, qty);
            case Phase::Halted: return engine_.reduce(id, qty);
            case Phase::Quoting: return engine_.reduce(id, qty);
            case Phase::PreClose: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::ClosingLockOut: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::ClosingFreeze: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::PostMarket: return engine_.reduce(id, qty);
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
    void on_replace(Order order, std::uint64_t replaced) {
        switch (phase_) {
            case Phase::Closed: on_refused_(order); return;
            case Phase::PreOpen: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLockIn: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLockOut: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLate: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::Continuous: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::Halted: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::Quoting: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::PreClose: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::ClosingLockOut: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::ClosingFreeze: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::PostMarket: if (order.lifetime == matching::Lifetime::Session) { on_refused_(order, Reason::Phase); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.on_new_order(order); engine_.replaced(order); return;
        }
    }

    // PreOpen: Closed to PreOpen
    bool preopen() {
        if (moved(Phase::Closed, Phase::PreOpen)) {
            on_announced_(Announcement::StartOfMessages);
            on_announced_(Announcement::StartOfSystemHours);
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // OpeningLockIn: PreOpen to OpeningLockIn
    bool openinglockin() {
        if (moved(Phase::PreOpen, Phase::OpeningLockIn)) {
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Open: PreOpen to Continuous, OpeningLockIn to Continuous, OpeningLockOut to Continuous, OpeningLate to Continuous
    bool open() {
        if (moved(Phase::PreOpen, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_)); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            return true;
        }
        if (moved(Phase::OpeningLockIn, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_)); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            return true;
        }
        if (moved(Phase::OpeningLockOut, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_)); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            return true;
        }
        if (moved(Phase::OpeningLate, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_)); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            return true;
        }
        return false;
    }

    // OpeningLockOut: OpeningLockIn to OpeningLockOut
    bool openinglockout() {
        if (moved(Phase::OpeningLockIn, Phase::OpeningLockOut)) {
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // OpeningLate: OpeningLockOut to OpeningLate
    bool openinglate() {
        if (moved(Phase::OpeningLockOut, Phase::OpeningLate)) {
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Halt: Continuous to Halted
    bool halt() {
        if (moved(Phase::Continuous, Phase::Halted)) {
            on_announced_(Announcement::Halted);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // PreClose: Continuous to PreClose, Halted to PreClose, Quoting to PreClose
    bool preclose() {
        if (moved(Phase::Continuous, Phase::PreClose)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        if (moved(Phase::Halted, Phase::PreClose)) {
            on_announced_(Announcement::Resumed);
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        if (moved(Phase::Quoting, Phase::PreClose)) {
            on_announced_(Announcement::Resumed);
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Quote: Halted to Quoting
    bool quote() {
        if (moved(Phase::Halted, Phase::Quoting)) {
            on_announced_(Announcement::Quoting);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Resume: Halted to Continuous, Quoting to Continuous
    bool resume() {
        if (moved(Phase::Halted, Phase::Continuous)) {
            crossed(Cross::Intraday, engine_.uncross(Cross::Intraday, reference_, floor_, ceiling_)); engine_.cancel_pending();
            engine_.ended(Cross::Intraday);
            engine_.expire(matching::Lifetime::Immediate);
            on_announced_(Announcement::Resumed);
            return true;
        }
        if (moved(Phase::Quoting, Phase::Continuous)) {
            crossed(Cross::Intraday, engine_.uncross(Cross::Intraday, reference_, floor_, ceiling_)); engine_.cancel_pending();
            engine_.ended(Cross::Intraday);
            engine_.expire(matching::Lifetime::Immediate);
            on_announced_(Announcement::Resumed);
            return true;
        }
        return false;
    }

    // ClosingLockOut: PreClose to ClosingLockOut
    bool closinglockout() {
        if (moved(Phase::PreClose, Phase::ClosingLockOut)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Close: PreClose to PostMarket, ClosingLockOut to PostMarket, ClosingFreeze to PostMarket
    bool close() {
        if (moved(Phase::PreClose, Phase::PostMarket)) {
            on_announced_(Announcement::EndOfMarketHours);
            close_ = closed(crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_, ceiling_))); engine_.cancel_pending();
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            return true;
        }
        if (moved(Phase::ClosingLockOut, Phase::PostMarket)) {
            on_announced_(Announcement::EndOfMarketHours);
            close_ = closed(crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_, ceiling_))); engine_.cancel_pending();
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            return true;
        }
        if (moved(Phase::ClosingFreeze, Phase::PostMarket)) {
            on_announced_(Announcement::EndOfMarketHours);
            close_ = closed(crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_, ceiling_))); engine_.cancel_pending();
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            return true;
        }
        return false;
    }

    // ClosingFreeze: ClosingLockOut to ClosingFreeze
    bool closingfreeze() {
        if (moved(Phase::ClosingLockOut, Phase::ClosingFreeze)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // End: PostMarket to Closed
    bool end() {
        if (moved(Phase::PostMarket, Phase::Closed)) {
            on_announced_(Announcement::EndOfSystemHours);
            engine_.expire(matching::Lifetime::Extended);
            engine_.expire(matching::Lifetime::Timed);
            on_announced_(Announcement::EndOfMessages);
            return true;
        }
        return false;
    }

private:
    bool moved(Phase from, Phase to) {
        if (phase_ != from) return false;
        phase_ = to;
        ++phases_;
        return true;
    }

    // An order for one cross alone, which the book never shows: given to its cross while that
    // collects, held aside until it does, and refused once it is done for the day.
    void alone(Order order) {
        order.shown = false;
        if (engine_.done(order.only)) { on_refused_(order); return; }
        if (collects(order.only)) { engine_.hold(order); return; }
        engine_.wait(order);
    }

    // Does the phase the day is in collect for this cross?
    bool collects(Cross cross) const {
        switch (phase_) {
            case Phase::PreOpen: return cross == Cross::Opening;
            case Phase::OpeningLockIn: return cross == Cross::Opening;
            case Phase::OpeningLockOut: return cross == Cross::Opening;
            case Phase::OpeningLate: return cross == Cross::Opening;
            case Phase::Halted: return cross == Cross::Intraday;
            case Phase::Quoting: return cross == Cross::Intraday;
            case Phase::PreClose: return cross == Cross::Closing;
            case Phase::ClosingLockOut: return cross == Cross::Closing;
            case Phase::ClosingFreeze: return cross == Cross::Closing;
            default: return false;
        }
    }

    // An uncross the owner is told of: which auction it ended, what it crossed at, and the
    // match number of its print, numbered after its fills - one that crossed nothing too, as
    // the venue numbers it.
    Indicative crossed(Cross cross, Indicative at) {
        on_crossed_(cross, at, engine_.numbered());
        return at;
    }

    // With nothing crossed in the closing auction the reference stands; a venue that
    // closes on its last trade sets the reference to it first.
    std::int64_t closed(Indicative at) const { return at.qty > 0 ? at.price : reference_; }

    Engine&       engine_;
    OnRefused     on_refused_;
    OnAnnounced   on_announced_;
    OnCrossed     on_crossed_;
    OnIndicated   on_indicated_;
    Phase         phase_ = Phase::Closed;
    std::uint64_t phases_ = 0;
    std::int64_t  reference_ = 0;
    std::int64_t  floor_ = std::numeric_limits<std::int64_t>::min();
    std::int64_t  ceiling_ = std::numeric_limits<std::int64_t>::max();
    std::int64_t  close_ = 0;
    std::int64_t  now_ = 0;   // the time the owner last handed, in nanoseconds
};
}
