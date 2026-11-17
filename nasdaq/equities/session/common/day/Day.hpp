#pragma once

#include "protocols/ipc/IpcBus.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

namespace sim::components {

// The trading day as a component: the venue's changes of phase, each asked of the engine over the
// Ipc as a Phase Request through `emit`.
template <typename Emit>
class Day {
public:
    // How often an auction is asked about while it collects for it, in seconds: first `after` the
    // collection starts, then every `every`, and every `final` over the last `final_percent` of the
    // collection. A final of zero never quickens.
    struct Indication {
        int after;
        int every;
        int final;
        int final_percent;
    };

    static constexpr std::array<Indication, 3> indications{ {
        Indication{ 0, 10, 1, 40 },   // Opening
        Indication{ 1, 1, 0, 0 },   // Intraday
        Indication{ 0, 10, 1, 40 }    // Closing
    } };

    // A change of the declared day, and the turn of the startup run it is asked for before (a turn
    // past the last is after the run).
    // Where it starts a collection the day asks about, which indication it asks by.
    struct Scheduled {
        PhaseChange change;
        int         tick;
        std::string_view reason;   // why the change is made, as the day declares it; nothing where it declares none
        int         indication;   // into indications, or -1 where it starts no collection asked about
    };

    static constexpr std::array<Scheduled, 13> schedule{ {
        Scheduled{ PhaseChange::PreOpen, 0, "", 0 },
        Scheduled{ PhaseChange::OpeningLockIn, 2, "", 0 },
        Scheduled{ PhaseChange::OpeningLockOut, 3, "", 0 },
        Scheduled{ PhaseChange::OpeningLate, 4, "", 0 },
        Scheduled{ PhaseChange::Open, 4, "", -1 },
        Scheduled{ PhaseChange::Halt, 8, "T1", 1 },
        Scheduled{ PhaseChange::Quote, 9, "T3", 1 },
        Scheduled{ PhaseChange::Resume, 10, "", -1 },
        Scheduled{ PhaseChange::PreClose, 14, "", 2 },
        Scheduled{ PhaseChange::ClosingLockOut, 16, "", 2 },
        Scheduled{ PhaseChange::ClosingFreeze, 17, "", 2 },
        Scheduled{ PhaseChange::Close, 18, "", -1 },
        Scheduled{ PhaseChange::End, 20, "", -1 }
    } };

    // The seconds after a served venue starts that each scheduled change is asked for.
    using Times = std::array<int, schedule.size()>;

    explicit Day(Emit emit) : emit_(std::move(emit)) {}

    // Ask the engine for a scheduled change now: the startup run, between its turns, asks for them
    // in the order they are scheduled.
    void request(const Scheduled& scheduled) {
        sim::emit_phase_request(emit_, scheduled.change, scheduled.reason);
        asking_ = scheduled.indication;
        if (books_) sim::emit_book_request(emit_);   // and for its book as the change leaves it
    }

    // Whether the engine is asked for its own book after each change of phase the day asks for: where
    // the venue's settings say [book] every = phase. Anything on the Ipc may ask at any other time.
    void books(bool every_phase) { books_ = every_phase; }

    // And every so many milliseconds of the day it serves, where its settings give a number: a book
    // asked for through continuous trading, not only as the phase changes. Nothing after the day's
    // last change.
    void books_each(long long milliseconds) { book_every_ = milliseconds > 0 ? milliseconds : 0; }

    // Ask the engine for its book now.
    void book() { sim::emit_book_request(emit_); }
    // Ask what the auction being collected for would cross at, where the change made last started a
    // collection the day asks about: the startup run, once a turn.
    void indicate() {
        if (asking_ >= 0) sim::emit_imbalance_request(emit_);
    }

    // Serve the day on the clock, from now. The component's thread reads the times only once it
    // sees it is serving, so they may be handed over while that thread is already running.
    void serve(const Times& at) {
        at_ = at;
        sim::clock::start_day();
        serving_.store(true, std::memory_order_release);
    }

