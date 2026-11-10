#pragma once

// Generated from the Nasdaq Equities Open engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <cstdint>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Continuous };

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

    void on_new_order(Order order) {
        switch (phase_) {
            case Phase::Continuous: engine_.on_new_order(order); return;
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
    void on_replace(Order order) {
        switch (phase_) {
            case Phase::Continuous: if (!engine_.on_cancel(order.id)) { on_refused_(order); return; } engine_.on_new_order(order); return;
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
};
}
