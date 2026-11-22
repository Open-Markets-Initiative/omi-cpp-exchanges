#pragma once

#include "protocols/ipc/IpcBus.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <cstdio>
#include <string>
#include <string_view>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
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
    // An `early` one asks for the early indicator, which states no indicative prices.
    struct Indication {
        int after;
        int every;
        int final;
        int final_percent;
        bool early;
    };

    static constexpr std::array<Indication, 7> indications{ {
        Indication{ 0, 10, 0, 0, true },   // Opening, early, from OpeningLockIn
        Indication{ 0, 1, 0, 0, false },   // Opening, from OpeningLockOut
        Indication{ 1, 1, 0, 0, false },   // Intraday, from Halt (LUDP)
        Indication{ 1, 1, 0, 0, false },   // Intraday, from Quote
        Indication{ 0, 10, 0, 0, true },   // Closing, early, from PreClose
        Indication{ 0, 1, 0, 0, false },   // Closing, from ClosingLockOut
        Indication{ 5, 5, 0, 0, false }    // Extended, from Close
    } };

    // A change of the declared day, and the turn of the startup run it is asked for before (a turn
    // past the last is after the run).
    // Where it starts a collection the day asks about, which indication it asks by.
    struct Scheduled {
        PhaseChange change;
        int         tick;
        int         released;   // the change that releases its security for quotation, or -1
        std::string_view instrument;   // the one instrument whose day the change is made in; none for every day
        std::string_view reason;   // why the change is made, as the day declares it; nothing where it declares none
        int         indication;   // into indications, or -1 where it starts no collection asked about
    };

    static constexpr std::array<Scheduled, 20> schedule{ {
        Scheduled{ PhaseChange::PreOpen, 0, -1, {}, "", -1 },
        Scheduled{ PhaseChange::Halt, 0, 6, "ZXZZT", "", -1 },
        Scheduled{ PhaseChange::OpeningLockIn, 2, -1, {}, "", 0 },
        Scheduled{ PhaseChange::OpeningLockOut, 3, -1, {}, "", 1 },
        Scheduled{ PhaseChange::OpeningLate, 4, -1, {}, "", 1 },
        Scheduled{ PhaseChange::Open, 4, -1, {}, "", -1 },
        Scheduled{ PhaseChange::Postpone, 5, 9, "ZXZZT", "", -1 },
        Scheduled{ PhaseChange::Halt, 5, -1, "ZWZZT", "LUDP", 2 },
        Scheduled{ PhaseChange::Resume, 7, -1, "ZWZZT", "", -1 },
        Scheduled{ PhaseChange::Quote, 7, -1, "ZXZZT", "IPOQ", 3 },
        Scheduled{ PhaseChange::Resume, 8, -1, "ZXZZT", "", -1 },
        Scheduled{ PhaseChange::Halt, 8, -1, {}, "T1", -1 },
        Scheduled{ PhaseChange::Quote, 9, -1, {}, "T3", 3 },
        Scheduled{ PhaseChange::Resume, 10, -1, {}, "", -1 },
        Scheduled{ PhaseChange::PreClose, 14, -1, {}, "", 4 },
        Scheduled{ PhaseChange::ClosingLockOut, 16, -1, {}, "", 5 },
        Scheduled{ PhaseChange::ClosingFreeze, 17, -1, {}, "", 5 },
        Scheduled{ PhaseChange::Close, 18, -1, {}, "", 6 },
        Scheduled{ PhaseChange::ExtendedTradingCloseEnd, 19, -1, {}, "", -1 },
        Scheduled{ PhaseChange::End, 20, -1, {}, "", -1 }
    } };

    // The seconds after a served venue starts that each scheduled change is asked for.
    using Times = std::array<int, schedule.size()>;

    explicit Day(Emit emit) : emit_(std::move(emit)) {}
    // A supervisor's requests, where the venue's settings give a port: lines of text, each
    //   break <match> <reason>
    // asking that a match be broken - its number as the venue told its fills - for a reason, one of the
    // codes break_reasons gives. Each is a Break Trade, put in order with everything else bound for the
    // engine; the supervisor is answered "asked", or why it is not. The engine says the Trade Broken, or
    // refuses a number it gave no match. One supervisor at a time; nothing here waits. It listens on
    // every interface, as a link does: on a container network the supervisor is elsewhere.
    void supervise(int port) {
        if (port <= 0) return;
        listener_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listener_ < 0) { std::printf("SUPERVISOR: no socket\n"); return; }
        int yes = 1;
        ::setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
        sockaddr_in where{};
        where.sin_family      = AF_INET;
        where.sin_port        = htons(static_cast<std::uint16_t>(port));
        where.sin_addr.s_addr = htonl(INADDR_ANY);
        if (::bind(listener_, reinterpret_cast<sockaddr*>(&where), sizeof where) != 0 || ::listen(listener_, 1) != 0) {
            std::printf("SUPERVISOR: cannot listen on port %d\n", port);
            ::close(listener_);
            listener_ = -1;
            return;
        }
        ::fcntl(listener_, F_SETFL, ::fcntl(listener_, F_GETFL, 0) | O_NONBLOCK);
        std::printf("SUPERVISOR listening on port %d\n", port);
    }

    ~Day() {
        if (supervisor_ >= 0) ::close(supervisor_);
        if (listener_ >= 0) ::close(listener_);
    }

    Day(const Day&)            = delete;
    Day& operator=(const Day&) = delete;

    // Ask the engine for a scheduled change now: the startup run, between its turns, asks for them
    // in the order they are scheduled.
    void request(const Scheduled& scheduled) {
        sim::emit_phase_request(emit_, scheduled.change, scheduled.reason, scheduled.instrument, released(scheduled));
        asking_ = scheduled.indication;
        if (books_) sim::emit_book_request(emit_);   // and for its book as the change leaves it
    }
    // When the security of a change is released for quotation: the time its releasing change is due,
    // counted from now by the seconds between the two in the day's times; 0 for a change that releases
    // nothing. The startup run, whose times are none, says now.
    std::uint64_t released(const Scheduled& scheduled) const {
        if (scheduled.released < 0) return 0;
        const auto at = static_cast<std::size_t>(&scheduled - schedule.data());
        const long long ahead = static_cast<long long>(at_[static_cast<std::size_t>(scheduled.released)]) - static_cast<long long>(at_[at]);
        return static_cast<std::uint64_t>(sim::send_time_nanos()) + static_cast<std::uint64_t>(ahead > 0 ? ahead : 0) * 1000000000ULL;
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
        if (asking_ >= 0) sim::emit_imbalance_request(emit_, indications[static_cast<std::size_t>(asking_)].early);
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
        if (supervised()) return true;   // what a supervisor asked, put in order first
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
            if (asking_ == was && (asking_ == 1 || asking_ == 5)) {   // the same collection goes on through the change
                ends_ = next_ < schedule.size() ? at_[next_] * 1000LL : std::numeric_limits<std::int64_t>::max();
                return true;
            }
            begun_ = at_[made] * 1000LL;
            ends_ = next_ < schedule.size() ? at_[next_] * 1000LL : std::numeric_limits<std::int64_t>::max();
            if (asking_ >= 0) due_ = begun_ + indications[static_cast<std::size_t>(asking_)].after * 1000LL;
            return true;
        }

        if (asking_ < 0 || due_ >= ends_ || now < due_) return false;
        sim::emit_imbalance_request(emit_, indications[static_cast<std::size_t>(asking_)].early);
        due_ = following(due_);
        return true;
    }

