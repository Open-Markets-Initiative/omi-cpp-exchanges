#pragma once

// Generated from the Odx Security Token Session engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <cstdint>
#include <limits>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Closed, Started, PreOpen, PreOpenNcp, Break, PreClose, PreCloseNcp, PostClose };

// What the venue announces as its phases change. The owner publishes each
// through every feed it serves, as that feed's own markings say it.
enum class Announcement : std::uint8_t { StartOfMessages, Trading, EndOfMessages };

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

    // The price an uncross is drawn towards when two prices would cross as much: the
    // orderbook's reference, which the owner sets from what the venue publishes.
    void reference(std::int64_t price) { reference_ = price; }

    // The prices an uncross may cross at, the owner's to set: the venue's own limits
    // where it states them, else what its rules put around the reference.
    void band(std::int64_t floor, std::int64_t ceiling) { floor_ = floor; ceiling_ = ceiling; }

    // What the closing auction crossed at, which trade at last trades at.
    std::int64_t closing_price() const { return close_; }

    // Is an auction collecting? An owner publishes the indicative price while one is.
    bool collecting() const { return phase_ == Phase::PreOpen || phase_ == Phase::PreOpenNcp || phase_ == Phase::PreClose || phase_ == Phase::PreCloseNcp; }

    // What the auction being collected for would cross at now, told to the owner with which
    // auction it is; nothing while none is collecting.
    void indicate() {
        switch (phase_) {
            case Phase::PreOpen: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::PreOpenNcp: on_indicated_(Cross::Opening, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::PreClose: on_indicated_(Cross::Closing, engine_.indicative(reference_, floor_, ceiling_)); return;
            case Phase::PreCloseNcp: on_indicated_(Cross::Closing, engine_.indicative(reference_, floor_, ceiling_)); return;
            default: return;
        }
    }

    void on_new_order(Order order) {
        switch (phase_) {
            case Phase::Closed: on_refused_(order); return;
            case Phase::Started: on_refused_(order); return;
            case Phase::PreOpen: engine_.hold(order); return;
            case Phase::PreOpenNcp: engine_.hold(order); return;
            case Phase::Break: on_refused_(order); return;
            case Phase::PreClose: engine_.hold(order); return;
            case Phase::PreCloseNcp: engine_.hold(order); return;
            case Phase::PostClose: on_refused_(order); return;
        }
    }

    // A cancel, answering whether the order was in the book to take out. A phase
    // that refuses cancels holds nothing to take, so it answers no.
    bool on_cancel(std::uint64_t id) {
        switch (phase_) {
            case Phase::Closed: return false;
            case Phase::Started: return false;
            case Phase::PreOpen: return engine_.on_cancel(id);
            case Phase::PreOpenNcp: return false;
            case Phase::Break: return false;
            case Phase::PreClose: return engine_.on_cancel(id);
            case Phase::PreCloseNcp: return false;
            case Phase::PostClose: return false;
        }
        return false;
    }

    // Start: Closed to Started
    bool start() {
        if (moved(Phase::Closed, Phase::Started)) {
            on_announced_(Announcement::StartOfMessages);
            on_announced_(Announcement::Trading);
            return true;
        }
        return false;
    }

    // PreOpen: Started to PreOpen
    bool preopen() {
        if (moved(Phase::Started, Phase::PreOpen)) return true;
        return false;
    }

    // NoCancel: PreOpen to PreOpenNcp, PreClose to PreCloseNcp
    bool nocancel() {
        if (moved(Phase::PreOpen, Phase::PreOpenNcp)) return true;
        if (moved(Phase::PreClose, Phase::PreCloseNcp)) return true;
        return false;
    }

    // Open: PreOpen to Break, PreOpenNcp to Break
    bool open() {
        if (moved(Phase::PreOpen, Phase::Break)) {
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_));
            return true;
        }
        if (moved(Phase::PreOpenNcp, Phase::Break)) {
            crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_, ceiling_));
            return true;
        }
        return false;
    }

    // PreClose: Break to PreClose
    bool preclose() {
        if (moved(Phase::Break, Phase::PreClose)) return true;
        return false;
    }

    // Close: PreClose to PostClose, PreCloseNcp to PostClose
    bool close() {
        if (moved(Phase::PreClose, Phase::PostClose)) {
            close_ = closed(crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_, ceiling_)));
            engine_.expire(matching::Lifetime::Session);
            return true;
        }
        if (moved(Phase::PreCloseNcp, Phase::PostClose)) {
            close_ = closed(crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_, ceiling_)));
            engine_.expire(matching::Lifetime::Session);
            return true;
        }
        return false;
    }

    // End: PostClose to Closed
    bool end() {
        if (moved(Phase::PostClose, Phase::Closed)) {
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
};
}
