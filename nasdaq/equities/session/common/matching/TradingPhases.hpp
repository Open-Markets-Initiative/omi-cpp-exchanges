#pragma once

// Generated from the Nasdaq Equities Session engine chart.
// The phases of a trading day, and what an order arriving in each of them
// meets. The book itself is MatchingEngine.hpp.

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

#include "MatchingEngine.hpp"

namespace matching {

// The phases the chart states. The day starts in the first of them.
enum class Phase : std::uint8_t { Closed, PreOpen, OpeningLockIn, OpeningLockOut, OpeningLate, Continuous, Halted, Quoting, PreClose, ClosingLockOut, ClosingFreeze, PostMarket, HaltedPreClose, HaltedClosingLockOut, HaltedClosingFreeze };
// Why a phase refuses an order, where the chart says: each word its refusals give.
enum class Reason : std::uint8_t { Lifetime, Type, Cross, Aggressive, Peg, Unreferenced, Phase, Slid, FatFinger, LimitProtection };

// What the venue announces as its phases change. The owner publishes each
// through every feed it serves, as that feed's own markings say it.
enum class Announcement : std::uint8_t { StartOfMessages, StartOfSystemHours, Halted, StartOfMarketHours, Quoting, Postponed, Resumed, EndOfMarketHours, EndOfSystemHours, EndOfMessages, Extended };

// The halts whose release is held, each by the change and the reason the owner says
// starts it (holding); none while none is.
enum class Hold : std::uint8_t { None, Ludp, T1, Ipoq };

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

    // The time, in nanoseconds, as the owner hands it with each message it gives the
    // day: what a timer set now is due from. The day keeps no clock of its own.
    void at(std::int64_t now) { now_ = now; }

    // The time come, as the owner's clock hands it: each timer due by then fires, in
    // the order the chart sets them, and is not set again until the chart sets it.
    void clock(std::int64_t now) {
        now_ = now;
        engine_.lapse(now);   // what expires by a time of its own, its time come
        engine_.resume_extended();   // and what the Extended Trading Close may now match
        engine_.lives(now);   // and what of the Midpoint Extended Life orders may now begin its holding period, or trade
        if (collar_due_ != 0 && now >= collar_due_) {   // a collared pause's extension over: its release tried again
            collar_due_ = 0;
            resume();
        }
    }

    // A halt's Auction Collar as the owner hands it, its listing's - Nasdaq's Limit Up-Limit Down pause
    // (Equity 4, Rule 4120(b)(4)(A)(i)c): its reference and the collar a halt starts with, and how far
    // each extension widens it on the side the cross would fall outside.
    void collar(std::int64_t reference, std::int64_t lower, std::int64_t upper, std::int64_t step) {
        collar_reference_given_ = reference; collar_lower_given_ = lower; collar_upper_given_ = upper; collar_step_ = step;
    }

    // A held halt has started, as the owner says: its cross held to the collar it starts with, where it
    // has one, and each extension of its display lasting as long as the venue declares. Only where the
    // change moved the day - its count of changes no longer what it was before: a venue-wide halt does
    // not hold anew a security it found already halted or quoting.
    void holding(Hold hold, std::uint64_t before) {
        if (phases_ == before) return;
        hold_ = hold; extension_ = 0; collar_due_ = 0; collar_widened_ = 0;
        switch (hold) {
            case Hold::Ludp: collar_wait_ = 3LL * 1000000000LL; collar_reference_ = collar_reference_given_; collar_lower_ = collar_lower_given_; collar_upper_ = collar_upper_given_; return;   // the listing's collar, as the owner handed it
            case Hold::T1:   // either side of its last sale, else its reference: the greater of 10% of it and 10000, 5000 where it is at or below 10000, each rounded to a tick
                collar_wait_ = 3LL * 1000000000LL; collar_reference_ = engine_.traded() > 0 ? engine_.traded() : reference_;
                collar_lower_ = engine_.on_tick(collar_reference_ - std::max<std::int64_t>(collar_reference_ * 1000 / 10000, collar_reference_ > 10000 ? 10000 : 5000), 0);
                collar_upper_ = engine_.on_tick(collar_reference_ + std::max<std::int64_t>(collar_reference_ * 1000 / 10000, collar_reference_ > 10000 ? 10000 : 5000), 0);
                return;
            case Hold::Ipoq: collar_wait_ = 3LL * 1000000000LL; collar_reference_ = 0; collar_lower_ = 0; collar_upper_ = 0; return;   // held to no collar
            default: return;
        }
    }

    // What the venue announces of the collar: its reference, its prices now, and its extensions.
    std::int64_t  collar_reference() const { return collar_reference_; }
    std::int64_t  collar_upper() const { return collar_upper_; }
    std::int64_t  collar_lower() const { return collar_lower_; }
    std::uint32_t collar_extension() const { return extension_; }

