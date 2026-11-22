#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <chrono>
#include "../../run/Clock.hpp"
#include <cstdio>
#include <optional>
#include <string_view>
#include <type_traits>
#include "structs/NewOrderMessage.hpp"
#include "structs/CancelOrderMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/OrderCanceledMessage.hpp"
#include "structs/OrderAcceptedMessage.hpp"
#include "structs/OrderEliminatedMessage.hpp"
#include "structs/ReplaceOrderMessage.hpp"
#include "structs/OrderReplacedMessage.hpp"
#include "structs/ReduceOrderMessage.hpp"
#include "structs/OrderReducedMessage.hpp"
#include "structs/BookRequestMessage.hpp"
#include "structs/BookOrderMessage.hpp"
#include "structs/BookEndMessage.hpp"
#include "structs/BreakTradeMessage.hpp"
#include "structs/TradeBrokenMessage.hpp"
#include "structs/CancelPendingMessage.hpp"
#include "structs/ReduceRefusedMessage.hpp"
#include "structs/OrderRefusedMessage.hpp"
#include "structs/PhaseAnnouncedMessage.hpp"
#include "structs/PhaseRequestMessage.hpp"
#include "structs/CrossCompletedMessage.hpp"
#include "structs/ImbalanceRequestMessage.hpp"
#include "structs/ImbalanceIndicatedMessage.hpp"
#include "structs/ClockMessage.hpp"
#include "MatchingEngine.hpp"
#include "TradingPhases.hpp"

namespace sim {

namespace ipc_ns = nasdaq::nsmequities::matchingengine::ipc::v2026;

// The price the trading day's uncross is drawn towards when two prices would cross as much: the
// instrument's reference, as the venue declares it.
inline constexpr std::int64_t instrument_reference = 10000;

// The reference an instrument has, as the venue declares it: a day for each instrument draws its own
// uncross towards its own.
constexpr std::int64_t reference_of(std::string_view instrument) {
    if (instrument == "ZVZZT") return 10000;
    if (instrument == "ZWZZT") return 10000;
    if (instrument == "ZXZZT") return 10000;
    return instrument_reference;
}

// The changes of phase the trading day answers, as its chart names them.
enum class PhaseChange : std::uint8_t { PreOpen, OpeningLockIn, Halt, Open, OpeningLockOut, OpeningLate, PreClose, Quote, Postpone, Resume, ClosingLockOut, Close, ClosingFreeze, ExtendedTradingCloseEnd, End };

// A change as the internal messaging model codes it, and back.
inline char requested(PhaseChange change) {
    switch (change) {
        case PhaseChange::PreOpen: return 'P';
        case PhaseChange::OpeningLockIn: return 'I';
        case PhaseChange::Halt: return 'H';
        case PhaseChange::Open: return 'O';
        case PhaseChange::OpeningLockOut: return 'K';
        case PhaseChange::OpeningLate: return 'J';
        case PhaseChange::PreClose: return 'L';
        case PhaseChange::Quote: return 'Q';
        case PhaseChange::Postpone: return 'W';
        case PhaseChange::Resume: return 'R';
        case PhaseChange::ClosingLockOut: return 'X';
        case PhaseChange::Close: return 'C';
        case PhaseChange::ClosingFreeze: return 'F';
        case PhaseChange::ExtendedTradingCloseEnd: return 'A';
        case PhaseChange::End: return 'E';
    }
    return '?';
}

inline std::optional<PhaseChange> phase_change(char code) {
    switch (code) {
        case 'P': return PhaseChange::PreOpen;
        case 'I': return PhaseChange::OpeningLockIn;
        case 'H': return PhaseChange::Halt;
        case 'O': return PhaseChange::Open;
        case 'K': return PhaseChange::OpeningLockOut;
        case 'J': return PhaseChange::OpeningLate;
        case 'L': return PhaseChange::PreClose;
        case 'Q': return PhaseChange::Quote;
        case 'W': return PhaseChange::Postpone;
        case 'R': return PhaseChange::Resume;
        case 'X': return PhaseChange::ClosingLockOut;
        case 'C': return PhaseChange::Close;
        case 'F': return PhaseChange::ClosingFreeze;
        case 'A': return PhaseChange::ExtendedTradingCloseEnd;
        case 'E': return PhaseChange::End;
    }
    return std::nullopt;
}

inline const char* phase_name(PhaseChange change) {
    switch (change) {
        case PhaseChange::PreOpen: return "PreOpen";
        case PhaseChange::OpeningLockIn: return "OpeningLockIn";
        case PhaseChange::Halt: return "Halt";
        case PhaseChange::Open: return "Open";
        case PhaseChange::OpeningLockOut: return "OpeningLockOut";
        case PhaseChange::OpeningLate: return "OpeningLate";
        case PhaseChange::PreClose: return "PreClose";
        case PhaseChange::Quote: return "Quote";
        case PhaseChange::Postpone: return "Postpone";
        case PhaseChange::Resume: return "Resume";
        case PhaseChange::ClosingLockOut: return "ClosingLockOut";
        case PhaseChange::Close: return "Close";
        case PhaseChange::ClosingFreeze: return "ClosingFreeze";
        case PhaseChange::ExtendedTradingCloseEnd: return "ExtendedTradingCloseEnd";
        case PhaseChange::End: return "End";
    }
    return "?";
}

// What serves a Phase Request: a trading day, which answers every change its chart names. A book
// on its own has no phases to change.
template <typename Day>
concept TradingDay = requires(Day& day) { day.preopen(); day.openinglockin(); day.halt(); day.open(); day.openinglockout(); day.openinglate(); day.preclose(); day.quote(); day.postpone(); day.resume(); day.closinglockout(); day.close(); day.closingfreeze(); day.extendedtradingcloseend(); day.end(); };

// Make the change the day was asked for: false when the phase it is in has no such change.
template <TradingDay Day>
bool change_phase(Day& day, PhaseChange change) {
    bool moved = false;
    switch (change) {
        case PhaseChange::PreOpen: moved = day.preopen(); break;
        case PhaseChange::OpeningLockIn: moved = day.openinglockin(); break;
        case PhaseChange::Halt: moved = day.halt(); break;
        case PhaseChange::Open: moved = day.open(); break;
        case PhaseChange::OpeningLockOut: moved = day.openinglockout(); break;
        case PhaseChange::OpeningLate: moved = day.openinglate(); break;
        case PhaseChange::PreClose: moved = day.preclose(); break;
        case PhaseChange::Quote: moved = day.quote(); break;
        case PhaseChange::Postpone: moved = day.postpone(); break;
        case PhaseChange::Resume: moved = day.resume(); break;
        case PhaseChange::ClosingLockOut: moved = day.closinglockout(); break;
        case PhaseChange::Close: moved = day.close(); break;
        case PhaseChange::ClosingFreeze: moved = day.closingfreeze(); break;
        case PhaseChange::ExtendedTradingCloseEnd: moved = day.extendedtradingcloseend(); break;
        case PhaseChange::End: moved = day.end(); break;
    }
    std::printf("PHASE %s %s\n", moved ? "moved" : "refused", phase_name(change));
    return moved;
}

// The auction a trade ended, as the internal messaging model codes it.
inline char auctioned(matching::Cross cross) {
    switch (cross) {
        case matching::Cross::None: return 'N';
        case matching::Cross::Opening: return 'O';
        case matching::Cross::Intraday: return 'I';
        case matching::Cross::Closing: return 'C';
        case matching::Cross::Extended: return 'A';
        case matching::Cross::ExtendedLife: return 'E';
        default: break;
    }
    return '?';
}

// And an imbalance-only order's fill in a cross: the cross's own order, offset by it.
inline char offset_auctioned(matching::Cross cross) {
    switch (cross) {
        case matching::Cross::Opening: return 'M';
        case matching::Cross::Closing: return 'L';
        default: break;
    }
    return '?';
}

// How long an order lives, by the code of its time in force, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Lifetime> lifetime_of(Code code) {
    switch (static_cast<long long>(code)) {
        case '0': return matching::Lifetime::Session;
        case '3': return matching::Lifetime::Immediate;
        case '5': return matching::Lifetime::Extended;
        case '6': return matching::Lifetime::Timed;
        default: return std::nullopt;
    }
}

