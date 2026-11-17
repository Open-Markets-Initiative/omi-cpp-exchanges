#pragma once

// Generated from the Iex Equities Open engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <cstdint>
#include <limits>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Continuous };
// Why a phase refuses an order, where the chart says: each word its refusals give.
enum class Reason : std::uint8_t { Type, Lifetime };

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
            case Phase::Continuous: if (order.instruction != matching::Instruction::Unrouted && order.instruction != matching::Instruction::Routed && !(order.price != market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.instruction != matching::Instruction::Unrouted && order.lifetime == matching::Lifetime::Whole) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Sweep && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::TradeAt && order.lifetime != matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.instruction == matching::Instruction::Discretionary && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Timed) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.on_new_order(order); return; } if ((order.price != market(order.side) && order.peg == matching::Peg::None)) { engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { on_refused_(order, Reason::Lifetime); return; } if (order.lifetime == matching::Lifetime::Hours) { on_refused_(order, Reason::Lifetime); return; } engine_.on_new_order(order); return;
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
            case Phase::Continuous: if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); engine_.on_new_order(order); engine_.replaced(order); return;
        }
    }

private:
    bool moved(Phase from, Phase to) {
        if (phase_ != from) return false;
        phase_ = to;
        ++phases_;
        return true;
    }

    Engine&       engine_;
    OnRefused     on_refused_;
    Phase         phase_ = Phase::Continuous;
    std::uint64_t phases_ = 0;
    std::int64_t  now_ = 0;   // the time the owner last handed, in nanoseconds
};
}