    // A held halt's release refused where its cross may not yet release it, and its display extended:
    // "Nasdaq will extend the Display Only Period for an additional 5-minute period" where the cross
    // would fall outside its collar (Equity 4, Rule 4120(b)(4)(A)(i)a.2-3, c.2-3), the collar widened as
    // the venue declares and the extension announced;
    // and where "all market orders would not be executed in the cross" (Rule 4120(b)(4)(A)(i)a.5.b,
    // c.5.b) - for a pause widened on one side, "all sell market orders" widening its lower price and
    // "all buy market orders" its upper (c.2) - and an IPO's release, held to no collar, postponed until
    // "all market orders will be executed in the cross" (Rule 4120(b)(4)(C)(i)b).
    // It is tried again once the extension is over. False where the cross may release it.
    bool extended() {
        const Indicative at = engine_.indicative(Cross::Intraday, reference_, floor_of(Cross::Intraday), ceiling_of(Cross::Intraday));
        const std::uint64_t bought = engine_.market_left(Side::Buy, at), sold = engine_.market_left(Side::Sell, at);   // the market orders it would leave unexecuted
        const bool below = at.qty > 0 && at.price < collar_lower_, above = at.qty > 0 && at.price > collar_upper_;   // outside its collar
        switch (hold_) {
            case Hold::Ludp:   // widened by its step on the side it would fall past
                if (!below && !above && bought == 0 && sold == 0) return false;
                if (below || sold > 0) collar_lower_ -= collar_step_;
                if (above || bought > 0) collar_upper_ += collar_step_;
                collar_widened_ = static_cast<std::uint8_t>((below || sold > 0 ? 1 : 0) | (above || bought > 0 ? 2 : 0));   // the sides it widened, which a Closing Cross releasing it widens again
                break;
            case Hold::T1: {   // widened on both sides, by 10%, then 20% of its reference, at least its amount
                if (!below && !above && bought == 0 && sold == 0) return false;
                const std::int64_t width = std::max<std::int64_t>(collar_reference_ * (extension_ == 0 ? 1000 : 2000) / 10000, collar_reference_ > 10000 ? 10000 : 5000);
                collar_lower_ = engine_.on_tick(collar_lower_ - width, 0);
                collar_upper_ = engine_.on_tick(collar_upper_ + width, 0);
                collar_widened_ = 3;   // both sides, which a Closing Cross releasing it widens again
                break;
            }
            case Hold::Ipoq:   // held to no collar: postponed while it would leave market orders unexecuted
                if (bought == 0 && sold == 0) return false;
                if (extension_++ == 0) on_announced_(Announcement::Postponed);   // announced once, then tried again until it may be made
                collar_due_ = now_ + collar_wait_;
                return true;
            default: return false;
        }
        ++extension_;
        on_announced_(Announcement::Extended);
        collar_due_ = now_ + collar_wait_;
        return true;
    }

    // Is the halt running released by the Closing Cross now that it collects, rather than by its own cross?
    // Where the venue declares its threshold prices: Nasdaq's "a trading halt that exists at or after 3:50
    // p.m. in a stock shall re-open via a Hybrid Closing Cross pursuant to Rule 4754(b)(7)" and "a Trading
    // Pause that exists at or after 3:50 p.m. in a stock shall re-open via a LULD Closing Cross pursuant to
    // Rule 4754(b)(6)" (Equity 4, Rule 4120(b)(4)(A)(i)a.4, c.4) - and, where it declares the change from
    // whose phase, only from there: "A halted security shall only be eligible to resume trading via the
    // Hybrid Closing Cross after the Display Only Period begins" (Rule 4754(b)(7)).
    bool closes_halt() const {
        switch (hold_) {
            case Hold::Ludp: return true;   // from any phase of the halt
            case Hold::T1: return phase_ == Phase::Quoting;   // from the phase Quote moves it into
            default: return false;
        }
    }

    // The threshold prices of the Closing Cross that releases the halt running: the collar it holds, as last
    // widened, widened again on the side its last extension widened - on both where it was never extended -
    // by the greater of a percent of the collar's reference and an amount, each rounded to a tick, a midpoint
    // up: "adding (subtracting) $1 or a certain percentage of the initial Auction Reference Price, whichever
    // is greater, to the upper (or from the lower) Auction Collar price that was last disseminated" (Rule
    // 4754(b)(7)(E)), "adding (subtracting) a threshold amount to the upper (or from the lower) Auction
    // Collar that was last updated" (Rule 4754(b)(6)(E)(i)). Its extensions are over: the close releases it.
    void close_halt() {
        collar_due_ = 0;
        std::int64_t width = 0;
        switch (hold_) {
            case Hold::Ludp: width = collar_reference_ * 500 / 10000; break;   // 5% of its reference
            case Hold::T1: { const std::int64_t least = collar_reference_ > 10000 ? 10000 : 5000; width = collar_reference_ * 1000 / 10000 > least ? collar_reference_ * 1000 / 10000 : least; break; }   // the greater of 10% of its reference and 10000, 5000 where it is at or below 10000
            default: threshold_lower_ = collar_lower_; threshold_upper_ = collar_upper_; return;
        }
        const bool lower = collar_widened_ == 0 || (collar_widened_ & 1) != 0;
        const bool upper = collar_widened_ == 0 || (collar_widened_ & 2) != 0;
        threshold_lower_ = lower ? engine_.on_tick(collar_lower_ > width ? collar_lower_ - width : 0, 0) : collar_lower_;
        threshold_upper_ = upper ? engine_.on_tick(collar_upper_ + width, 0) : collar_upper_;
    }

    // The prices the Closing Cross releasing a halt may cross at: its threshold prices, within the band the
    // owner set - "The Hybrid Closing Cross will occur at the price within the threshold prices" (Rule
    // 4754(b)(7)(D)), the LULD Closing Cross "within the benchmark prices" (Rule 4754(b)(6)(D)).
    std::int64_t threshold_floor() const { return threshold_lower_ > floor_ ? threshold_lower_ : floor_; }
    std::int64_t threshold_ceiling() const { return threshold_upper_ < ceiling_ ? threshold_upper_ : ceiling_; }

    // What the Closing Cross releasing a halt would cross at now: within its threshold prices, and its
    // Near and Current Reference Price so; its Far without them - "The Near Clearing Price and Reference
    // Prices contained in the EOII and the NOII ... will represent the price at which the Hybrid Closing
    // Cross would execute should the cross conclude at that time, bounded by the Threshold Prices, and the
    // Far Clearing Price will represent the price at which the Hybrid Closing Cross would execute should
    // the cross conclude at that time, if it were not bounded by the Threshold Prices" (Rule
    // 4754(b)(7)(C)). Asked early, it states neither.
    Indicative thresholded(bool early) const {
        Indicative at = engine_.indicative(Cross::Closing, reference_, threshold_floor(), threshold_ceiling());
        const Indicative unbounded = engine_.indicative(Cross::Closing, reference_, floor_, ceiling_);
        at.near_price = early ? 0 : at.price;
        at.far_price  = early ? 0 : unbounded.price;
        at.early      = early;
        return at;
    }

    // The price an uncross is drawn towards when two prices would cross as much: the
    // orderbook's reference, which the owner sets from what the venue publishes.
    void reference(std::int64_t price) { reference_ = price; }