// What a pegged order follows, none for one that is not pegged, by the code of its peg, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Peg> peg_of(Code code) {
    switch (static_cast<long long>(code)) {
        case 'L': return matching::Peg::None;
        case 'M': return matching::Peg::Midpoint;
        case 'P': return matching::Peg::Far;
        case 'R': return matching::Peg::Near;
        default: return std::nullopt;
    }
}

// Whose an attributable order is shown as, as the engine's order holds it: the characters its Ipc
// carries, padded with spaces.
inline std::array<char, 4> attribution_of(std::string_view text) {
    std::array<char, 4> held{ ' ', ' ', ' ', ' ' };
    for (std::size_t at = 0; at < held.size() && at < text.size(); ++at) held[at] = text[at];
    return held;
}

// What is done of two orders of one owner that would trade, by the code of an order's strategy, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Prevention> prevention_of(Code code) {
    switch (static_cast<long long>(code)) {
        case 'D': return matching::Prevention::Decrement;
        case 'O': return matching::Prevention::Oldest;
        case 'W': return matching::Prevention::Newest;
        case 'R': return matching::Prevention::Remover;
        default: return std::nullopt;
    }
}

// The one cross an order trades in, none for one that trades continuously, by the code of its cross type, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Cross> cross_of(Code code) {
    switch (static_cast<long long>(code)) {
        case 'N': return matching::Cross::None;
        case 'O': return matching::Cross::Opening;
        case 'C': return matching::Cross::Closing;
        case 'H': return matching::Cross::Intraday;
        case 'E': return matching::Cross::ExtendedLife;
        case 'A': return matching::Cross::Extended;
        default: return std::nullopt;
    }
}


// Why the engine refused an order: its type, or how long it lives, is none the engine has.
enum class Refusal { Lifetime, Phase, Cross, Peg, Type, Aggressive, Unreferenced, Slid, FatFinger, LimitProtection };

// What a server built straight over a book - a test's - does with an order it refuses, with
// nobody to tell: says so, as the engine does of an order for an instrument it does not trade.
struct UnheardRefusal {
    template <typename... Instrument>
    void operator()(const matching::Order& order, Refusal, Instrument...) const {
        std::printf("REFUSED order %llu: nobody listens for refusals here\n", (unsigned long long)order.id);
    }
                 template <typename... Instrument>
                 void pending(std::uint64_t id, Instrument...) const {
                     std::printf("PENDING cancel of %llu: nobody listens for refusals here\n", (unsigned long long)id);
                 }
                 template <typename... Instrument>
                 void reduce(std::uint64_t id, Instrument...) const {
                     std::printf("REFUSED reduce of %llu: nobody listens for refusals here\n", (unsigned long long)id);
                 }
};
// Ipc message type codes (ascii) — mirror the Ipc specification.
struct IpcType {
    static constexpr char Heartbeat          = 'H';
    static constexpr char NewOrder           = 'N';
    static constexpr char CancelOrder        = 'C';
    static constexpr char OrderRested        = 'R';
    static constexpr char OrderCanceled      = 'X';
    static constexpr char Trade              = 'T';
    static constexpr char OrderRefused       = 'J';
    static constexpr char PhaseAnnounced     = 'S';
    static constexpr char PhaseRequest       = 'P';
    static constexpr char CrossCompleted     = 'U';
    static constexpr char ImbalanceRequest   = 'Q';
    static constexpr char ImbalanceIndicated = 'I';
    static constexpr char OrderEliminated    = 'E';
    static constexpr char OrderAccepted      = 'A';
    static constexpr char ReplaceOrder       = 'M';
    static constexpr char OrderReplaced      = 'W';
    static constexpr char ReduceOrder        = 'D';
    static constexpr char OrderReduced       = 'Y';
    static constexpr char CancelPending      = 'G';
    static constexpr char ReduceRefused      = 'Z';
    static constexpr char Clock              = 'L';
    static constexpr char BookRequest        = 'O';
    static constexpr char BookOrder          = 'F';
    static constexpr char BookEnd            = 'V';
    static constexpr char BreakTrade         = 'k';
    static constexpr char TradeBroken        = 'b';
};

// The instruments the engine trades, a book for each, in the order the venue declares them:
// by symbol.
inline constexpr std::array<std::string_view, 3> instruments{ "ZVZZT", "ZWZZT", "ZXZZT" };

// The first, which a caller that names no instrument trades.
inline constexpr std::string_view instrument_symbol = instruments[0];

