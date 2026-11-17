#pragma once

// Generated from the Iex Equities Session engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <cstdint>
#include <limits>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Closed, PreMarket, OpeningLockIn, OpeningLockOut, Continuous, Halted, ClosingAuction, ClosingLockOut, PostMarket };
// Why a phase refuses an order, where the chart says: each word its refusals give.
enum class Reason : std::uint8_t { Lifetime, Type, Phase };

// What the venue announces as its phases change. The owner publishes each
// through every feed it serves, as that feed's own markings say it.
enum class Announcement : std::uint8_t { StartOfMessages, StartOfSystemHours, StartOfMarketHours, Halted, Resumed, EndOfMarketHours, EndOfSystemHours, EndOfMessages };

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
    bool collecting() const { return phase_ == Phase::PreMarket || phase_ == Phase::OpeningLockIn || phase_ == Phase::OpeningLockOut || phase_ == Phase::Halted || phase_ == Phase::ClosingAuction || phase_ == Phase::ClosingLockOut; }

    // What the auction being collected for would cross at now, told to the owner with which
    // auction it is; nothing while none is collecting.
    void indicate() {
        switch (phase_) {
            case Phase::PreMarket: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::OpeningLockIn: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::OpeningLockOut: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::Halted: on_indicated_(Cross::Intraday, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::ClosingAuction: on_indicated_(Cross::Closing, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::ClosingLockOut: on_indicated_(Cross::Closing, engine_.indicative(reference_, floor_, ceiling_)); return;
            default: return;
        }
    }

    void on_new_order(Order order) {
        switch (phase_) {
            case Phase::Closed: on_refused_(order); return;
            case Phase::PreMarket: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Routed && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime != matching::Lifetime::Session) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None) && order.lifetime == matching::Lifetime::Session) { engine_.hold(order); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.lifetime == matching::Lifetime::Session) { engine_.hold(order); return; } if ((order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Immediate) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Whole) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Hours) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLockIn: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only == Cross::Opening && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Routed && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime != matching::Lifetime::Session) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None) && order.lifetime == matching::Lifetime::Session) { engine_.hold(order); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if ((order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.lifetime == matching::Lifetime::Session) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Whole) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Hours) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLockOut: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only == Cross::Opening) { on_refused_(order, Reason::Phase); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Routed && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime != matching::Lifetime::Session) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None) && order.lifetime == matching::Lifetime::Session) { engine_.hold(order); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if ((order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.lifetime == matching::Lifetime::Session) { on_refused_(order, Reason::Phase); return; } if (order.lifetime == matching::Lifetime::Extended) { on_refused_(order, Reason::Phase); return; } engine_.on_new_order(order); return;
            case Phase::Continuous: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.on_new_order(order); return; } if ((order.price != market(order.side) && order.peg == matching::Peg::None)) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Hours) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Timed) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
            case Phase::Halted: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if ((order.price != market(order.side) && order.peg == matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Extended) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Hours) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Timed) { on_refused_(order, Reason::Lifetime); return; } engine_.hold(order); return;
            case Phase::ClosingAuction: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only == Cross::Closing && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.on_new_order(order); return; } if ((order.price != market(order.side) && order.peg == matching::Peg::None)) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Hours) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Timed) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
            case Phase::ClosingLockOut: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only == Cross::Closing) { on_refused_(order, Reason::Phase); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.on_new_order(order); return; } if ((order.price != market(order.side) && order.peg == matching::Peg::None)) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Hours) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Timed) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
            case Phase::PostMarket: if (order.only != Cross::None && order.instruction != matching::Instruction::Unrouted) { on_refused_(order, Reason::Lifetime); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary) { on_refused_(order, Reason::Type); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if ((order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.lifetime == matching::Lifetime::Session) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
        }
    }

    // A cancel, answering whether the order was in the book to take out. A phase
    // that refuses cancels holds nothing to take, so it answers no.
    bool on_cancel(std::uint64_t id) {
        switch (phase_) {
            case Phase::Closed: return false;
            case Phase::PreMarket: return engine_.on_cancel(id);
            case Phase::OpeningLockIn: return engine_.on_cancel_trading(id);   // what the auction holds is locked in
            case Phase::OpeningLockOut: return engine_.on_cancel_trading(id);   // what the auction holds is locked in
            case Phase::Continuous: return engine_.on_cancel(id);
            case Phase::Halted: return engine_.on_cancel(id);
            case Phase::ClosingAuction: return engine_.on_cancel_trading(id);   // what the auction holds is locked in
            case Phase::ClosingLockOut: return engine_.on_cancel_trading(id);   // what the auction holds is locked in
            case Phase::PostMarket: return engine_.on_cancel(id);
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
            case Phase::PreMarket: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLockIn: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLockOut: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::Continuous: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::Halted: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.hold(order); engine_.replaced(order); return;
            case Phase::ClosingAuction: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::ClosingLockOut: if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::PostMarket: if (order.lifetime == matching::Lifetime::Session) { on_refused_(order, Reason::Lifetime); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.on_new_order(order); engine_.replaced(order); return;
        }
    }

    // PreOpen: Closed to PreMarket
    bool preopen() {
        if (moved(Phase::Closed, Phase::PreMarket)) {
            on_announced_(Announcement::StartOfMessages);
            on_announced_(Announcement::StartOfSystemHours);
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // OpeningLockIn: PreMarket to OpeningLockIn
    bool openinglockin() {
        if (moved(Phase::PreMarket, Phase::OpeningLockIn)) {
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Open: PreMarket to Continuous, OpeningLockIn to Continuous, OpeningLockOut to Continuous
    bool open() {
        if (moved(Phase::PreMarket, Phase::Continuous)) {
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_));
            engine_.ended(Cross::Opening);
            engine_.eliminate_market();
            on_announced_(Announcement::StartOfMarketHours);
            return true;
        }
        if (moved(Phase::OpeningLockIn, Phase::Continuous)) {
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_));
            engine_.ended(Cross::Opening);
            engine_.eliminate_market();
            on_announced_(Announcement::StartOfMarketHours);
            return true;
        }
        if (moved(Phase::OpeningLockOut, Phase::Continuous)) {
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_));
            engine_.ended(Cross::Opening);
            engine_.eliminate_market();
            on_announced_(Announcement::StartOfMarketHours);
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

    // Halt: Continuous to Halted
    bool halt() {
        if (moved(Phase::Continuous, Phase::Halted)) {
            on_announced_(Announcement::Halted);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // PreClose: Continuous to ClosingAuction
    bool preclose() {
        if (moved(Phase::Continuous, Phase::ClosingAuction)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Resume: Halted to Continuous
    bool resume() {
        if (moved(Phase::Halted, Phase::Continuous)) {
            crossed(Cross::Intraday, engine_.uncross(Cross::Intraday, reference_, floor_, ceiling_));
            engine_.ended(Cross::Intraday);
            on_announced_(Announcement::Resumed);
            return true;
        }
        return false;
    }

    // ClosingLockOut: ClosingAuction to ClosingLockOut
    bool closinglockout() {
        if (moved(Phase::ClosingAuction, Phase::ClosingLockOut)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Close: ClosingAuction to PostMarket, ClosingLockOut to PostMarket
    bool close() {
        if (moved(Phase::ClosingAuction, Phase::PostMarket)) {
            close_ = closed(crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_, ceiling_)));
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_instructed(matching::Instruction::Discretionary);
            on_announced_(Announcement::EndOfMarketHours);
            return true;
        }
        if (moved(Phase::ClosingLockOut, Phase::PostMarket)) {
            close_ = closed(crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_, ceiling_)));
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_instructed(matching::Instruction::Discretionary);
            on_announced_(Announcement::EndOfMarketHours);
            return true;
        }
        return false;
    }

    // End: PostMarket to Closed
    bool end() {
        if (moved(Phase::PostMarket, Phase::Closed)) {
            engine_.expire(matching::Lifetime::Extended);
            engine_.expire(matching::Lifetime::Hours);
            engine_.expire(matching::Lifetime::Timed);
            on_announced_(Announcement::EndOfSystemHours);
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
            case Phase::PreMarket: return cross == Cross::Opening;
            case Phase::OpeningLockIn: return cross == Cross::Opening;
            case Phase::OpeningLockOut: return cross == Cross::Opening;
            case Phase::Halted: return cross == Cross::Intraday;
            case Phase::ClosingAuction: return cross == Cross::Closing;
            case Phase::ClosingLockOut: return cross == Cross::Closing;
            default: return false;
        }
    }

    // An uncross the owner is told of: which auction it ended, what it crossed at, and the
    // match number of its print, numbered after its fills; one that crossed nothing made no
    // execution to number.
    Indicative crossed(Cross cross, Indicative at) {
        on_crossed_(cross, at, at.qty > 0 ? engine_.numbered() : 0);
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
