#pragma once

#include "protocols/ipc/IpcBus.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <utility>

namespace sim::components {

// The trading day as a component: the venue's changes of phase, each asked of the engine over the
// Ipc as a Phase Request through `emit`.
template <typename Emit>
class Day {
public:
    // A change of the declared day, and the turn of the startup run it is asked for before (a turn
    // past the last is after the run).
    struct Scheduled {
        PhaseChange change;
        int         tick;
    };

    static constexpr std::array<Scheduled, 1> schedule{ {
        Scheduled{ PhaseChange::Start, 0 }
    } };

    // The seconds after a served venue starts that each scheduled change is asked for.
    using Times = std::array<int, schedule.size()>;

    explicit Day(Emit emit) : emit_(std::move(emit)) {}

    // Ask the engine for a scheduled change now: the startup run, between its turns, asks for them
    // in the order they are scheduled.
    void request(const Scheduled& scheduled) {
        sim::emit_phase_request(emit_, scheduled.change);
    }

    // Serve the day on the clock, from now. The component's thread reads the times only once it
    // sees it is serving, so they may be handed over while that thread is already running.
    void serve(const Times& at) {
        at_ = at;
        sim::clock::start_day();
        serving_.store(true, std::memory_order_release);
    }

    // How many of the scheduled changes it has asked for on the clock.
    std::size_t asked() const { return next_; }

    // One pass: the next scheduled change, when its time has come. False when none was due, which
    // is what tells a component thread to back off.
    bool step() {
        if (!serving_.load(std::memory_order_acquire) || next_ == schedule.size()) return false;
        if (sim::clock::day_ms() < at_[next_] * 1000LL) return false;   // the day's time, as the venue's clock keeps it
        request(schedule[next_++]);
        return true;
    }

private:
    Emit                                  emit_;
    Times                                 at_{};
    std::atomic<bool>                     serving_{false};
    std::size_t                           next_ = 0;
};

}