// A message bound for an engine, stamped by the engine's sequencer with its place in the one
// order it puts everything in and the time it put it there - the time the engine's order is
// kept by. False for a frame of no kind an engine is sent, which it leaves as it is.
inline bool stamp_sequence(char* frame, std::size_t length, std::uint64_t sequence, std::int64_t time) {
    if (length < 3) return false;
    std::byte* data = reinterpret_cast<std::byte*>(frame) + 3;
    switch (frame[2]) {
        case IpcType::NewOrder: { if (length < 3 + sizeof(ipc_ns::new_order_message)) return false; auto* m = ipc_ns::new_order_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::CancelOrder: { if (length < 3 + sizeof(ipc_ns::cancel_order_message)) return false; auto* m = ipc_ns::cancel_order_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::PhaseRequest: { if (length < 3 + sizeof(ipc_ns::phase_request_message)) return false; auto* m = ipc_ns::phase_request_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::ImbalanceRequest: { if (length < 3 + sizeof(ipc_ns::imbalance_request_message)) return false; auto* m = ipc_ns::imbalance_request_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::ReplaceOrder: { if (length < 3 + sizeof(ipc_ns::replace_order_message)) return false; auto* m = ipc_ns::replace_order_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::ReduceOrder: { if (length < 3 + sizeof(ipc_ns::reduce_order_message)) return false; auto* m = ipc_ns::reduce_order_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::Clock: { if (length < 3 + sizeof(ipc_ns::clock_message)) return false; auto* m = ipc_ns::clock_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::BookRequest: { if (length < 3 + sizeof(ipc_ns::book_request_message)) return false; auto* m = ipc_ns::book_request_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::BreakTrade: { if (length < 3 + sizeof(ipc_ns::break_trade_message)) return false; auto* m = ipc_ns::break_trade_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        default: return false;
    }
}

// A message's place in its sequencer's order; 0 for one no sequencer numbered - a test's,
// put straight to the engine.
inline std::uint64_t sequence_of(std::string_view bytes) {
    if (bytes.size() < 3) return 0;
    const std::byte* data = reinterpret_cast<const std::byte*>(bytes.data()) + 3;
    switch (bytes[2]) {
        case IpcType::NewOrder: return bytes.size() < 3 + sizeof(ipc_ns::new_order_message) ? 0 : ipc_ns::new_order_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::CancelOrder: return bytes.size() < 3 + sizeof(ipc_ns::cancel_order_message) ? 0 : ipc_ns::cancel_order_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::PhaseRequest: return bytes.size() < 3 + sizeof(ipc_ns::phase_request_message) ? 0 : ipc_ns::phase_request_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::ImbalanceRequest: return bytes.size() < 3 + sizeof(ipc_ns::imbalance_request_message) ? 0 : ipc_ns::imbalance_request_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::ReplaceOrder: return bytes.size() < 3 + sizeof(ipc_ns::replace_order_message) ? 0 : ipc_ns::replace_order_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::ReduceOrder: return bytes.size() < 3 + sizeof(ipc_ns::reduce_order_message) ? 0 : ipc_ns::reduce_order_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::Clock: return bytes.size() < 3 + sizeof(ipc_ns::clock_message) ? 0 : ipc_ns::clock_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::BookRequest: return bytes.size() < 3 + sizeof(ipc_ns::book_request_message) ? 0 : ipc_ns::book_request_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::BreakTrade: return bytes.size() < 3 + sizeof(ipc_ns::break_trade_message) ? 0 : ipc_ns::break_trade_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        default: return 0;
    }
}

// The time a message bound for an engine was put in order at, in nanoseconds; 0 for one no
// sequencer stamped. The time its day's timers are set from.
inline std::int64_t time_of(std::string_view bytes) {
    if (bytes.size() < 3) return 0;
    const std::byte* data = reinterpret_cast<const std::byte*>(bytes.data()) + 3;
    switch (bytes[2]) {
        case IpcType::NewOrder: return bytes.size() < 3 + sizeof(ipc_ns::new_order_message) ? 0 : static_cast<std::int64_t>(ipc_ns::new_order_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::CancelOrder: return bytes.size() < 3 + sizeof(ipc_ns::cancel_order_message) ? 0 : static_cast<std::int64_t>(ipc_ns::cancel_order_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::PhaseRequest: return bytes.size() < 3 + sizeof(ipc_ns::phase_request_message) ? 0 : static_cast<std::int64_t>(ipc_ns::phase_request_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::ImbalanceRequest: return bytes.size() < 3 + sizeof(ipc_ns::imbalance_request_message) ? 0 : static_cast<std::int64_t>(ipc_ns::imbalance_request_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::ReplaceOrder: return bytes.size() < 3 + sizeof(ipc_ns::replace_order_message) ? 0 : static_cast<std::int64_t>(ipc_ns::replace_order_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::ReduceOrder: return bytes.size() < 3 + sizeof(ipc_ns::reduce_order_message) ? 0 : static_cast<std::int64_t>(ipc_ns::reduce_order_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::Clock: return bytes.size() < 3 + sizeof(ipc_ns::clock_message) ? 0 : static_cast<std::int64_t>(ipc_ns::clock_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::BookRequest: return bytes.size() < 3 + sizeof(ipc_ns::book_request_message) ? 0 : static_cast<std::int64_t>(ipc_ns::book_request_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::BreakTrade: return bytes.size() < 3 + sizeof(ipc_ns::break_trade_message) ? 0 : static_cast<std::int64_t>(ipc_ns::break_trade_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        default: return 0;
    }
}

// Send-time clock (Ipc SendTime action): wall-clock nanoseconds since the POSIX epoch,
// captured as each message is framed onto the bus. Carried through the transforms onto
// every feed message's timestamp field.
inline std::int64_t send_time_nanos() {
    return sim::clock::now_nanos();   // the venue's clock: the wall clock, or its simulated day
}

namespace detail_ipc {

// Frame one packed ipc struct: [len u16 le][type u8][struct bytes]; emit the frame.
template <typename Emit, typename Struct>
inline void emit_framed(Emit& emit, char type, const Struct& payload) {
    char frame[3 + sizeof(Struct)];
    std::uint16_t length = static_cast<std::uint16_t>(1 + sizeof(Struct));
    std::memcpy(frame, &length, 2);
    frame[2] = type;
    std::memcpy(frame + 3, &payload, sizeof payload);
    emit(frame, sizeof frame);
}

}

// Gateway-side engine proxy: encodes engine calls as Ipc messages. Drop-in for the
// gateway's `Engine` template parameter — same on_new_order/on_cancel surface as
// matching::MatchingEngine.
template <typename Emit>
class IpcEngineClient {
public:
    explicit IpcEngineClient(Emit emit) : emit_(emit) {}

    void on_new_order(const matching::Order& order, std::string_view instrument = instrument_symbol) {
        ipc_ns::new_order_message m{};
        m.timestamp.set(send_time_nanos());
        m.symbol.set(instrument);
        m.order_id.set(order.id);
        m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
        m.price.set((order.peg == matching::Peg::None ? order.price : order.limit) == matching::market(order.side) ? std::nullopt : std::make_optional(static_cast<std::remove_cvref_t<decltype(m.price.get().value())>>((order.peg == matching::Peg::None ? order.price : order.limit))));
        m.quantity.set(static_cast<std::uint32_t>(order.qty));
        m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Session ? '0' : order.lifetime == matching::Lifetime::Immediate ? '3' : order.lifetime == matching::Lifetime::Extended ? '5' : '6'));
        m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
        m.cross_type.set(static_cast<std::remove_cvref_t<decltype(m.cross_type.get().value())>>(order.only == matching::Cross::ExtendedLife ? 'E' : order.only == matching::Cross::Extended ? 'A' : order.only == matching::Cross::Intraday ? 'H' : order.only == matching::Cross::Closing ? 'C' : order.only == matching::Cross::Opening ? 'O' : 'N'));
        m.peg.set(static_cast<std::remove_cvref_t<decltype(m.peg.get().value())>>(order.peg == matching::Peg::Near ? 'R' : order.peg == matching::Peg::Far ? 'P' : order.peg == matching::Peg::Midpoint ? 'M' : 'L'));
        m.expire_time.set(static_cast<std::remove_cvref_t<decltype(m.expire_time.get().value())>>(order.expires == 0 ? 0 : (order.expires - static_cast<std::int64_t>(send_time_nanos())) / 1000000000));
        detail_ipc::emit_framed(emit_, IpcType::NewOrder, m);
    }

    bool on_cancel(std::uint64_t id, std::string_view instrument = instrument_symbol) {
        ipc_ns::cancel_order_message m{};
        m.order_id.set(id);
        m.symbol.set(instrument);
        detail_ipc::emit_framed(emit_, IpcType::CancelOrder, m);
        return true;   // fire-and-forget over the bus; the engine result flows back as events
    }

private:
    Emit emit_;
};

// Engine-side endpoint: decodes Ipc messages and drives the real matching engine.
template <typename Engine, typename OnRefused = UnheardRefusal>
class IpcEngineServer {
public:
    explicit IpcEngineServer(Engine& engine, OnRefused on_refused = {}) : engine_(engine), on_refused_(on_refused) {}

    // Handle one framed Ipc message; returns false on an unknown/short frame.
    bool on_message(std::string_view bytes) {
        if (bytes.size() < 3) return false;
        const std::byte* data = reinterpret_cast<const std::byte*>(bytes.data()) + 3;
        // Its place in the order the engine's sequencer put everything in: the next, or a message was
        // lost between them. One no sequencer numbered - a test's, put straight here - is read as it is.
        if (const std::uint64_t sequence = sequence_of(bytes); sequence != 0) {
            if (sequence != sequenced_ + 1)
                std::printf("SEQUENCE GAP: expected %llu, read %llu\n", (unsigned long long)(sequenced_ + 1), (unsigned long long)sequence);
            sequenced_ = sequence;
        }

        // The time it was put in order at, handed to the days first: a timer the message sets is
        // due from it. One no sequencer stamped leaves the time as it was.
        if constexpr (requires { engine_.at(std::int64_t{}); }) {
            if (const std::int64_t now = time_of(bytes); now != 0) {
                engine_.at(now);
            }
        }

        switch (bytes[2]) {
            case IpcType::NewOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::new_order_message)) return false;
                const auto* m = ipc_ns::new_order_message::parse(const_cast<std::byte*>(data));
                const matching::Side side = static_cast<char>(m->side.get().value()) == 'B' ? matching::Side::Buy : matching::Side::Sell;
                const bool market = !m->price.get().has_value();   // it states no price
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                const std::optional<matching::Cross> only = cross_of(m->cross_type.get().value());
                const std::optional<matching::Peg> peg = peg_of(m->peg.get().value());
                const bool pegged = peg && *peg != matching::Peg::None;
                if (!peg || (*peg != matching::Peg::None) != pegged || (!lifetime && !(only && *only == matching::Cross::Extended)) || !only || static_cast<char>(m->discretion_type.get().value()) != 'L' || static_cast<char>(m->imbalance_only.get().value()) == 'Y' && (!only || (*only != matching::Cross::Opening && *only != matching::Cross::Closing))) {
                    matching::Order refused{};
                    refused.id    = m->order_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value_or(matching::market(side));
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, !peg || (*peg != matching::Peg::None) != pegged ? Refusal::Peg : (!lifetime && !(only && *only == matching::Cross::Extended)) ? Refusal::Lifetime : !only ? Refusal::Cross : static_cast<char>(m->discretion_type.get().value()) != 'L' ? Refusal::Peg : Refusal::Cross, m->symbol.get_trimmed().value());
                    return true;
                }
                const matching::Order order = [&] { matching::Order made = matching::Order{
                    m->order_id.get().value(),
                    side,
                    market ? matching::market(side) : m->price.get().value(),
                    m->quantity.get().value(),
                    lifetime.value_or(matching::Lifetime::Session),   // an order for the Extended Trading Close lives until it ends, whatever its time in force says
                    static_cast<char>(m->display.get().value()) != 'N',
                    *only,
                    *peg,
                    market ? matching::market(side) : m->price.get().value(),
                    static_cast<std::int64_t>(m->peg_offset.get().value()),
                    (m->expire_time.get().value() == 0 ? std::int64_t{0} : static_cast<std::int64_t>(m->timestamp.get().value()) + static_cast<std::int64_t>(m->expire_time.get().value()) * 1000000000),
                    static_cast<std::uint64_t>(m->min_qty.get().value()),
                    (static_cast<char>(m->post_only.get().value()) == 'P' || static_cast<char>(m->post_only.get().value()) == 'C'),
                    static_cast<char>(m->trade_now.get().value()) == 'Y',
                    static_cast<char>(m->short_sale.get().value()) == 'T',
                    static_cast<std::uint64_t>(m->max_floor.get().value()),
                    static_cast<char>(m->display.get().value()) == 'A',
                    attribution_of(m->attribution.get().value()),
                    static_cast<std::uint64_t>(m->self_match_group.get().value()),
                    static_cast<long long>(m->self_match_level.get().value()) == 'M' ? std::uint8_t{0} : static_cast<std::uint8_t>(m->self_match_level.get().value()),
                    prevention_of(m->self_match_strategy.get().value()).value_or(matching::Prevention::None),
                    static_cast<char>(m->aiq_strategy.get().value()),
                }; made.withdraws = static_cast<char>(m->post_only.get().value()) == 'C'; made.discretion = static_cast<std::int64_t>(m->discretion_price.get().value()); made.random = static_cast<std::uint64_t>(m->random_reserve.get().value()); made.imbalance_only = static_cast<char>(m->imbalance_only.get().value()) == 'Y'; made.rejects_aggressive = static_cast<char>(m->late_limit.get().value()) == 'J'; made.extends = static_cast<char>(m->extended_close.get().value()) == 'Y'; made.life_continuous = static_cast<char>(m->extended_life_book.get().value()) == 'B'; made.protection_percent = static_cast<std::uint32_t>(m->protection_percent.get().value()); made.protection_amount = static_cast<std::int64_t>(m->protection_amount.get().value()); return made; }();
                // To the book of the instrument it names, where the engine keeps one for each.
                if constexpr (requires { engine_.on_new_order(order, m->symbol.get_trimmed().value()); })
                    engine_.on_new_order(order, m->symbol.get_trimmed().value());
                else
                    engine_.on_new_order(order);
                return true;
            }
            case IpcType::CancelOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::cancel_order_message)) return false;
                const auto* m = ipc_ns::cancel_order_message::parse(const_cast<std::byte*>(data));
                // To the book of the instrument the order is for, where the engine keeps one for each.
                if constexpr (requires { engine_.on_cancel(m->order_id.get().value(), m->symbol.get_trimmed().value()); })
                    engine_.on_cancel(m->order_id.get().value(), m->symbol.get_trimmed().value());
                else
                    engine_.on_cancel(m->order_id.get().value());
                return true;
            }
            case IpcType::ReplaceOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::replace_order_message)) return false;
                const auto* m = ipc_ns::replace_order_message::parse(const_cast<std::byte*>(data));
                const matching::Side side = static_cast<char>(m->side.get().value()) == 'B' ? matching::Side::Buy : matching::Side::Sell;
                const bool market = !m->price.get().has_value();   // it states no price
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                const std::optional<matching::Cross> only = cross_of(m->cross_type.get().value());
                const std::optional<matching::Peg> peg = peg_of(m->peg.get().value());
                const bool pegged = peg && *peg != matching::Peg::None;
                if (!peg || (*peg != matching::Peg::None) != pegged || (!lifetime && !(only && *only == matching::Cross::Extended)) || !only || static_cast<char>(m->discretion_type.get().value()) != 'L' || static_cast<char>(m->imbalance_only.get().value()) == 'Y' && (!only || (*only != matching::Cross::Opening && *only != matching::Cross::Closing))) {
                    matching::Order refused{};
                    refused.id    = m->replacement_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value_or(matching::market(side));
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, !peg || (*peg != matching::Peg::None) != pegged ? Refusal::Peg : (!lifetime && !(only && *only == matching::Cross::Extended)) ? Refusal::Lifetime : !only ? Refusal::Cross : static_cast<char>(m->discretion_type.get().value()) != 'L' ? Refusal::Peg : Refusal::Cross, m->symbol.get_trimmed().value());
                    return true;
                }
                const matching::Order order = [&] { matching::Order made = matching::Order{
                    m->replacement_id.get().value(),
                    side,
                    market ? matching::market(side) : m->price.get().value(),
                    m->quantity.get().value(),
                    lifetime.value_or(matching::Lifetime::Session),   // an order for the Extended Trading Close lives until it ends, whatever its time in force says
                    static_cast<char>(m->display.get().value()) != 'N',
                    *only,
                    *peg,
                    market ? matching::market(side) : m->price.get().value(),
                    static_cast<std::int64_t>(m->peg_offset.get().value()),
                    0,
                    static_cast<std::uint64_t>(m->min_qty.get().value()),
                    (static_cast<char>(m->post_only.get().value()) == 'P' || static_cast<char>(m->post_only.get().value()) == 'C'),
                    static_cast<char>(m->trade_now.get().value()) == 'Y',
                    static_cast<char>(m->short_sale.get().value()) == 'T',
                    static_cast<std::uint64_t>(m->max_floor.get().value()),
                    static_cast<char>(m->display.get().value()) == 'A',
                    attribution_of(m->attribution.get().value()),
                    static_cast<std::uint64_t>(m->self_match_group.get().value()),
                    static_cast<long long>(m->self_match_level.get().value()) == 'M' ? std::uint8_t{0} : static_cast<std::uint8_t>(m->self_match_level.get().value()),
                    prevention_of(m->self_match_strategy.get().value()).value_or(matching::Prevention::None),
                    static_cast<char>(m->aiq_strategy.get().value()),
                }; made.withdraws = static_cast<char>(m->post_only.get().value()) == 'C'; made.discretion = static_cast<std::int64_t>(m->discretion_price.get().value()); made.random = static_cast<std::uint64_t>(m->random_reserve.get().value()); made.imbalance_only = static_cast<char>(m->imbalance_only.get().value()) == 'Y'; made.extends = static_cast<char>(m->extended_close.get().value()) == 'Y'; made.life_continuous = static_cast<char>(m->extended_life_book.get().value()) == 'B'; made.protection_percent = static_cast<std::uint32_t>(m->protection_percent.get().value()); made.protection_amount = static_cast<std::int64_t>(m->protection_amount.get().value()); return made; }();
                // To the day of the instrument it names, where the engine keeps one for each; an
                // engine without a day - a test's bare book - replaces nothing.
                if constexpr (requires { engine_.on_replace(order, m->order_id.get().value(), m->symbol.get_trimmed().value()); })
                    engine_.on_replace(order, m->order_id.get().value(), m->symbol.get_trimmed().value());
                return true;
            }
            case IpcType::ReduceOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::reduce_order_message)) return false;
                const auto* m = ipc_ns::reduce_order_message::parse(const_cast<std::byte*>(data));
                if constexpr (requires { engine_.on_reduce(m->order_id.get().value(), m->quantity.get().value(), m->symbol.get_trimmed().value()); })
                    engine_.on_reduce(m->order_id.get().value(), m->quantity.get().value(), m->symbol.get_trimmed().value());
                return true;
            }
            case IpcType::PhaseRequest: {
                if constexpr (TradingDay<Engine>) {
                    if (bytes.size() < 3 + sizeof(ipc_ns::phase_request_message)) return false;
                    const auto* m = ipc_ns::phase_request_message::parse(const_cast<std::byte*>(data));
                    const auto change = phase_change(static_cast<char>(m->phase_change.get().value()));
                    phase_reason_ = std::string(m->reason.get_trimmed().value());   // why, which what is announced of it says
                    phase_release_ = static_cast<std::uint64_t>(m->release.get().value());   // when its security is released, which what is announced of it says
                    if (!change) return false;
                    if (const auto named = m->symbol.get_trimmed().value(); named != decltype(named){}) {   // one instrument's day alone
                        if constexpr (requires { engine_.of(named); }) {
                            auto* one = engine_.of(named);
                            if (one == nullptr) return false;
                            [[maybe_unused]] const std::uint64_t before = (*one).phases();   // its day's changes before this one
                            change_phase(*one, *change);
                            if constexpr (requires { (*one).holding(matching::Hold::None, before); }) { if (*change == PhaseChange::Halt && phase_reason_ == "LUDP") (*one).holding(matching::Hold::Ludp, before); else if (*change == PhaseChange::Halt && phase_reason_ == "T1") (*one).holding(matching::Hold::T1, before); else if (*change == PhaseChange::Quote && phase_reason_ == "IPOQ") (*one).holding(matching::Hold::Ipoq, before); }   // a held halt: its release held as the venue declares, in each day the change moved
                        } else {
                            return false;
                        }
                    } else {
                        [[maybe_unused]] const auto before = engine_.moves();   // each day's changes before this one
                        change_phase(engine_, *change);
                        if constexpr (requires { engine_.holding(matching::Hold::None, before); }) { if (*change == PhaseChange::Halt && phase_reason_ == "LUDP") engine_.holding(matching::Hold::Ludp, before); else if (*change == PhaseChange::Halt && phase_reason_ == "T1") engine_.holding(matching::Hold::T1, before); else if (*change == PhaseChange::Quote && phase_reason_ == "IPOQ") engine_.holding(matching::Hold::Ipoq, before); }   // a held halt: its release held as the venue declares, in each day the change moved
                    }
                    phase_reason_.clear();
                    phase_release_ = 0;
                    return true;
                } else {
                    return false;
                }
            }
            case IpcType::ImbalanceRequest: {
                if constexpr (requires { engine_.indicate(bool{}); }) {
                    if (bytes.size() < 3 + sizeof(ipc_ns::imbalance_request_message)) return false;
                    const auto* m = ipc_ns::imbalance_request_message::parse(const_cast<std::byte*>(data));
                    engine_.indicate(static_cast<char>(m->early.get().value()) == 'Y');   // asked for early: no indicative prices
                    return true;
                } else {
                    return false;
                }
            }
            case IpcType::Clock: {
                if constexpr (requires { engine_.clock(std::int64_t{}); }) {
                    if (bytes.size() < 3 + sizeof(ipc_ns::clock_message)) return false;
                    const auto* m = ipc_ns::clock_message::parse(const_cast<std::byte*>(data));
                    engine_.clock(static_cast<std::int64_t>(m->timestamp.get().value()));
                    return true;
                } else {
                    return false;
                }
            }
            case IpcType::BookRequest: {
                if (bytes.size() < 3 + sizeof(ipc_ns::book_request_message)) return false;
                book_asked_ = true;
                return true;
            }
            case IpcType::BreakTrade: {
                if (bytes.size() < 3 + sizeof(ipc_ns::break_trade_message)) return false;
                const auto* m = ipc_ns::break_trade_message::parse(const_cast<std::byte*>(data));
                break_asked_ = std::pair<std::uint64_t, char>{ static_cast<std::uint64_t>(m->execution_id.get().value()), static_cast<char>(m->break_reason.get().value()) };
                return true;
            }
            default:
                return false;
        }
    }

    // Did the message just read ask for the engine's book? Asked once, answered once.
    bool book_asked() { const bool asked = book_asked_; book_asked_ = false; return asked; }

    // The place of the message just read in the order its sequencer put everything in; 0 where none numbered it.
    std::uint64_t book_sequence() const { return sequenced_; }
    // The match the message just read asked to be broken, and why; asked once, answered once.
    std::optional<std::pair<std::uint64_t, char>> break_asked() { auto asked = break_asked_; break_asked_.reset(); return asked; }
    // Why the change of phase being made is made, as its request said; nothing while none is.
    std::string_view phase_reason() const { return phase_reason_; }

    // When the security of the change being made will be released, as its request said; 0 while none is.
    std::uint64_t phase_release() const { return phase_release_; }

private:
    Engine& engine_;
    std::string phase_reason_;   // why the change of phase being made is made
    std::uint64_t phase_release_ = 0;   // when the security of the change being made will be released
    bool book_asked_ = false;   // the message just read asked for the engine's book
    std::optional<std::pair<std::uint64_t, char>> break_asked_;   // the match the message just read asked to be broken, and why
    OnRefused on_refused_;
    std::uint64_t sequenced_ = 0;   // the last place in its sequencer's order it read
};

// The event callback of a consumer that does not listen for that event.
struct IgnoreEvent {
    template <typename Message>
    void operator()(const Message&) const {}
};

// Event-side endpoint: decodes engine events off the bus and hands them to the
// consumers (feed publishers / gateway exec reports) as matching types.
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreEvent, typename OnAccepted = IgnoreEvent, typename OnEliminated = IgnoreEvent, typename OnRefused = IgnoreEvent, typename OnAnnounced = IgnoreEvent, typename OnCrossed = IgnoreEvent, typename OnIndicated = IgnoreEvent, typename OnReplaced = IgnoreEvent, typename OnReduced = IgnoreEvent, typename OnCancelPending = IgnoreEvent, typename OnReduceRefused = IgnoreEvent, typename OnBroken = IgnoreEvent>
class IpcEventServer {
public:
    IpcEventServer(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_() {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled) {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnAccepted on_accepted, OnEliminated on_eliminated, OnRefused on_refused, OnAnnounced on_announced, OnCrossed on_crossed, OnIndicated on_indicated, OnReplaced on_replaced, OnReduced on_reduced, OnCancelPending on_cancel_pending, OnReduceRefused on_reduce_refused, OnBroken on_broken)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_accepted_(on_accepted), on_eliminated_(on_eliminated), on_refused_(on_refused), on_announced_(on_announced), on_crossed_(on_crossed), on_indicated_(on_indicated), on_replaced_(on_replaced), on_reduced_(on_reduced), on_cancel_pending_(on_cancel_pending), on_reduce_refused_(on_reduce_refused), on_broken_(on_broken) {}
    bool on_message(std::string_view bytes) {
        if (bytes.size() < 3) return false;
        const std::byte* data = reinterpret_cast<const std::byte*>(bytes.data()) + 3;

        switch (bytes[2]) {
            case IpcType::Trade: {
                if (bytes.size() < 3 + sizeof(ipc_ns::trade_message)) return false;
                on_trade_(*ipc_ns::trade_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::OrderRested: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_rested_message)) return false;
                on_rest_(*ipc_ns::order_rested_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::OrderCanceled: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_canceled_message)) return false;
                on_canceled_(*ipc_ns::order_canceled_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::OrderAccepted: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_accepted_message)) return false;
                on_accepted_(*ipc_ns::order_accepted_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::OrderEliminated: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_eliminated_message)) return false;
                on_eliminated_(*ipc_ns::order_eliminated_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::OrderRefused: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_refused_message)) return false;
                on_refused_(*ipc_ns::order_refused_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::PhaseAnnounced: {
                if (bytes.size() < 3 + sizeof(ipc_ns::phase_announced_message)) return false;
                on_announced_(*ipc_ns::phase_announced_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::CrossCompleted: {
                if (bytes.size() < 3 + sizeof(ipc_ns::cross_completed_message)) return false;
                on_crossed_(*ipc_ns::cross_completed_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::ImbalanceIndicated: {
                if (bytes.size() < 3 + sizeof(ipc_ns::imbalance_indicated_message)) return false;
                on_indicated_(*ipc_ns::imbalance_indicated_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::OrderReplaced: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_replaced_message)) return false;
                on_replaced_(*ipc_ns::order_replaced_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::OrderReduced: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_reduced_message)) return false;
                on_reduced_(*ipc_ns::order_reduced_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::CancelPending: {
                if (bytes.size() < 3 + sizeof(ipc_ns::cancel_pending_message)) return false;
                on_cancel_pending_(*ipc_ns::cancel_pending_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::ReduceRefused: {
                if (bytes.size() < 3 + sizeof(ipc_ns::reduce_refused_message)) return false;
                on_reduce_refused_(*ipc_ns::reduce_refused_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            case IpcType::TradeBroken: {
                if (bytes.size() < 3 + sizeof(ipc_ns::trade_broken_message)) return false;
                on_broken_(*ipc_ns::trade_broken_message::parse(const_cast<std::byte*>(data)));
                return true;
            }
            default:
                return false;
        }
    }

private:
    OnTrade    on_trade_;
    OnRest     on_rest_;
    OnCanceled on_canceled_;
    OnAccepted  on_accepted_;
    OnEliminated on_eliminated_;
    OnRefused   on_refused_;
    OnAnnounced on_announced_;
    OnCrossed   on_crossed_;
    OnIndicated on_indicated_;
    OnReplaced  on_replaced_;
    OnReduced   on_reduced_;
    OnCancelPending on_cancel_pending_;
    OnReduceRefused on_reduce_refused_;
    OnBroken    on_broken_;
};

// Event-side encoders: matching-engine events as Ipc messages (consumed by feed
// publishers / the gateway / recorders in a multi-process deployment).
template <typename Emit>
inline void emit_trade(Emit& emit, const matching::Trade& trade, std::string_view instrument = instrument_symbol) {
    ipc_ns::trade_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.aggressor_order_id.set(trade.aggressor_id);
    m.resting_order_id.set(trade.resting_id);
    m.execution_id.set(trade.match_id);
    m.price.set(trade.price);
    m.quantity.set(static_cast<std::uint32_t>(trade.qty));
    m.auction.set(static_cast<std::remove_cvref_t<decltype(m.auction.get().value())>>(trade.offset ? offset_auctioned(trade.cross) : auctioned(trade.cross)));
    m.resting_display.set(static_cast<std::remove_cvref_t<decltype(m.resting_display.get().value())>>(trade.resting_shown ? 'Y' : 'N'));
    m.aggressor_display.set(static_cast<std::remove_cvref_t<decltype(m.aggressor_display.get().value())>>(trade.aggressor_shown ? 'Y' : 'N'));
    m.resting_midpoint.set(static_cast<std::remove_cvref_t<decltype(m.resting_midpoint.get().value())>>(trade.resting_midpoint ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::Trade, m);
}

template <typename Emit>
inline void emit_rested(Emit& emit, const matching::Order& order, std::string_view instrument = instrument_symbol) {
    ipc_ns::order_rested_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.order_id.set(order.id);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.price == matching::market(order.side) ? std::nullopt : std::make_optional(static_cast<std::remove_cvref_t<decltype(m.price.get().value())>>(order.price)));
    m.quantity.set(static_cast<std::uint32_t>(matching::showing(order)));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? (order.attributed ? 'A' : 'Y') : 'N'));
    m.attribution.set(std::string_view{ order.attribution.data(), order.attribution.size() });
    m.refreshed_from.set(order.renamed);   // the id it stood under, where what it shows was refreshed
    m.repriced.set(static_cast<std::remove_cvref_t<decltype(m.repriced.get().value())>>(order.repriced ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::OrderRested, m);
}

template <typename Emit>
inline void emit_canceled(Emit& emit, const matching::Order& order, std::string_view instrument = instrument_symbol) {
    ipc_ns::order_canceled_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument);
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.prevented.set(static_cast<std::remove_cvref_t<decltype(m.prevented.get().value())>>(order.prevented.qty > 0 ? 'Y' : 'N'));
    if (order.prevented.qty > 0) m.prevented_liquidity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_liquidity.get().value())>>(order.prevented.resting ? 'A' : 'R'));
    m.prevented_quantity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_quantity.get().value())>>(order.prevented.qty));
    m.prevented_price.set(static_cast<std::remove_cvref_t<decltype(m.prevented_price.get().value())>>(order.prevented.price));
    m.prevented_strategy.set(static_cast<std::remove_cvref_t<decltype(m.prevented_strategy.get().value())>>(order.prevented.strategy));
    m.elimination_reason.set(static_cast<std::remove_cvref_t<decltype(m.elimination_reason.get().value())>>(order.eliminated == matching::Elimination::Halt ? 'H' : order.eliminated == matching::Elimination::Protection ? 'X' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::OrderCanceled, m);
}

template <typename Emit>
inline void emit_accepted(Emit& emit, const matching::Order& order, std::string_view instrument = instrument_symbol) {
    ipc_ns::order_accepted_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument);
    m.price.set((order.peg == matching::Peg::None ? order.price : order.limit) == matching::market(order.side) ? std::nullopt : std::make_optional(static_cast<std::remove_cvref_t<decltype(m.price.get().value())>>((order.peg == matching::Peg::None ? order.price : order.limit))));
    detail_ipc::emit_framed(emit, IpcType::OrderAccepted, m);
}


template <typename Emit>
inline void emit_eliminated(Emit& emit, const matching::Order& order, std::string_view instrument = instrument_symbol) {
    ipc_ns::order_eliminated_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set((order.peg == matching::Peg::None ? order.price : order.limit) == matching::market(order.side) ? std::nullopt : std::make_optional(static_cast<std::remove_cvref_t<decltype(m.price.get().value())>>((order.peg == matching::Peg::None ? order.price : order.limit))));
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Session ? '0' : order.lifetime == matching::Lifetime::Immediate ? '3' : order.lifetime == matching::Lifetime::Extended ? '5' : '6'));
    m.elimination_reason.set(static_cast<std::remove_cvref_t<decltype(m.elimination_reason.get().value())>>(order.eliminated == matching::Elimination::Halt ? 'H' : order.eliminated == matching::Elimination::Protection ? 'X' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::OrderEliminated, m);
}


template <typename Emit>
inline void emit_replaced(Emit& emit, std::uint64_t replaced, const matching::Order& order, std::string_view instrument = instrument_symbol) {
    ipc_ns::order_replaced_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(replaced);
    m.replacement_id.set(order.id);
    m.symbol.set(instrument);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set((order.peg == matching::Peg::None ? order.price : order.limit) == matching::market(order.side) ? std::nullopt : std::make_optional(static_cast<std::remove_cvref_t<decltype(m.price.get().value())>>((order.peg == matching::Peg::None ? order.price : order.limit))));
    m.quantity.set(static_cast<std::uint32_t>(matching::showing(order)));
    m.open_quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::OrderReplaced, m);
}


