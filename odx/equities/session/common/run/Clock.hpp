#pragma once

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <string_view>

// The venue's clock. How it keeps time is a setting:
//   Elapsed       the day's changes the seconds after it starts that its settings give, on the clock that
//                 never steps; every stamp the wall clock. A host that is paused falls behind its stamps.
//   Wall          the same, counted on the wall clock, so the stamps and the declared times agree.
//   SimulatedDay  the day starts at `start`, a time of day in `zone`, whenever the venue starts, and runs at
//                 real speed: its stamps and its changes both read that clock, so it reads as a trading day
//                 at any hour.
//   TimeOfDay     the day starts at `start` in `zone` today, by the wall clock, as a venue's does; started
//                 later, the changes already due are made at once, in order.
// One venue's processes agree on when its day began where SIM_CLOCK_ANCHOR gives it, in nanoseconds since
// the epoch, as its launcher sets it; without it, a process takes the moment it reads its settings.
namespace sim::clock {

enum class Mode { Elapsed, Wall, SimulatedDay, TimeOfDay };

namespace detail {

struct State {
    Mode mode = Mode::Elapsed;
    std::int64_t anchor_ns = 0;      // the wall time the day began: what SimulatedDay counts its time from
    std::int64_t simulated_ns = 0;   // the time SimulatedDay reads at anchor_ns: `start` on its first day
    std::int64_t day_start_ns = 0;   // the wall time the day starts, which Wall, SimulatedDay and TimeOfDay time it from
    std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();   // when an Elapsed day began
    bool kept = false;               // an Elapsed day carried on from when it began, which only the wall clock knows
};

inline State& state() {
    static State held;
    return held;
}

inline std::int64_t wall_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

// `start` seconds after midnight in `zone`, on the date `at` falls on there.
inline std::int64_t local_start(std::int64_t at, std::int64_t start, std::string_view zone) {
#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L
    const auto* place = std::chrono::locate_zone(zone);
    const auto local = place->to_local(std::chrono::sys_time<std::chrono::nanoseconds>(std::chrono::nanoseconds(at)));
    const auto begins = place->to_sys(std::chrono::floor<std::chrono::days>(local) + std::chrono::seconds(start), std::chrono::choose::earliest);
    return std::chrono::duration_cast<std::chrono::nanoseconds>(begins.time_since_epoch()).count();
#else
    (void)at; (void)start; (void)zone;
    throw std::runtime_error("clock: built without a time zone database (GCC 14 or later), so its mode is Elapsed or Wall");
#endif
}

// A time of day, HH:MM or HH:MM:SS, in seconds after midnight.
inline std::int64_t seconds_of_day(std::string_view text) {
    const auto refused = [&] { return std::runtime_error("clock: start '" + std::string(text) + "' is no time of day; it is HH:MM or HH:MM:SS"); };
    std::int64_t parts[3] = { 0, 0, 0 };
    std::size_t part = 0, digits = 0;
    for (const char c : text) {
        if (c == ':') {
            if (digits == 0 || ++part > 2) throw refused();
            digits = 0;
        } else if (c >= '0' && c <= '9' && digits < 2) {
            parts[part] = parts[part] * 10 + (c - '0');
            ++digits;
        } else {
            throw refused();
        }
    }
    if (part == 0 || digits == 0 || parts[0] > 23 || parts[1] > 59 || parts[2] > 59) throw refused();
    return parts[0] * 3600 + parts[1] * 60 + parts[2];
}

inline Mode mode_of(std::string_view mode) {
    if (mode == "Elapsed") return Mode::Elapsed;
    if (mode == "Wall") return Mode::Wall;
    if (mode == "SimulatedDay") return Mode::SimulatedDay;
    if (mode == "TimeOfDay") return Mode::TimeOfDay;
    throw std::runtime_error("clock: mode '" + std::string(mode) + "' is none of Elapsed, Wall, SimulatedDay, TimeOfDay");
}

}  // namespace detail

// How the venue keeps time, as its settings say: set once, before anything is stamped.
inline void use(std::string_view mode, std::string_view start, std::string_view zone) {
    auto& held = detail::state();
    held.mode = detail::mode_of(mode);
    const auto begins = detail::seconds_of_day(start);
    const char* anchor = std::getenv("SIM_CLOCK_ANCHOR");
    held.anchor_ns = anchor != nullptr && *anchor != '\0' ? std::stoll(anchor) : detail::wall_ns();
    held.day_start_ns = held.anchor_ns;
    held.started = std::chrono::steady_clock::now();
    if (held.mode == Mode::SimulatedDay) held.simulated_ns = detail::local_start(held.anchor_ns, begins, zone);
    if (held.mode == Mode::TimeOfDay) held.day_start_ns = detail::local_start(detail::wall_ns(), begins, zone);
}

// Now, as every message the venue sends is stamped: nanoseconds since the epoch.
inline std::int64_t now_nanos() {
    const auto& held = detail::state();
    const auto wall = detail::wall_ns();
    return held.mode == Mode::SimulatedDay ? held.simulated_ns + (wall - held.anchor_ns) : wall;
}

// What the venue's FIX sessions are told the time is: the same clock.
struct Nanos {
    std::int64_t operator()() const { return now_nanos(); }
};

// The served day starts now: an Elapsed day from now; any other from when the clock says it does.
inline void start_day() {
    auto& held = detail::state();
    held.started = std::chrono::steady_clock::now();
    held.kept = false;
}

// The served day carries on from when it began, kept across a restart of the venue.
inline void start_day(std::chrono::system_clock::time_point began) {
    auto& held = detail::state();
    const auto at = std::chrono::duration_cast<std::chrono::nanoseconds>(began.time_since_epoch()).count();
    held.kept = true;
    if (held.mode != Mode::TimeOfDay) {
        held.day_start_ns = at;
        held.anchor_ns = at;
    }
}

// How far into its day the venue is, in milliseconds - negative before a TimeOfDay day starts.
inline std::int64_t day_ms() {
    const auto& held = detail::state();
    if (held.mode == Mode::Elapsed && !held.kept)
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - held.started).count();
    return (detail::wall_ns() - held.day_start_ns) / 1'000'000;
}

}  // namespace sim::clock