    // The prices an uncross may cross at, the owner's to set: the venue's own limits
    // where it states them, else what its rules put around the reference.
    void band(std::int64_t floor, std::int64_t ceiling) { floor_ = floor; ceiling_ = ceiling; }

    // The prices a cross may cross at: the band the owner set, held within its benchmark's threshold
    // where the venue declares one, rounded in to a tick - Nasdaq's "the Nasdaq Opening Cross shall
    // occur at a price within the threshold amounts" (Equity 4, Rules 4752(d)(2)(E), 4754(b)(2)(E)). A
    // cross with no benchmark to measure from is held to the band alone.
    std::int64_t floor_of(Cross cross) const {
        switch (cross) {
            case Cross::Opening: { const std::int64_t from = reference_; return from > 0 ? std::max(floor_, engine_.on_tick(from - from * 1000 / 10000, 1)) : floor_; }   // 10% of its reference
            case Cross::Closing: { const std::int64_t from = reference_; return from > 0 ? std::max(floor_, engine_.on_tick(from - from * 1000 / 10000, 1)) : floor_; }   // 10% of its reference
            default: return floor_;
        }
    }
    std::int64_t ceiling_of(Cross cross) const {
        switch (cross) {
            case Cross::Opening: { const std::int64_t from = reference_; return from > 0 ? std::min(ceiling_, engine_.on_tick(from + from * 1000 / 10000, -1)) : ceiling_; }   // 10% of its reference
            case Cross::Closing: { const std::int64_t from = reference_; return from > 0 ? std::min(ceiling_, engine_.on_tick(from + from * 1000 / 10000, -1)) : ceiling_; }   // 10% of its reference
            default: return ceiling_;
        }
    }

    // What the closing auction crossed at, which trade at last trades at.
    std::int64_t closing_price() const { return close_; }

    // Is an auction collecting? An owner publishes the indicative price while one is.
    bool collecting() const { return phase_ == Phase::PreOpen || phase_ == Phase::OpeningLockIn || phase_ == Phase::OpeningLockOut || phase_ == Phase::OpeningLate || phase_ == Phase::Halted || phase_ == Phase::Quoting || phase_ == Phase::PreClose || phase_ == Phase::ClosingLockOut || phase_ == Phase::ClosingFreeze || phase_ == Phase::HaltedPreClose || phase_ == Phase::HaltedClosingLockOut || phase_ == Phase::HaltedClosingFreeze; }