template <typename Emit>
inline void emit_reduced(Emit& emit, const matching::Order& order, std::uint64_t taken, std::string_view instrument = instrument_symbol) {
    ipc_ns::order_reduced_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.order_id.set(order.id);
    m.quantity.set(static_cast<std::uint32_t>(taken));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.prevented.set(static_cast<std::remove_cvref_t<decltype(m.prevented.get().value())>>(order.prevented.qty > 0 ? 'Y' : 'N'));
    if (order.prevented.qty > 0) m.prevented_liquidity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_liquidity.get().value())>>(order.prevented.resting ? 'A' : 'R'));
    m.prevented_quantity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_quantity.get().value())>>(order.prevented.qty));
    m.prevented_price.set(static_cast<std::remove_cvref_t<decltype(m.prevented_price.get().value())>>(order.prevented.price));
    m.prevented_strategy.set(static_cast<std::remove_cvref_t<decltype(m.prevented_strategy.get().value())>>(order.prevented.strategy));
    detail_ipc::emit_framed(emit, IpcType::OrderReduced, m);
}


// The question of the engine's own book: every order it holds, as of this message's place in the
// order its sequencer puts everything in.
template <typename Emit>
inline void emit_book_request(Emit& emit) {
    ipc_ns::book_request_message m{};
    m.timestamp.set(send_time_nanos());
    detail_ipc::emit_framed(emit, IpcType::BookRequest, m);
}

