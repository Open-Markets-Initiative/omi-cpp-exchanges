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
        Indication{ 20, 1, 0, 0 },   // Opening
        Indication{ 0, 1, 0, 0 },   // Intraday
        Indication{ 0, 1, 0, 0 }    // Closing
    } };

    // A change of the declared day, and the turn of the startup run it is asked for before (a turn
    // past the last is after the run).
    // Where it starts a collection the day asks about, which indication it asks by.
    struct Scheduled {
        PhaseChange change;
        int         tick;
        int         indication;   // into indications, or -1 where it starts no collection asked about
    };

    static constexpr std::array<Scheduled, 7> schedule{ {
        Scheduled{ PhaseChange::PreOpen, 0, 0 },
        Scheduled{ PhaseChange::Open, 4, -1 },
        Scheduled{ PhaseChange::Halt, 8, 1 },
        Scheduled{ PhaseChange::Resume, 10, -1 },
        Scheduled{ PhaseChange::PreClose, 14, 2 },
        Scheduled{ PhaseChange::Close, 18, -1 },
        Scheduled{ PhaseChange::End, 20, -1 }
    } };

    // The seconds after a served venue starts that each scheduled change is asked for.
    using Times = std::array<int, schedule.size()>;

    explicit Day(Emit emit) : emit_(std::move(emit)) {}

    // Ask the engine for a scheduled change now: the startup run, between its turns, asks for them
    // in the order they are scheduled.
    void request(const Scheduled& scheduled) {
        sim::emit_phase_request(emit_, scheduled.change);
        asking_ = scheduled.indication;
    }

    // Ask what the auction being collected for would cross at, where the change made last started a
    // collection the day asks about: the startup run, once a turn.
    void indicate() {
        if (asking_ >= 0) sim::emit_imbalance_request(emit_);
    }

    // Serve the day on the clock, from now. The component's thread reads the times only once it
    // sees it is serving, so they may be handed over while that thread is already running.
    void serve(const Times& at) {
        at_ = at;
        started_ = std::chrono::steady_clock::now();
        serving_.store(true, std::memory_order_release);
    }

    // How many of the scheduled changes it has asked for on the clock.
    std::size_t asked() const { return next_; }

    // One pass: the next scheduled change when its time has come, else the auction collecting asked
    // about when that is due. False when nothing was due, which is what tells a component thread to
    // back off.
    bool step() {
        if (!serving_.load(std::memory_order_acquire)) return false;
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started_).count();

        if (next_ < schedule.size() && now >= at_[next_] * 1000LL) {
            const auto made = next_++;
            request(schedule[made]);
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
    std::chrono::steady_clock::time_point started_{};
    std::atomic<bool>                     serving_{false};
    std::size_t                           next_ = 0;
    int                                   asking_ = -1;   // the indication the collection now is asked about by, or -1
    std::int64_t                          begun_ = 0;     // when it began, in milliseconds after the venue started
    std::int64_t                          ends_ = 0;      // when the next change ends it
    std::int64_t                          due_ = 0;       // when it is next asked about
};

}