    // The venue's time sent the engine now: the startup run, which keeps no time, sends it once a
    // turn where the venue's engine tells something by it.
    void clock() { sim::emit_clock(emit_); }

    // How often the venue's time is sent the engine, in milliseconds, as its settings give it: a
    // timer comes due by the first time sent at or after it, so a timer is late by up to this much.
    void every(std::int64_t milliseconds) {
        if (milliseconds > 0) {
            every_ = milliseconds;
        }
    }

    // How many of the scheduled changes it has asked for on the clock.
    std::size_t asked() const { return next_; }

    // One pass: the next scheduled change when its time has come, else the auction collecting asked
    // about when that is due. False when nothing was due, which is what tells a component thread to
    // back off.
    bool step() {
        if (book_every_ > 0 && serving_.load(std::memory_order_acquire) && next_ < schedule.size()) {
            const long long booking = sim::clock::day_ms();   // the engine's book, each so many milliseconds of the day
            if (booking >= booked_) { booked_ = booking - booking % book_every_ + book_every_; book(); return true; }
        }
        if (!serving_.load(std::memory_order_acquire)) return false;
        const auto now = sim::clock::day_ms();   // the day's time, as the venue's clock keeps it
        if (now >= clocked_) {   // the venue's time, each second, which the timers its day sets come due by
            sim::emit_clock(emit_);
            clocked_ = now - now % every_ + every_;
            return true;
        }

        if (next_ < schedule.size() && now >= at_[next_] * 1000LL) {
            const auto made = next_++;
            const int was = asking_;
            request(schedule[made]);
            if (asking_ == was && (asking_ == 0 || asking_ == 2)) {   // the same collection goes on through the change
                ends_ = next_ < schedule.size() ? at_[next_] * 1000LL : std::numeric_limits<std::int64_t>::max();
                return true;
            }
            begun_ = at_[made] * 1000LL;
            ends_ = next_ < schedule.size() ? at_[next_] * 1000LL : std::numeric_limits<std::int64_t>::max();
            if (asking_ >= 0) due_ = begun_ + indications[static_cast<std::size_t>(asking_)].after * 1000LL;
            return true;
        }

        if (asking_ < 0 || due_ >= ends_ || now < due_) return false;
        sim::emit_imbalance_request(emit_);
        due_ = following(due_);
        return true;
    }

private:
    // When to ask next after asking at `asked`, in milliseconds after the venue started: every
    // `every`, and every `final` once into the last part of the collection, which the slower asking
    // stops at rather than passes.
    std::int64_t following(std::int64_t asked) const {
        const auto& indication = indications[static_cast<std::size_t>(asking_)];
        const auto quickens = indication.final > 0 ? ends_ - (ends_ - begun_) * indication.final_percent / 100 : ends_;
        if (asked >= quickens) return asked + indication.final * 1000LL;
        const auto next = asked + indication.every * 1000LL;
        return next > quickens ? quickens : next;
    }

    Emit                                  emit_;
    Times                                 at_{};
    std::atomic<bool>                     serving_{false};
    std::size_t                           next_ = 0;
    bool                                  books_ = false;   // does it ask for the engine's book after each change
    long long                             book_every_ = 0;   // and every so many milliseconds of the day; 0 for not
    long long                             booked_ = 0;   // the day's time the next of those is due
    int                                   asking_ = -1;   // the indication the collection now is asked about by, or -1
    std::int64_t                          begun_ = 0;     // when it began, in milliseconds after the venue started
    std::int64_t                          ends_ = 0;      // when the next change ends it
    std::int64_t                          due_ = 0;       // when it is next asked about
    std::int64_t                          clocked_ = 0;      // when the venue's time is next sent, in milliseconds after the day began
    std::int64_t                          every_ = 1000;     // how often it is sent, in milliseconds, unless the venue's settings say otherwise
};

}