// One order of the engine's book, where the engine holds it, and the request it answers.
template <typename Emit>
inline void emit_book_order(Emit& emit, const matching::Order& order, char where, std::uint64_t sequence, std::string_view instrument = instrument_symbol) {
    ipc_ns::book_order_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set((order.peg == matching::Peg::None ? order.price : order.limit) == matching::market(order.side) ? std::nullopt : std::make_optional(static_cast<std::remove_cvref_t<decltype(m.price.get().value())>>((order.peg == matching::Peg::None ? order.price : order.limit))));
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.book_sequence.set(static_cast<std::remove_cvref_t<decltype(m.book_sequence.get().value())>>(sequence));
    m.book_place.set(static_cast<std::remove_cvref_t<decltype(m.book_place.get().value())>>(where));
    detail_ipc::emit_framed(emit, IpcType::BookOrder, m);
}

// The end of the engine's book: how many orders it just said, and the request they answer.
template <typename Emit>
inline void emit_book_end(Emit& emit, std::uint64_t orders, std::uint64_t sequence) {
    ipc_ns::book_end_message m{};
    m.timestamp.set(send_time_nanos());
    m.book_sequence.set(static_cast<std::remove_cvref_t<decltype(m.book_sequence.get().value())>>(sequence));
    m.book_orders.set(static_cast<std::remove_cvref_t<decltype(m.book_orders.get().value())>>(orders));
    detail_ipc::emit_framed(emit, IpcType::BookEnd, m);
}