    // What the auction being collected for would cross at now, told to the owner with which
    // auction it is; nothing while none is collecting.
    // In the three prices of an Order Imbalance Indicator, and without its indicative prices where asked for early.
    void indicate(bool early) {
        switch (phase_) {
            case Phase::PreOpen: on_indicated_(Cross::Opening, engine_.indicated(Cross::Opening, early, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); return;
            case Phase::OpeningLockIn: on_indicated_(Cross::Opening, engine_.indicated(Cross::Opening, early, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); return;
            case Phase::OpeningLockOut: on_indicated_(Cross::Opening, engine_.indicated(Cross::Opening, early, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); return;
            case Phase::OpeningLate: on_indicated_(Cross::Opening, engine_.indicated(Cross::Opening, early, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); return;
            case Phase::Halted: on_indicated_(Cross::Intraday, engine_.indicated(Cross::Intraday, early, reference_, floor_of(Cross::Intraday), ceiling_of(Cross::Intraday))); return;
            case Phase::Quoting: on_indicated_(Cross::Intraday, engine_.indicated(Cross::Intraday, early, reference_, floor_of(Cross::Intraday), ceiling_of(Cross::Intraday))); return;
            case Phase::PreClose: on_indicated_(Cross::Closing, engine_.indicated(Cross::Closing, early, reference_, floor_of(Cross::Closing), ceiling_of(Cross::Closing))); return;
            case Phase::ClosingLockOut: on_indicated_(Cross::Closing, engine_.indicated(Cross::Closing, early, reference_, floor_of(Cross::Closing), ceiling_of(Cross::Closing))); return;
            case Phase::ClosingFreeze: on_indicated_(Cross::Closing, engine_.indicated(Cross::Closing, early, reference_, floor_of(Cross::Closing), ceiling_of(Cross::Closing))); return;
            case Phase::HaltedPreClose: on_indicated_(Cross::Closing, thresholded(early)); return;
            case Phase::HaltedClosingLockOut: on_indicated_(Cross::Closing, thresholded(early)); return;
            case Phase::HaltedClosingFreeze: on_indicated_(Cross::Closing, thresholded(early)); return;
            case Phase::PostMarket: if (engine_.extending()) { on_indicated_(Cross::Extended, engine_.extended_indicated()); } return;
            default: return;
        }
    }

    void answer_on_new_order(Order order) {
        switch (phase_) {
            case Phase::Closed: on_refused_(order); return;
            case Phase::PreOpen: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLockIn: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLockOut: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Opening && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Opening && (order.price != market(order.side) && order.peg == matching::Peg::None) && !order.imbalance_only && unreferenced(order)) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Opening && (order.price != market(order.side) && order.peg == matching::Peg::None) && !order.imbalance_only && aggressive(order) && order.rejects_aggressive) { on_refused_(order, Reason::Aggressive); return; } if (order.only == Cross::Opening && (order.price != market(order.side) && order.peg == matching::Peg::None) && !order.imbalance_only && aggressive(order)) { reprice(order); } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::OpeningLate: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Opening && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Opening && order.lifetime == matching::Lifetime::Immediate) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { engine_.hold(order); return; } if (order.lifetime == matching::Lifetime::Immediate) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Extended) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } if (order.lifetime == matching::Lifetime::Timed) { if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } engine_.on_new_order(order); return; } engine_.hold(order); return;
            case Phase::Continuous: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (order.withdraws && engine_.slides(order)) { on_refused_(order, Reason::Slid); return; } engine_.on_new_order(order); return;
            case Phase::Halted: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } engine_.hold(order); return;
            case Phase::Quoting: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } engine_.hold(order); return;
            case Phase::PreClose: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (order.withdraws && engine_.slides(order)) { on_refused_(order, Reason::Slid); return; } engine_.on_new_order(order); return;
            case Phase::ClosingLockOut: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Closing && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Closing && (order.price != market(order.side) && order.peg == matching::Peg::None) && !order.imbalance_only && unreferenced(order)) { on_refused_(order, Reason::Unreferenced); return; } if (order.only == Cross::Closing && (order.price != market(order.side) && order.peg == matching::Peg::None) && !order.imbalance_only && aggressive(order) && order.rejects_aggressive) { on_refused_(order, Reason::Aggressive); return; } if (order.only == Cross::Closing && (order.price != market(order.side) && order.peg == matching::Peg::None) && !order.imbalance_only && aggressive(order)) { reprice(order); } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (order.withdraws && engine_.slides(order)) { on_refused_(order, Reason::Slid); return; } engine_.on_new_order(order); return;
            case Phase::ClosingFreeze: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Closing && !order.imbalance_only) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (order.withdraws && engine_.slides(order)) { on_refused_(order, Reason::Slid); return; } engine_.on_new_order(order); return;
            case Phase::PostMarket: if (order.only == Cross::Extended) { if (!engine_.extending()) { on_refused_(order, Reason::Cross); return; } engine_.extend(order); return; } if (order.only == Cross::ExtendedLife) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } if (order.lifetime == matching::Lifetime::Session) { on_refused_(order, Reason::Phase); return; } if (fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (order.withdraws && engine_.slides(order)) { on_refused_(order, Reason::Slid); return; } engine_.on_new_order(order); return;
            case Phase::HaltedPreClose: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } engine_.hold(order); return;
            case Phase::HaltedClosingLockOut: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Closing && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Closing && (order.price != market(order.side) && order.peg == matching::Peg::None) && !order.imbalance_only) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } engine_.hold(order); return;
            case Phase::HaltedClosingFreeze: if (order.only == Cross::ExtendedLife) { engine_.live(order, now_); return; } if (order.only == Cross::None && (order.price == market(order.side) && order.peg == matching::Peg::None)) { on_refused_(order, Reason::Type); return; } if (order.only == Cross::Intraday) { on_refused_(order, Reason::Cross); return; } if (order.only == Cross::Closing && !order.imbalance_only) { on_refused_(order, Reason::Cross); return; } if (order.only != Cross::None) { alone(order); return; } if ((order.lifetime == matching::Lifetime::Timed ? order.expires : std::numeric_limits<std::int64_t>::max()) <= now_) { on_refused_(order, Reason::Lifetime); return; } if ((order.peg != matching::Peg::None)) { on_refused_(order, Reason::Peg); return; } engine_.hold(order); return;
        }
    }

    // What the Extended Trading Close may match once the phase has answered: the book trading
    // on changed, a suspension that held is over where it no longer bids above its price or
    // offers below it.
    void on_new_order(Order order) {
        answer_on_new_order(order);
        engine_.resume_extended();
    }

    // A cancel, answering whether the order was in the book to take out. A phase
    // that refuses cancels holds nothing to take, so it answers no.
    bool answer_on_cancel(std::uint64_t id) {
        switch (phase_) {
            case Phase::Closed: return false;
            case Phase::PreOpen: return engine_.on_cancel(id);
            case Phase::OpeningLockIn: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::OpeningLockOut: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::OpeningLate: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::Continuous: return engine_.on_cancel(id);
            case Phase::Halted: return engine_.on_cancel(id);
            case Phase::Quoting: return engine_.on_cancel(id);
            case Phase::PreClose: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::ClosingLockOut: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::ClosingFreeze: if (engine_.on_cancel_trading(id)) return true; if (const int held = engine_.defer_cancel(id); held != 0) { if (held == 1) { on_refused_.pending(id); } return true; } return false;
            case Phase::PostMarket: return engine_.on_cancel(id);
            case Phase::HaltedPreClose: return engine_.on_cancel(id);
            case Phase::HaltedClosingLockOut: return engine_.on_cancel(id);
            case Phase::HaltedClosingFreeze: return engine_.on_cancel(id);
        }
        return false;
    }
    // A reduce, answering whether the order was in the book: lowered where it stands in a phase
    // that takes cancels, as a cancel is taken. A phase that refuses cancels answers no.
    bool answer_on_reduce(std::uint64_t id, std::uint64_t qty) {
        switch (phase_) {
            case Phase::Closed: return false;
            case Phase::PreOpen: return engine_.reduce(id, qty);
            case Phase::OpeningLockIn: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::OpeningLockOut: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::OpeningLate: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::Continuous: return engine_.reduce(id, qty);
            case Phase::Halted: return engine_.reduce(id, qty);
            case Phase::Quoting: return engine_.reduce(id, qty);
            case Phase::PreClose: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::ClosingLockOut: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::ClosingFreeze: if (engine_.reduce_trading(id, qty)) return true; if (engine_.holds(id)) { on_refused_.reduce(id); } return false;
            case Phase::PostMarket: return engine_.reduce(id, qty);
            case Phase::HaltedPreClose: return engine_.reduce(id, qty);
            case Phase::HaltedClosingLockOut: return engine_.reduce(id, qty);
            case Phase::HaltedClosingFreeze: return engine_.reduce(id, qty);
        }
        return false;
    }

    // What the Extended Trading Close may match once the book trading on has changed: a
    // suspension that held is over where it no longer bids above its price or offers below it.
    bool on_cancel(std::uint64_t id) {
        const bool answered = answer_on_cancel(id);
        engine_.resume_extended();
        return answered;
    }

    // What the Extended Trading Close may match once the book trading on has changed: a
    // suspension that held is over where it no longer bids above its price or offers below it.
    bool on_reduce(std::uint64_t id, std::uint64_t qty) {
        const bool answered = answer_on_reduce(id, qty);
        engine_.resume_extended();
        return answered;
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
    void answer_on_replace(Order order, [[maybe_unused]] std::uint64_t replaced) {
        switch (phase_) {
            case Phase::Closed: on_refused_(order); return;
            case Phase::PreOpen: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLockIn: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLockOut: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::OpeningLate: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::Continuous: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (order.only == Cross::None && fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (order.only == Cross::None && unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::Halted: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::Quoting: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::PreClose: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (order.only == Cross::None && fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (order.only == Cross::None && unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::ClosingLockOut: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (order.only == Cross::None && fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (order.only == Cross::None && unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::ClosingFreeze: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (order.only == Cross::None && fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (order.only == Cross::None && unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (!engine_.take_replaced_trading(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } if (order.only != Cross::None) { order.shown = false; engine_.hold(order); engine_.replaced(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::PostMarket: if (order.only == Cross::Extended) { if (!engine_.extending()) { on_refused_(order, Reason::Cross); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.extend(order); return; } if (order.only == Cross::ExtendedLife) { on_refused_(order, Reason::Cross); return; } if (order.lifetime == matching::Lifetime::Session) { on_refused_(order, Reason::Phase); return; } if (order.only == Cross::None && fat_fingered(order)) { on_refused_(order, Reason::FatFinger); return; } if (order.only == Cross::None && unprotected(order)) { on_refused_(order, Reason::LimitProtection); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.on_new_order(order); engine_.replaced(order); return;
            case Phase::HaltedPreClose: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::HaltedClosingLockOut: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
            case Phase::HaltedClosingFreeze: if (order.only == Cross::ExtendedLife) { if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } engine_.live(order, now_); return; } if (!engine_.take_replaced(replaced)) { on_refused_(order); return; } order.expires = engine_.expires_replaced(); if (engine_.replaced_waiting() && order.only != Cross::None) { order.shown = false; engine_.rewait(order); return; } engine_.hold(order); engine_.replaced(order); return;
        }
    }

    // What the Extended Trading Close may match once the phase has answered: the book trading
    // on changed, a suspension that held is over where it no longer bids above its price or
    // offers below it.
    void on_replace(Order order, std::uint64_t replaced) {
        answer_on_replace(order, replaced);
        engine_.resume_extended();
    }

    // PreOpen: Closed to PreOpen
    bool preopen() {
        if (moved(Phase::Closed, Phase::PreOpen)) {
            on_announced_(Announcement::StartOfMessages);
            on_announced_(Announcement::StartOfSystemHours);
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            keep(Cross::Opening, true, reference_ > 0, reference_);   // the orderbook's declared reference
            return true;
        }
        return false;
    }

    // OpeningLockIn: PreOpen to OpeningLockIn
    bool openinglockin() {
        if (moved(Phase::PreOpen, Phase::OpeningLockIn)) {
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Halt: PreOpen to Halted, Continuous to Halted
    bool halt() {
        if (moved(Phase::PreOpen, Phase::Halted)) {
            on_announced_(Announcement::Halted);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        if (moved(Phase::Continuous, Phase::Halted)) {
            on_announced_(Announcement::Halted);
            engine_.stop_lives();   // the Midpoint Extended Life orders held, ineligible
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Open: PreOpen to Continuous, OpeningLockIn to Continuous, OpeningLockOut to Continuous, OpeningLate to Continuous, Halted to Halted, Quoting to Quoting
    bool open() {
        if (moved(Phase::PreOpen, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            if (tested(Cross::Opening)) crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); else abandoned(Cross::Opening); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            engine_.start_lives(now_, false);   // the Midpoint Extended Life orders may trade, each held one beginning its holding period
            return true;
        }
        if (moved(Phase::OpeningLockIn, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            if (tested(Cross::Opening)) crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); else abandoned(Cross::Opening); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            engine_.start_lives(now_, false);   // the Midpoint Extended Life orders may trade, each held one beginning its holding period
            return true;
        }
        if (moved(Phase::OpeningLockOut, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            if (tested(Cross::Opening)) crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); else abandoned(Cross::Opening); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            engine_.start_lives(now_, false);   // the Midpoint Extended Life orders may trade, each held one beginning its holding period
            return true;
        }
        if (moved(Phase::OpeningLate, Phase::Continuous)) {
            on_announced_(Announcement::StartOfMarketHours);
            if (tested(Cross::Opening)) crossed(Cross::Opening, engine_.uncross(Cross::Opening, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening))); else abandoned(Cross::Opening); engine_.cancel_pending();
            engine_.ended(Cross::Opening);
            engine_.start_lives(now_, false);   // the Midpoint Extended Life orders may trade, each held one beginning its holding period
            return true;
        }
        if (moved(Phase::Halted, Phase::Halted)) {
            engine_.expire_cross(Cross::Opening, matching::Elimination::Halt);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        if (moved(Phase::Quoting, Phase::Quoting)) {
            engine_.expire_cross(Cross::Opening, matching::Elimination::Halt);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // OpeningLockOut: OpeningLockIn to OpeningLockOut
    bool openinglockout() {
        if (moved(Phase::OpeningLockIn, Phase::OpeningLockOut)) {
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            keep_indicated(Cross::Opening, false);   // the Current Reference Price its indication states now
            return true;
        }
        return false;
    }

    // OpeningLate: OpeningLockOut to OpeningLate
    bool openinglate() {
        if (moved(Phase::OpeningLockOut, Phase::OpeningLate)) {
            engine_.join(Cross::Opening);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // PreClose: Continuous to PreClose, Halted to HaltedPreClose, Halted to PreClose, Quoting to HaltedPreClose, Quoting to PreClose
    bool preclose() {
        if (moved(Phase::Continuous, Phase::PreClose)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            keep_indicated(Cross::Closing, true);   // the Current Reference Price its indication states now
            return true;
        }
        if (closes_halt() && moved(Phase::Halted, Phase::HaltedPreClose)) {
            close_halt();   // the halt carried into the Closing Cross, held within its threshold prices
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        if (moved(Phase::Halted, Phase::PreClose)) {
            on_announced_(Announcement::Resumed);
            engine_.start_lives(now_, true);   // the Midpoint Extended Life orders trade again, each whose holding period had not passed beginning the resumed one
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            keep_indicated(Cross::Closing, true);   // the Current Reference Price its indication states now
            return true;
        }
        if (closes_halt() && moved(Phase::Quoting, Phase::HaltedPreClose)) {
            close_halt();   // the halt carried into the Closing Cross, held within its threshold prices
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        if (moved(Phase::Quoting, Phase::PreClose)) {
            on_announced_(Announcement::Resumed);
            engine_.start_lives(now_, true);   // the Midpoint Extended Life orders trade again, each whose holding period had not passed beginning the resumed one
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            keep_indicated(Cross::Closing, true);   // the Current Reference Price its indication states now
            return true;
        }
        return false;
    }

    // Quote: Halted to Quoting
    bool quote() {
        if (moved(Phase::Halted, Phase::Quoting)) {
            on_announced_(Announcement::Quoting);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Postpone: Halted to Halted
    bool postpone() {
        if (moved(Phase::Halted, Phase::Halted)) {
            on_announced_(Announcement::Postponed);
            engine_.join(Cross::Intraday);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Resume: Halted to Continuous, Quoting to Continuous
    bool resume() {
        if (hold_ != Hold::None && phase_ == Phase::Halted && extended()) return true;   // not yet to be released: extended
        if (moved(Phase::Halted, Phase::Continuous)) {
            hold_ = Hold::None; collar_due_ = 0;   // released: the halt is over
            crossed(Cross::Intraday, engine_.uncross(Cross::Intraday, reference_, floor_of(Cross::Intraday), ceiling_of(Cross::Intraday))); engine_.cancel_pending();
            engine_.ended(Cross::Intraday);
            engine_.expire(matching::Lifetime::Immediate);
            on_announced_(Announcement::Resumed);
            engine_.start_lives(now_, true);   // the Midpoint Extended Life orders trade again, each whose holding period had not passed beginning the resumed one
            return true;
        }
        if (hold_ != Hold::None && phase_ == Phase::Quoting && extended()) return true;   // not yet to be released: extended
        if (moved(Phase::Quoting, Phase::Continuous)) {
            hold_ = Hold::None; collar_due_ = 0;   // released: the halt is over
            crossed(Cross::Intraday, engine_.uncross(Cross::Intraday, reference_, floor_of(Cross::Intraday), ceiling_of(Cross::Intraday))); engine_.cancel_pending();
            engine_.ended(Cross::Intraday);
            engine_.expire(matching::Lifetime::Immediate);
            on_announced_(Announcement::Resumed);
            engine_.start_lives(now_, true);   // the Midpoint Extended Life orders trade again, each whose holding period had not passed beginning the resumed one
            return true;
        }
        return false;
    }

    // ClosingLockOut: PreClose to ClosingLockOut, HaltedPreClose to HaltedClosingLockOut
    bool closinglockout() {
        if (moved(Phase::PreClose, Phase::ClosingLockOut)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            keep_indicated(Cross::Closing, false);   // the Current Reference Price its indication states now
            return true;
        }
        if (moved(Phase::HaltedPreClose, Phase::HaltedClosingLockOut)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // Close: PreClose to PostMarket, ClosingLockOut to PostMarket, ClosingFreeze to PostMarket, HaltedPreClose to PostMarket, HaltedClosingLockOut to PostMarket, HaltedClosingFreeze to PostMarket
    bool close() {
        if (moved(Phase::PreClose, Phase::PostMarket)) {
            on_announced_(Announcement::EndOfMarketHours);
            const Indicative closing = crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_of(Cross::Closing), ceiling_of(Cross::Closing))); close_ = closed(closing); engine_.cancel_pending();
            if (closing.qty > 0) { engine_.extend_from(close_); }   // the Extended Trading Close, at the Nasdaq Official Closing Price
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_cross(Cross::ExtendedLife);
            return true;
        }
        if (moved(Phase::ClosingLockOut, Phase::PostMarket)) {
            on_announced_(Announcement::EndOfMarketHours);
            const Indicative closing = crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_of(Cross::Closing), ceiling_of(Cross::Closing))); close_ = closed(closing); engine_.cancel_pending();
            if (closing.qty > 0) { engine_.extend_from(close_); }   // the Extended Trading Close, at the Nasdaq Official Closing Price
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_cross(Cross::ExtendedLife);
            return true;
        }
        if (moved(Phase::ClosingFreeze, Phase::PostMarket)) {
            on_announced_(Announcement::EndOfMarketHours);
            const Indicative closing = crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, floor_of(Cross::Closing), ceiling_of(Cross::Closing))); close_ = closed(closing); engine_.cancel_pending();
            if (closing.qty > 0) { engine_.extend_from(close_); }   // the Extended Trading Close, at the Nasdaq Official Closing Price
            engine_.ended(Cross::Closing);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_cross(Cross::ExtendedLife);
            return true;
        }
        if (moved(Phase::HaltedPreClose, Phase::PostMarket)) {
            hold_ = Hold::None; collar_due_ = 0;   // released through the Closing Cross: the halt is over
            on_announced_(Announcement::EndOfMarketHours);
            const Indicative closing = crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, threshold_floor(), threshold_ceiling())); close_ = closed(closing); engine_.cancel_pending();
            if (closing.qty > 0) { engine_.extend_from(close_); }   // the Extended Trading Close, at the Nasdaq Official Closing Price
            engine_.ended(Cross::Closing);
            engine_.expire_cross(Cross::Intraday);
            engine_.expire(matching::Lifetime::Immediate);
            on_announced_(Announcement::Resumed);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_cross(Cross::ExtendedLife);
            return true;
        }
        if (moved(Phase::HaltedClosingLockOut, Phase::PostMarket)) {
            hold_ = Hold::None; collar_due_ = 0;   // released through the Closing Cross: the halt is over
            on_announced_(Announcement::EndOfMarketHours);
            const Indicative closing = crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, threshold_floor(), threshold_ceiling())); close_ = closed(closing); engine_.cancel_pending();
            if (closing.qty > 0) { engine_.extend_from(close_); }   // the Extended Trading Close, at the Nasdaq Official Closing Price
            engine_.ended(Cross::Closing);
            engine_.expire_cross(Cross::Intraday);
            engine_.expire(matching::Lifetime::Immediate);
            on_announced_(Announcement::Resumed);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_cross(Cross::ExtendedLife);
            return true;
        }
        if (moved(Phase::HaltedClosingFreeze, Phase::PostMarket)) {
            hold_ = Hold::None; collar_due_ = 0;   // released through the Closing Cross: the halt is over
            on_announced_(Announcement::EndOfMarketHours);
            const Indicative closing = crossed(Cross::Closing, engine_.uncross(Cross::Closing, reference_, threshold_floor(), threshold_ceiling())); close_ = closed(closing); engine_.cancel_pending();
            if (closing.qty > 0) { engine_.extend_from(close_); }   // the Extended Trading Close, at the Nasdaq Official Closing Price
            engine_.ended(Cross::Closing);
            engine_.expire_cross(Cross::Intraday);
            engine_.expire(matching::Lifetime::Immediate);
            on_announced_(Announcement::Resumed);
            engine_.expire(matching::Lifetime::Session);
            engine_.expire_pegged();
            engine_.expire_cross(Cross::ExtendedLife);
            return true;
        }
        return false;
    }

    // ClosingFreeze: ClosingLockOut to ClosingFreeze, HaltedClosingLockOut to HaltedClosingFreeze
    bool closingfreeze() {
        if (moved(Phase::ClosingLockOut, Phase::ClosingFreeze)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        if (moved(Phase::HaltedClosingLockOut, Phase::HaltedClosingFreeze)) {
            engine_.join(Cross::Closing);   // what waits for its cross joins it as it collects
            return true;
        }
        return false;
    }

    // ExtendedTradingCloseEnd: PostMarket to PostMarket
    bool extendedtradingcloseend() {
        if (moved(Phase::PostMarket, Phase::PostMarket)) {
            engine_.expire_cross(Cross::Extended);
            return true;
        }
        return false;
    }

    // End: PostMarket to Closed
    bool end() {
        if (moved(Phase::PostMarket, Phase::Closed)) {
            on_announced_(Announcement::EndOfSystemHours);
            engine_.expire(matching::Lifetime::Extended);
            engine_.expire(matching::Lifetime::Timed);
            engine_.expire_cross(Cross::Extended);
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

    // An order for one cross alone, which the book never shows: given to its cross while that
    // collects, held aside until it does, and refused once it is done for the day.
    void alone(Order order) {
        if (order.only == Cross::Extended) { on_refused_(order, Reason::Cross); return; }   // the Extended Trading Close takes its own where it runs; elsewhere it is refused
        if (order.only == Cross::ExtendedLife) { on_refused_(order, Reason::Cross); return; }   // the Midpoint Extended Life book takes its own where a phase gives it one; elsewhere it is refused
        order.shown = false;
        if (engine_.done(order.only)) { on_refused_(order); return; }
        if (collects(order.only)) { engine_.hold(order); return; }
        engine_.wait(order);
    }

    // Does the phase the day is in collect for this cross?
    bool collects(Cross cross) const {
        switch (phase_) {
            case Phase::PreOpen: return cross == Cross::Opening;
            case Phase::OpeningLockIn: return cross == Cross::Opening;
            case Phase::OpeningLockOut: return cross == Cross::Opening;
            case Phase::OpeningLate: return cross == Cross::Opening;
            case Phase::Halted: return cross == Cross::Intraday;
            case Phase::Quoting: return cross == Cross::Intraday;
            case Phase::PreClose: return cross == Cross::Closing;
            case Phase::ClosingLockOut: return cross == Cross::Closing;
            case Phase::ClosingFreeze: return cross == Cross::Closing;
            case Phase::HaltedPreClose: return cross == Cross::Closing;
            case Phase::HaltedClosingLockOut: return cross == Cross::Closing;
            case Phase::HaltedClosingFreeze: return cross == Cross::Closing;
            default: return false;
        }
    }

    // An uncross the owner is told of: which auction it ended, what it crossed at, and the
    // match number of its print, numbered after its fills - one that crossed nothing too, as
    // the venue numbers it.
    Indicative crossed(Cross cross, Indicative at) {
        on_crossed_(cross, at, engine_.numbered());
        return at;
    }

    // With nothing crossed in the closing auction the reference stands; a venue that
    // closes on its last trade sets the reference to it first.
    std::int64_t closed(Indicative at) const { return at.qty > 0 ? at.price : reference_; }

    // Does the cross pass one of its price tests, tried in order: its price, as it would cross now,
    // within a test's threshold of what the test measures from - Nasdaq's Opening Cross Price Tests:
    // "each security in the Nasdaq Opening Cross must also pass one of the Opening Cross Price Tests"
    // (Equity 4, Rule 4752(d)(2)(F)). A cross that would cross nothing has no price to test.
    bool tested(Cross cross) const {
        switch (cross) {
            case Cross::Opening: {
                const Indicative at = engine_.indicative(Cross::Opening, reference_, floor_of(Cross::Opening), ceiling_of(Cross::Opening));
                if (at.qty == 0) return true;
                if (within(at.price, reference_, 500)) return true;   // 5% of its reference
                if (within(at.price, engine_.traded(), 500)) return true;   // 5% of its last sale
                if (within(at.price, engine_.quoted(at.price > reference_ ? Side::Buy : Side::Sell).value_or(0), 500)) return true;   // 5% of its best bid above its reference, its best offer at or below
                return false;
            }
            default: return true;
        }
    }

    // Is a price within a threshold, in hundredths of a percent, of what it is measured from? Never
    // where there is nothing to measure from.
    static bool within(std::int64_t price, std::int64_t from, std::int64_t basis_points) {
        if (from <= 0) return false;
        const std::int64_t threshold = from * basis_points / 10000;
        return price >= from - threshold && price <= from + threshold;
    }

    // A cross that passed none of its price tests: "all MOO, LOO, OIO, and Early Market Hours orders in
    // the Nasdaq Opening Cross in the security will be cancelled back to Participants, no Nasdaq Opening
    // Cross in that security will occur" (Rule 4752(d)(2)(F)) - every order it collected canceled back,
    // and the owner told it crossed nothing.
    Indicative abandoned(Cross cross) {
        engine_.abandon(cross, matching::Elimination::Protection);
        return crossed(cross, Indicative{});
    }

    // The reference prices a cross holds a late order to, as the day kept them: the lowest and the
    // highest, none until one is kept (Nasdaq Equity 4, Rule 4702(b)(9)(A), (12)(A)).
    struct Kept {
        bool         any  = false;
        std::int64_t low  = 0;
        std::int64_t high = 0;
    };

    // A reference price kept for a cross's late orders, as the chart keeps it; `first` starts the
    // cross's references anew. Nothing is kept where there is no price: an indication pairing no
    // shares states no Current Reference Price.
    void keep(Cross cross, bool first, bool any, std::int64_t price) {
        Kept& kept = kept_[static_cast<std::size_t>(cross)];
        if (first) kept = Kept{};
        if (!any) return;
        kept.low  = kept.any ? std::min(kept.low, price) : price;
        kept.high = kept.any ? std::max(kept.high, price) : price;
        kept.any  = true;
    }

    // The Current Reference Price the cross's indication states now, kept where it pairs any shares.
    void keep_indicated(Cross cross, bool first) {
        const Indicative at = engine_.indicated(cross, false, reference_, floor_of(cross), ceiling_of(cross));
        keep(cross, first, at.qty > 0, at.price);
    }

    // Does a late order's cross keep no reference price to hold it to?
    bool unreferenced(const Order& order) const { return !kept_[static_cast<std::size_t>(order.only)].any; }

    // Is a late order priced beyond its cross's reference prices: a buy above the highest of them, a
    // sell below the lowest?
    bool aggressive(const Order& order) const {
        const Kept& kept = kept_[static_cast<std::size_t>(order.only)];
        return kept.any && (order.side == Side::Buy ? order.price > kept.high : order.price < kept.low);
    }

    // A late order priced beyond its cross's reference prices, repriced to the most aggressive of
    // them: a buy to the highest, a sell to the lowest. One off its tick is rounded up where the
    // cross's indication has buyers left over as the order arrives, down where it has sellers, and
    // to the nearer tick, a midpoint up, where it has neither.
    void reprice(Order& order) const {
        const Kept& kept = kept_[static_cast<std::size_t>(order.only)];
        const Indicative at = engine_.indicated(order.only, false, reference_, floor_of(order.only), ceiling_of(order.only));
        order.price = engine_.on_tick(order.side == Side::Buy ? kept.high : kept.low, at.surplus == 0 ? 0 : at.buyers ? 1 : -1);
    }

    // A limit order priced further through the book's best opposite price than its port lets it
    // be: Nasdaq's Fat Finger Protection, "evaluating whether the limit price is set too
    // aggressively through the NBBO (percentage-based and dollar-based controls are
    // available)" (Equity 6, Section 5(m)) - the tighter of the two where both are given. None
    // for a port giving neither, a market or pegged order, or where the other side shows nothing.
    bool fat_fingered(const Order& o) const {
        if (o.protection_percent == 0 && o.protection_amount == 0) return false;
        if (o.price == market(o.side)) return false;   // a market order is no limit order
        if (o.peg != Peg::None) return false;   // its price is the engine's
        const auto far = engine_.quoted(o.side == Side::Buy ? Side::Sell : Side::Buy);
        if (!far) return false;
        std::int64_t room = std::numeric_limits<std::int64_t>::max();
        if (o.protection_percent != 0) room = std::min(room, *far * static_cast<std::int64_t>(o.protection_percent) / 10000);
        if (o.protection_amount != 0) room = std::min(room, o.protection_amount);
        return o.side == Side::Buy ? o.price - *far > room : *far - o.price > room;
    }

    // A limit order priced beyond the venue's own limit from the book's best opposite price, whatever
    // its port: Nasdaq's Limit Order Protection, "The LOP Limit shall be the greater of 10% of the
    // LOP Reference Price or $0.50", the offer for a buy and the bid for a sell, and a sell is not
    // held to it while "the National Best Bid ... is equal to or less than $0.50" (Equity 4, Rule
    // 4757(c)) - the limit as the venue declares it. It "does not apply to Orders with Market and
    // Primary Pegging", and "A Midpoint Pegging Order with a discretion price would not be
    // subject to LOP": a Midpoint Peg without one is held to it by the limit it states.
    bool unprotected(const Order& o) const {
        if (o.price == market(o.side)) return false;   // a market order is no limit order, nor a peg stating no limit
        if (o.peg != Peg::None && (o.peg != Peg::Midpoint || o.discretion != 0)) return false;
        const auto reference = engine_.quoted(o.side == Side::Buy ? Side::Sell : Side::Buy);
        if (!reference) return false;   // no established reference price
        if (o.side == Side::Sell && *reference <= 5000) return false;   // a bid at or below which a sell is not held to it
        const std::int64_t room = std::max<std::int64_t>(*reference * 1000 / 10000, 5000);   // 10% of it, and 5000 at the least
        return o.side == Side::Buy ? o.price - *reference > room : *reference - o.price > room;
    }

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
    std::int64_t  now_ = 0;   // the time the owner last handed, in nanoseconds
    std::array<Kept, 6> kept_{};   // by Cross: the reference prices each holds a late order to
    Hold          hold_ = Hold::None;   // the held halt running, none while none is
    std::int64_t  collar_reference_given_ = 0, collar_lower_given_ = 0, collar_upper_given_ = 0;   // the listing's collar, as the owner handed it
    std::int64_t  collar_step_ = 0;   // how far each extension widens the listing's collar
    std::int64_t  collar_reference_ = 0;   // the reference of the collar the halt running holds its cross to
    std::int64_t  collar_lower_ = 0, collar_upper_ = 0;   // and that collar, none for a halt held to no collar
    std::uint32_t extension_ = 0;   // how many times the halt running has been extended
    std::int64_t  collar_due_ = 0;   // when its extension is over, 0 while none runs
    std::int64_t  collar_wait_ = 0;   // how long an extension of the halt running lasts, in nanoseconds
    std::uint8_t  collar_widened_ = 0;   // the sides the last extension of the halt running widened: 1 its lower, 2 its upper
    std::int64_t  threshold_lower_ = 0, threshold_upper_ = 0;   // the threshold prices of the Closing Cross releasing it
};
}