private:
    // What the supervisor sent since the last pass, each whole line of it asked. False where nothing came.
    bool supervised() {
        if (listener_ < 0) return false;
        if (const int taken = ::accept(listener_, nullptr, nullptr); taken >= 0) {   // a new supervisor replaces the last
            if (supervisor_ >= 0) ::close(supervisor_);
            supervisor_ = taken;
            ::fcntl(supervisor_, F_SETFL, ::fcntl(supervisor_, F_GETFL, 0) | O_NONBLOCK);
            heard_.clear();
        }
        if (supervisor_ < 0) return false;
        char bytes[512];
        const auto got = ::recv(supervisor_, bytes, sizeof bytes, 0);
        if (got < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) return false;
        if (got <= 0) {   // gone
            ::close(supervisor_);
            supervisor_ = -1;
            return false;
        }
        heard_.append(bytes, static_cast<std::size_t>(got));
        for (auto end = heard_.find('\n'); end != std::string::npos; end = heard_.find('\n')) {
            asked(heard_.substr(0, end));
            heard_.erase(0, end + 1);
        }
        if (heard_.size() > 4096) heard_.clear();   // no line is so long
        return true;
    }

    // One line the supervisor sent: a break asked, or refused.
    void asked(std::string line) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        char word[8] = {};
        unsigned long long match = 0;
        char reason = 0;
        int read = 0;
        if (std::sscanf(line.c_str(), "%7s %llu %c%n", word, &match, &reason, &read) == 3 && static_cast<std::size_t>(read) == line.size()
            && std::string_view(word) == "break" && sim::break_reasons.find(reason) != std::string_view::npos) {
            sim::emit_break_trade(emit_, match, reason);
            answer("asked\n");
            return;
        }
        answer("refused: break <match> <reason>, the reason one of " + std::string(sim::break_reasons) + "\n");
    }

    void answer(const std::string& said) const { (void)::send(supervisor_, said.data(), said.size(), MSG_NOSIGNAL); }
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
    int                                   listener_ = -1;     // the supervisor's port, where the settings give one
    int                                   supervisor_ = -1;   // the supervisor connected, or none
    std::string                           heard_;             // what it sent of a line not yet whole
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