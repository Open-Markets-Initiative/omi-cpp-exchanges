#pragma once

// Generated from the Odx Security Token Open engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <cstdint>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Closed, Continuous };

// What the venue announces as its phases change. The owner publishes each
// through every feed it serves, as that feed's own markings say it.
enum class Announcement : std::uint8_t { StartOfMessages, Trading };

// The trading day. An order is given to the phase the venue is in, which
// crosses it, takes it out of the book, or refuses it, as the chart says.
template <typename Engine, typename OnRefused, typename OnAnnounced>
class TradingPhases {
public:
    TradingPhases(Engine& engine, OnRefused on_refused, OnAnnounced on_announced)
        : engine_(engine), on_refused_(on_refused), on_announced_(on_announced) {}

    Phase phase() const { return phase_; }

    // How many phase changes the day has been through: a venue that halted and
    // resumed is not one that never opened.
    std::uint64_t phases() const { return phases_; }

    void on_new_order(Order order) {
        switch (phase_) {
            case Phase::Closed: on_refused_(order); return;
            case Phase::Continuous: if (order.price == market(order.side)) { on_refused_(order); return; } engine_.on_new_order(order); return;
        }
    }

    // A cancel, answering whether the order was in the book to take out. A phase
    // that refuses cancels holds nothing to take, so it answers no.
    bool on_cancel(std::uint64_t id) {
        switch (phase_) {
            case Phase::Closed: return engine_.on_cancel(id);
            case Phase::Continuous: return engine_.on_cancel(id);
        }
        return false;
    }

    // Start: Closed to Continuous
    bool start() {
        if (moved(Phase::Closed, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMessages);
            on_announced_(Announcement::Trading);
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

    Engine&       engine_;
    OnRefused     on_refused_;
    OnAnnounced   on_announced_;
    Phase         phase_ = Phase::Closed;
    std::uint64_t phases_ = 0;
};
}
