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

    static constexpr std::array<Scheduled, 7> schedule{ {
        Scheduled{ PhaseChange::PreOpen, 0 },
        Scheduled{ PhaseChange::Open, 4 },
        Scheduled{ PhaseChange::Halt, 8 },
        Scheduled{ PhaseChange::Resume, 10 },
        Scheduled{ PhaseChange::PreClose, 14 },
        Scheduled{ PhaseChange::Close, 18 },
        Scheduled{ PhaseChange::End, 20 }
    } };

    // The seconds after a served venue starts that each scheduled change is asked for.
    using Times = std::array<int, schedule.size()>;

    explicit Day(Emit emit) : emit_(std::move(emit)) {}

    // Ask the engine for a change now: the startup run, between its turns.
    void request(PhaseChange change) { sim::emit_phase_request(emit_, change); }

    // Serve the day on the clock, from now. The component's thread reads the times only once it
    // sees it is serving, so they may be handed over while that thread is already running.
    void serve(const Times& at) {
        at_ = at;
        started_ = std::chrono::steady_clock::now();
        serving_.store(true, std::memory_order_release);
    }

    // One pass: the next scheduled change, when its time has come. False when none was due, which
    // is what tells a component thread to back off.
    bool step() {
        if (!serving_.load(std::memory_order_acquire) || next_ == schedule.size()) return false;
        if (std::chrono::steady_clock::now() - started_ < std::chrono::seconds(at_[next_])) return false;
        request(schedule[next_++].change);
        return true;
    }

private:
    Emit                                  emit_;
    Times                                 at_{};
    std::chrono::steady_clock::time_point started_{};
    std::atomic<bool>                     serving_{false};
    std::size_t                           next_ = 0;
};

}