// The request a Book End answers, where a frame is one: none of any other.
inline std::optional<std::uint64_t> book_end_of(std::string_view bytes) {
    if (bytes.size() < 3 + sizeof(ipc_ns::book_end_message) || bytes[2] != IpcType::BookEnd) return std::nullopt;
    const auto* m = ipc_ns::book_end_message::parse(const_cast<std::byte*>(reinterpret_cast<const std::byte*>(bytes.data()) + 3));
    return static_cast<std::uint64_t>(m->book_sequence.get().value());
}

// The codes a match may be broken for, as the venue's Ipc gives them.
inline constexpr std::string_view break_reasons = "ECSX";

// A supervisor's request that a match be broken - its number as the venue told its fills - for a
// reason, one of break_reasons: put in order by the engine's sequencer as anything bound for it is.
template <typename Emit>
inline void emit_break_trade(Emit& emit, std::uint64_t execution, char reason) {
    ipc_ns::break_trade_message m{};
    m.timestamp.set(send_time_nanos());
    m.execution_id.set(execution);
    m.break_reason.set(static_cast<std::remove_cvref_t<decltype(m.break_reason.get().value())>>(reason));
    detail_ipc::emit_framed(emit, IpcType::BreakTrade, m);
}

// A match the engine made, broken: its number, its two orders, and why. The book is not changed by it.
template <typename Emit>
inline void emit_trade_broken(Emit& emit, std::uint64_t execution, std::uint64_t aggressor, std::uint64_t resting, char reason, std::string_view instrument = instrument_symbol) {
    ipc_ns::trade_broken_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.execution_id.set(execution);
    m.aggressor_order_id.set(aggressor);
    m.resting_order_id.set(resting);
    m.break_reason.set(static_cast<std::remove_cvref_t<decltype(m.break_reason.get().value())>>(reason));
    detail_ipc::emit_framed(emit, IpcType::TradeBroken, m);
}
// The trading day's events: an order the engine did not take - the phase it arrived in does not,
// or its type or time in force is none the engine has - and why, and what the venue announces as
// its phases change, each as the internal messaging model codes it.
template <typename Emit>
inline void emit_refused(Emit& emit, const matching::Order& order, Refusal reason, std::string_view instrument = instrument_symbol) {
    ipc_ns::order_refused_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument);
    m.refusal_reason.set(static_cast<std::remove_cvref_t<decltype(m.refusal_reason.get().value())>>(reason == Refusal::Phase ? 'E' : reason == Refusal::Peg ? 'P' : reason == Refusal::Type ? 'T' : reason == Refusal::Lifetime ? 'O' : reason == Refusal::Cross ? 'C' : reason == Refusal::Slid ? 'G' : reason == Refusal::Unreferenced ? 'N' : reason == Refusal::Aggressive ? 'A' : reason == Refusal::FatFinger ? 'F' : 'L'));
    detail_ipc::emit_framed(emit, IpcType::OrderRefused, m);
}

inline char announced(matching::Announcement announcement) {
    switch (announcement) {
        case matching::Announcement::StartOfMessages: return 'O';
        case matching::Announcement::StartOfSystemHours: return 'S';
        case matching::Announcement::Halted: return 'H';
        case matching::Announcement::StartOfMarketHours: return 'Q';
        case matching::Announcement::Quoting: return 'q';
        case matching::Announcement::Postponed: return 'W';
        case matching::Announcement::Resumed: return 'T';
        case matching::Announcement::EndOfMarketHours: return 'M';
        case matching::Announcement::EndOfSystemHours: return 'E';
        case matching::Announcement::EndOfMessages: return 'C';
        case matching::Announcement::Extended: return 'X';
    }
    return '?';
}

template <typename Emit>
inline void emit_announced(Emit& emit, matching::Announcement announcement, std::string_view reason, std::uint64_t release, std::int64_t collar_reference, std::int64_t collar_upper, std::int64_t collar_lower, std::uint32_t collar_extension, std::string_view instrument = instrument_symbol) {
    ipc_ns::phase_announced_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.reason.set(reason);
    m.release.set(release);
    m.collar_reference.set(static_cast<std::remove_cvref_t<decltype(m.collar_reference.get().value())>>(collar_reference));
    m.collar_upper.set(static_cast<std::remove_cvref_t<decltype(m.collar_upper.get().value())>>(collar_upper));
    m.collar_lower.set(static_cast<std::remove_cvref_t<decltype(m.collar_lower.get().value())>>(collar_lower));
    m.collar_extension.set(static_cast<std::remove_cvref_t<decltype(m.collar_extension.get().value())>>(collar_extension));
    m.announcement.set(static_cast<std::remove_cvref_t<decltype(m.announcement.get().value())>>(announced(announcement)));
    detail_ipc::emit_framed(emit, IpcType::PhaseAnnounced, m);
}

// The day's request of the engine: a change of phase, framed onto the day's link.
// Of one instrument's day where it names one, and of every day where it names none.
template <typename Emit>
inline void emit_phase_request(Emit& emit, PhaseChange change, std::string_view reason = {}, std::string_view instrument = {}, std::uint64_t release = 0) {
    ipc_ns::phase_request_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.reason.set(reason);
    m.release.set(release);
    m.phase_change.set(static_cast<std::remove_cvref_t<decltype(m.phase_change.get().value())>>(requested(change)));
    detail_ipc::emit_framed(emit, IpcType::PhaseRequest, m);
}

// An auction the day uncrossed: which one, the price it crossed at, the shares that traded there
// - none when nothing crossed, which is still a cross completed - and the match number of its
// print, where the internal messaging carries one.
inline char crossed(matching::Cross cross) {
    switch (cross) {
        case matching::Cross::Opening: return 'O';
        case matching::Cross::Intraday: return 'I';
        case matching::Cross::Closing: return 'C';
        case matching::Cross::Extended: return 'A';
        default: break;
    }
    return '?';
}

template <typename Emit>
inline void emit_crossed(Emit& emit, matching::Cross cross, const matching::Indicative& at, std::uint64_t match, std::string_view instrument = instrument_symbol) {
    ipc_ns::cross_completed_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.cross.set(static_cast<std::remove_cvref_t<decltype(m.cross.get().value())>>(crossed(cross)));
    m.price.set(at.price);
    m.quantity.set(static_cast<std::uint32_t>(at.qty));
    m.execution_id.set(match);
    detail_ipc::emit_framed(emit, IpcType::CrossCompleted, m);
}

// The day's question of the engine: what would the auction it collects for cross at now?
// Asked for early, the answer states no indicative prices; a question that does not say is not.
template <typename Emit>
inline void emit_imbalance_request(Emit& emit, bool early = false) {
    ipc_ns::imbalance_request_message m{};
    m.timestamp.set(send_time_nanos());
    m.early.set(static_cast<std::remove_cvref_t<decltype(m.early.get().value())>>(early ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::ImbalanceRequest, m);
}

// The engine's answer: which auction, the price it would cross at, the shares that would pair
// there, and those left over and on which side - insufficient when nothing would cross, and no
// side when nothing is left over.
inline char imbalanced(const matching::Indicative& at) {
    if (at.qty == 0) return 'O';
    if (at.surplus == 0) return 'N';
    return at.buyers ? 'B' : 'S';
}
// The Extended Trading Close's: the side of the shares it leaves unmatched, whether or not any have paired
// yet, and none where it leaves none - its ETC Imbalance's "buy/sell direction" (Nasdaq Equity 4, Rule
// 4755(a)(8)).
inline char extended_imbalanced(const matching::Indicative& at) {
    if (at.surplus == 0) return 'N';
    return at.buyers ? 'B' : 'S';
}

// How far the Near price is from the Current Reference Price, by the bands of whole percent of it the
// venue's Price Variation marks - the band of the highest percent the distance reaches - and none
// where either price is none: asked for early, or where nothing pairs.
inline char varied(std::int64_t near_price, std::int64_t reference) {
    if (near_price <= 0 || reference <= 0) return ' ';
    const std::int64_t apart = near_price > reference ? near_price - reference : reference - near_price;
    if (apart * 100 >= 30 * reference) return 'C';
    if (apart * 100 >= 20 * reference) return 'B';
    if (apart * 100 >= 10 * reference) return 'A';
    if (apart * 100 >= 9 * reference) return '9';
    if (apart * 100 >= 8 * reference) return '8';
    if (apart * 100 >= 7 * reference) return '7';
    if (apart * 100 >= 6 * reference) return '6';
    if (apart * 100 >= 5 * reference) return '5';
    if (apart * 100 >= 4 * reference) return '4';
    if (apart * 100 >= 3 * reference) return '3';
    if (apart * 100 >= 2 * reference) return '2';
    if (apart * 100 >= 1 * reference) return '1';
    return 'L';
}

template <typename Emit>
inline void emit_indicated(Emit& emit, matching::Cross cross, const matching::Indicative& at, std::string_view instrument = instrument_symbol) {
    ipc_ns::imbalance_indicated_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument);
    m.cross.set(static_cast<std::remove_cvref_t<decltype(m.cross.get().value())>>(crossed(cross)));
    m.price.set(at.near_price);
    m.paired_quantity.set(at.qty);
    m.imbalance_quantity.set(at.surplus);
    m.imbalance_direction.set(static_cast<std::remove_cvref_t<decltype(m.imbalance_direction.get().value())>>(cross == matching::Cross::Extended ? extended_imbalanced(at) : imbalanced(at)));
    m.far_price.set(at.far_price);
    m.near_price.set(at.near_price);
    m.reference_price.set(at.price);
    m.price_variation.set(static_cast<std::remove_cvref_t<decltype(m.price_variation.get().value())>>(varied(at.near_price, at.price)));
    m.early.set(static_cast<std::remove_cvref_t<decltype(m.early.get().value())>>(at.early ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::ImbalanceIndicated, m);
}

// The venue's time, sent to the engine each second its day runs, where the day sets timers: the
// engine keeps no clock of its own, and its timers come due by the time this is put in order at.
template <typename Emit>
inline void emit_clock(Emit& emit) {
    ipc_ns::clock_message m{};
    m.timestamp.set(send_time_nanos());
    detail_ipc::emit_framed(emit, IpcType::Clock, m);
}

// A cancel the engine holds back: its order waits for a cross, and is canceled once the cross is done.
template <typename Emit>
inline void emit_cancel_pending(Emit& emit, std::uint64_t order, std::string_view instrument = instrument_symbol) {
    ipc_ns::cancel_pending_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order);
    m.symbol.set(instrument);
    detail_ipc::emit_framed(emit, IpcType::CancelPending, m);
}

// A reduce the engine refuses: its order waits for a cross, and its cancels wait with it.
template <typename Emit>
inline void emit_reduce_refused(Emit& emit, std::uint64_t order, std::string_view instrument = instrument_symbol) {
    ipc_ns::reduce_refused_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order);
    m.symbol.set(instrument);
    detail_ipc::emit_framed(emit, IpcType::ReduceRefused, m);
}}
