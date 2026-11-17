#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
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
#include "structs/BookRequestMessage.hpp"
#include "structs/BookOrderMessage.hpp"
#include "structs/BookEndMessage.hpp"
#include "structs/CancelRefusedMessage.hpp"
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

namespace ipc_ns = iex::iexequities::matchingengine::ipc::v2026;

// The price the trading day's uncross is drawn towards when two prices would cross as much: the
// instrument's reference, as the venue declares it.
inline constexpr std::int64_t instrument_reference = 10000;

// The changes of phase the trading day answers, as its chart names them.
enum class PhaseChange : std::uint8_t { PreOpen, OpeningLockIn, Open, OpeningLockOut, Halt, PreClose, Resume, ClosingLockOut, Close, End };

// A change as the internal messaging model codes it, and back.
inline char requested(PhaseChange change) {
    switch (change) {
        case PhaseChange::PreOpen: return 'P';
        case PhaseChange::OpeningLockIn: return 'I';
        case PhaseChange::Open: return 'O';
        case PhaseChange::OpeningLockOut: return 'K';
        case PhaseChange::Halt: return 'H';
        case PhaseChange::PreClose: return 'L';
        case PhaseChange::Resume: return 'R';
        case PhaseChange::ClosingLockOut: return 'X';
        case PhaseChange::Close: return 'C';
        case PhaseChange::End: return 'E';
    }
    return '?';
}

inline std::optional<PhaseChange> phase_change(char code) {
    switch (code) {
        case 'P': return PhaseChange::PreOpen;
        case 'I': return PhaseChange::OpeningLockIn;
        case 'O': return PhaseChange::Open;
        case 'K': return PhaseChange::OpeningLockOut;
        case 'H': return PhaseChange::Halt;
        case 'L': return PhaseChange::PreClose;
        case 'R': return PhaseChange::Resume;
        case 'X': return PhaseChange::ClosingLockOut;
        case 'C': return PhaseChange::Close;
        case 'E': return PhaseChange::End;
    }
    return std::nullopt;
}

inline const char* phase_name(PhaseChange change) {
    switch (change) {
        case PhaseChange::PreOpen: return "PreOpen";
        case PhaseChange::OpeningLockIn: return "OpeningLockIn";
        case PhaseChange::Open: return "Open";
        case PhaseChange::OpeningLockOut: return "OpeningLockOut";
        case PhaseChange::Halt: return "Halt";
        case PhaseChange::PreClose: return "PreClose";
        case PhaseChange::Resume: return "Resume";
        case PhaseChange::ClosingLockOut: return "ClosingLockOut";
        case PhaseChange::Close: return "Close";
        case PhaseChange::End: return "End";
    }
    return "?";
}

// What serves a Phase Request: a trading day, which answers every change its chart names. A book
// on its own has no phases to change.
template <typename Day>
concept TradingDay = requires(Day& day) { day.preopen(); day.openinglockin(); day.open(); day.openinglockout(); day.halt(); day.preclose(); day.resume(); day.closinglockout(); day.close(); day.end(); };

// Make the change the day was asked for: false when the phase it is in has no such change.
template <TradingDay Day>
bool change_phase(Day& day, PhaseChange change) {
    bool moved = false;
    switch (change) {
        case PhaseChange::PreOpen: moved = day.preopen(); break;
        case PhaseChange::OpeningLockIn: moved = day.openinglockin(); break;
        case PhaseChange::Open: moved = day.open(); break;
        case PhaseChange::OpeningLockOut: moved = day.openinglockout(); break;
        case PhaseChange::Halt: moved = day.halt(); break;
        case PhaseChange::PreClose: moved = day.preclose(); break;
        case PhaseChange::Resume: moved = day.resume(); break;
        case PhaseChange::ClosingLockOut: moved = day.closinglockout(); break;
        case PhaseChange::Close: moved = day.close(); break;
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
        default: break;
    }
    return '?';
}

// Whether an order is a market order (true) or a limit (false), by the code of its type, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<bool> market_of(Code code) {
    switch (static_cast<long long>(code)) {
        case '1': return true;
        case '2': return false;
        case 'P': return false;
        default: return std::nullopt;
    }
}

// How long an order lives, by the code of its time in force, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Lifetime> lifetime_of(Code code) {
    switch (static_cast<long long>(code)) {
        case '0': return matching::Lifetime::Session;
        case '3': return matching::Lifetime::Immediate;
        case '4': return matching::Lifetime::Whole;
        case '5': return matching::Lifetime::Extended;
        case '6': return matching::Lifetime::Timed;
        case 'M': return matching::Lifetime::Hours;
        case '2': return matching::Lifetime::Opening;
        case '7': return matching::Lifetime::Closing;
        default: return std::nullopt;
    }
}

// What a pegged order follows, none for one that is not pegged, by the code of its peg, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Peg> peg_of(Code code) {
    switch (static_cast<long long>(code)) {
        case 'N': return matching::Peg::None;
        case 'M': return matching::Peg::Midpoint;
        case 'o': return matching::Peg::Near;
        case 'P': return matching::Peg::Far;
        default: return std::nullopt;
    }
}

// How an order is handled beside its type, by the code of its instruction, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Instruction> instruction_of(Code code) {
    switch (static_cast<long long>(code)) {
        case 'i': return matching::Instruction::Unrouted;
        case 'u': return matching::Instruction::Routed;
        case 'f': return matching::Instruction::Sweep;
        case 'y': return matching::Instruction::TradeAt;
        case 'j': return matching::Instruction::Discretionary;
        default: return std::nullopt;
    }
}

// What is done of two orders of one owner that would trade, by the code of an order's strategy, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Prevention> prevention_of(Code code) {
    switch (static_cast<long long>(code)) {
        case 'N': return matching::Prevention::Newest;
        case 'O': return matching::Prevention::Oldest;
        case 'B': return matching::Prevention::Both;
        case 'S': return matching::Prevention::Smaller;
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
        default: return std::nullopt;
    }
}


// Why the engine refused an order: its type, or how long it lives, is none the engine has.
enum class Refusal { Type, Lifetime, Phase, Cross, Quantity };

// What a server built straight over a book - a test's - does with an order it refuses, with
// nobody to tell: says so, as the engine does of an order for an instrument it does not trade.
struct UnheardRefusal {
    template <typename... Instrument>
    void operator()(const matching::Order& order, Refusal, Instrument...) const {
        std::printf("REFUSED order %llu: nobody listens for refusals here\n", (unsigned long long)order.id);
    }
                 template <typename... Instrument>
                 void cancel(std::uint64_t id, Instrument...) const {
                     std::printf("REFUSED cancel of %llu: nobody listens for refusals here\n", (unsigned long long)id);
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
    static constexpr char CancelRefused      = 'K';
    static constexpr char Clock              = 'L';
    static constexpr char BookRequest        = 'O';
    static constexpr char BookOrder          = 'F';
    static constexpr char BookEnd            = 'V';
};

// The instrument the engine trades — stamped on the symbol field of every Ipc message.
inline constexpr std::string_view instrument_symbol = "ZIEXT";

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
        case IpcType::Clock: { if (length < 3 + sizeof(ipc_ns::clock_message)) return false; auto* m = ipc_ns::clock_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
        case IpcType::BookRequest: { if (length < 3 + sizeof(ipc_ns::book_request_message)) return false; auto* m = ipc_ns::book_request_message::parse(data); m->sequence.set(sequence); m->timestamp.set(time); return true; }
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
        case IpcType::Clock: return bytes.size() < 3 + sizeof(ipc_ns::clock_message) ? 0 : ipc_ns::clock_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
        case IpcType::BookRequest: return bytes.size() < 3 + sizeof(ipc_ns::book_request_message) ? 0 : ipc_ns::book_request_message::parse(const_cast<std::byte*>(data))->sequence.get().value();
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
        case IpcType::Clock: return bytes.size() < 3 + sizeof(ipc_ns::clock_message) ? 0 : static_cast<std::int64_t>(ipc_ns::clock_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
        case IpcType::BookRequest: return bytes.size() < 3 + sizeof(ipc_ns::book_request_message) ? 0 : static_cast<std::int64_t>(ipc_ns::book_request_message::parse(const_cast<std::byte*>(data))->timestamp.get().value());
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

    void on_new_order(const matching::Order& order) {
        ipc_ns::new_order_message m{};
        m.timestamp.set(send_time_nanos());
        m.symbol.set(instrument_symbol);
        m.order_id.set(order.id);
        m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
        m.price.set(order.peg == matching::Peg::None ? order.price : order.limit == matching::market(order.side) ? 0 : order.limit);
        m.quantity.set(static_cast<std::uint32_t>(order.qty));
        m.order_type.set(static_cast<std::remove_cvref_t<decltype(m.order_type.get().value())>>(order.peg != matching::Peg::None ? 'P' : order.price == matching::market(order.side) ? '1' : '2'));
        m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Session ? '0' : order.lifetime == matching::Lifetime::Immediate ? '3' : order.lifetime == matching::Lifetime::Whole ? '4' : order.lifetime == matching::Lifetime::Extended ? '5' : order.lifetime == matching::Lifetime::Timed ? '6' : order.lifetime == matching::Lifetime::Hours ? 'M' : order.lifetime == matching::Lifetime::Opening ? '2' : '7'));
        m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
        m.cross_type.set(static_cast<std::remove_cvref_t<decltype(m.cross_type.get().value())>>(order.only == matching::Cross::Intraday ? 'H' : order.only == matching::Cross::Closing ? 'C' : order.only == matching::Cross::Opening ? 'O' : 'N'));
        m.peg.set(static_cast<std::remove_cvref_t<decltype(m.peg.get().value())>>(order.peg == matching::Peg::Far ? 'P' : order.peg == matching::Peg::Near ? 'o' : order.peg == matching::Peg::Midpoint ? 'M' : 'N'));
        m.expire_time.set(static_cast<std::remove_cvref_t<decltype(m.expire_time.get().value())>>(order.expires));
        m.instruction.set(static_cast<std::remove_cvref_t<decltype(m.instruction.get().value())>>(order.instruction == matching::Instruction::Discretionary ? 'j' : order.instruction == matching::Instruction::TradeAt ? 'y' : order.instruction == matching::Instruction::Sweep ? 'f' : order.instruction == matching::Instruction::Routed ? 'u' : 'i'));
        detail_ipc::emit_framed(emit_, IpcType::NewOrder, m);
    }

    bool on_cancel(std::uint64_t id) {
        ipc_ns::cancel_order_message m{};
        m.order_id.set(id);
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
                const std::optional<bool> market = market_of(m->order_type.get().value());
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                const std::optional<matching::Cross> only = cross_of(m->cross_type.get().value());
                const std::optional<matching::Peg> peg = peg_of(m->peg.get().value());
                const bool pegged = static_cast<char>(m->order_type.get().value()) == 'P';
                const std::optional<matching::Instruction> instruction = instruction_of(m->instruction.get().value());
                if (m->quantity.get().value() == 0 || !market || !peg || (*peg != matching::Peg::None) != pegged || !instruction || !lifetime || !only) {
                    matching::Order refused{};
                    refused.id    = m->order_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value();
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, m->quantity.get().value() == 0 ? Refusal::Quantity : !market ? Refusal::Type : !peg || (*peg != matching::Peg::None) != pegged ? Refusal::Type : !instruction ? Refusal::Type : !lifetime ? Refusal::Lifetime : Refusal::Cross);
                    return true;
                }
                engine_.on_new_order(matching::Order{
                    m->order_id.get().value(),
                    side,
                    *market || (pegged && m->price.get().value() == 0) ? matching::market(side) : m->price.get().value(),
                    m->quantity.get().value(),
                    *lifetime,
                    static_cast<char>(m->display.get().value()) != 'N',
                    *only,
                    *peg,
                    *market || (pegged && m->price.get().value() == 0) ? matching::market(side) : m->price.get().value(),
                    static_cast<std::int64_t>(m->peg_difference.get().value()),
                    static_cast<std::int64_t>(m->expire_time.get().value()),
                    *instruction,
                    static_cast<std::uint64_t>(m->min_qty.get().value()),
                    static_cast<std::uint64_t>(m->self_match_group.get().value()),
                    prevention_of(m->self_match_modifier.get().value()).value_or(matching::Prevention::None),
                });
                return true;
            }
            case IpcType::CancelOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::cancel_order_message)) return false;
                const auto* m = ipc_ns::cancel_order_message::parse(const_cast<std::byte*>(data));
                // One its phase refuses, or no longer in the book, is told as refused, so its client is answered.
                if (!engine_.on_cancel(m->order_id.get().value())) on_refused_.cancel(m->order_id.get().value());
                return true;
            }
            case IpcType::ReplaceOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::replace_order_message)) return false;
                const auto* m = ipc_ns::replace_order_message::parse(const_cast<std::byte*>(data));
                const matching::Side side = static_cast<char>(m->side.get().value()) == 'B' ? matching::Side::Buy : matching::Side::Sell;
                const std::optional<bool> market = market_of(m->order_type.get().value());
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                const std::optional<matching::Cross> only = cross_of(m->cross_type.get().value());
                const std::optional<matching::Peg> peg = peg_of(m->peg.get().value());
                const bool pegged = static_cast<char>(m->order_type.get().value()) == 'P';
                const std::optional<matching::Instruction> instruction = instruction_of(m->instruction.get().value());
                if (m->quantity.get().value() == 0 || !market || !peg || (*peg != matching::Peg::None) != pegged || !instruction || !lifetime || !only) {
                    matching::Order refused{};
                    refused.id    = m->replacement_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value();
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, m->quantity.get().value() == 0 ? Refusal::Quantity : !market ? Refusal::Type : !peg || (*peg != matching::Peg::None) != pegged ? Refusal::Type : !instruction ? Refusal::Type : !lifetime ? Refusal::Lifetime : Refusal::Cross);
                    return true;
                }
                const matching::Order order = matching::Order{
                    m->replacement_id.get().value(),
                    side,
                    *market || (pegged && m->price.get().value() == 0) ? matching::market(side) : m->price.get().value(),
                    m->quantity.get().value(),
                    *lifetime,
                    static_cast<char>(m->display.get().value()) != 'N',
                    *only,
                    *peg,
                    *market || (pegged && m->price.get().value() == 0) ? matching::market(side) : m->price.get().value(),
                    static_cast<std::int64_t>(m->peg_difference.get().value()),
                    0,
                    *instruction,
                    static_cast<std::uint64_t>(m->min_qty.get().value()),
                    static_cast<std::uint64_t>(m->self_match_group.get().value()),
                    prevention_of(m->self_match_modifier.get().value()).value_or(matching::Prevention::None),
                };
                // To the day, which replaces; an engine without one - a test's bare book - replaces nothing.
                if constexpr (requires { engine_.on_replace(order, m->order_id.get().value()); })
                    engine_.on_replace(order, m->order_id.get().value());
                return true;
            }
            case IpcType::PhaseRequest: {
                if constexpr (TradingDay<Engine>) {
                    if (bytes.size() < 3 + sizeof(ipc_ns::phase_request_message)) return false;
                    const auto* m = ipc_ns::phase_request_message::parse(const_cast<std::byte*>(data));
                    const auto change = phase_change(static_cast<char>(m->phase_change.get().value()));
                    if (!change) return false;
                    change_phase(engine_, *change);
                    return true;
                } else {
                    return false;
                }
            }
            case IpcType::ImbalanceRequest: {
                if constexpr (requires { engine_.indicate(); }) {
                    if (bytes.size() < 3 + sizeof(ipc_ns::imbalance_request_message)) return false;
                    engine_.indicate();
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
            default:
                return false;
        }
    }

    // Did the message just read ask for the engine's book? Asked once, answered once.
    bool book_asked() { const bool asked = book_asked_; book_asked_ = false; return asked; }

    // The place of the message just read in the order its sequencer put everything in; 0 where none numbered it.
    std::uint64_t book_sequence() const { return sequenced_; }
private:
    Engine& engine_;
    bool book_asked_ = false;   // the message just read asked for the engine's book
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
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreEvent, typename OnAccepted = IgnoreEvent, typename OnEliminated = IgnoreEvent, typename OnRefused = IgnoreEvent, typename OnAnnounced = IgnoreEvent, typename OnCrossed = IgnoreEvent, typename OnIndicated = IgnoreEvent, typename OnReplaced = IgnoreEvent, typename OnCancelRefused = IgnoreEvent>
class IpcEventServer {
public:
    IpcEventServer(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_() {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled) {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnAccepted on_accepted, OnEliminated on_eliminated, OnRefused on_refused, OnAnnounced on_announced, OnCrossed on_crossed, OnIndicated on_indicated, OnReplaced on_replaced, OnCancelRefused on_cancel_refused)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_accepted_(on_accepted), on_eliminated_(on_eliminated), on_refused_(on_refused), on_announced_(on_announced), on_crossed_(on_crossed), on_indicated_(on_indicated), on_replaced_(on_replaced), on_cancel_refused_(on_cancel_refused) {}
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
            case IpcType::CancelRefused: {
                if (bytes.size() < 3 + sizeof(ipc_ns::cancel_refused_message)) return false;
                on_cancel_refused_(*ipc_ns::cancel_refused_message::parse(const_cast<std::byte*>(data)));
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
    OnCancelRefused on_cancel_refused_;
};

// Event-side encoders: matching-engine events as Ipc messages (consumed by feed
// publishers / the gateway / recorders in a multi-process deployment).
template <typename Emit>
inline void emit_trade(Emit& emit, const matching::Trade& trade) {
    ipc_ns::trade_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument_symbol);
    m.aggressor_order_id.set(trade.aggressor_id);
    m.resting_order_id.set(trade.resting_id);
    m.execution_id.set(trade.match_id);
    m.price.set(trade.price);
    m.quantity.set(static_cast<std::uint32_t>(trade.qty));
    m.auction.set(static_cast<std::remove_cvref_t<decltype(m.auction.get().value())>>(auctioned(trade.cross)));
    m.resting_display.set(static_cast<std::remove_cvref_t<decltype(m.resting_display.get().value())>>(trade.resting_shown ? 'Y' : 'N'));
    m.aggressor_display.set(static_cast<std::remove_cvref_t<decltype(m.aggressor_display.get().value())>>(trade.aggressor_shown ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::Trade, m);
}

template <typename Emit>
inline void emit_rested(Emit& emit, const matching::Order& order) {
    ipc_ns::order_rested_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument_symbol);
    m.order_id.set(order.id);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.price);
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::OrderRested, m);
}

template <typename Emit>
inline void emit_canceled(Emit& emit, const matching::Order& order) {
    ipc_ns::order_canceled_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.prevented.set(static_cast<std::remove_cvref_t<decltype(m.prevented.get().value())>>(order.prevented.qty > 0 ? 'Y' : 'N'));
    if (order.prevented.qty > 0) m.prevented_liquidity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_liquidity.get().value())>>(order.prevented.resting ? 'A' : 'R'));
    m.prevented_display.set(static_cast<std::remove_cvref_t<decltype(m.prevented_display.get().value())>>(order.prevented.shown ? 'Y' : 'N'));
    m.prevented_quantity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_quantity.get().value())>>(order.prevented.qty));
    m.prevented_price.set(static_cast<std::remove_cvref_t<decltype(m.prevented_price.get().value())>>(order.prevented.price));
    m.prevented_against.set(static_cast<std::remove_cvref_t<decltype(m.prevented_against.get().value())>>(order.prevented.against));
    detail_ipc::emit_framed(emit, IpcType::OrderCanceled, m);
}

template <typename Emit>
inline void emit_accepted(Emit& emit, const matching::Order& order) {
    ipc_ns::order_accepted_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    detail_ipc::emit_framed(emit, IpcType::OrderAccepted, m);
}


template <typename Emit>
inline void emit_eliminated(Emit& emit, const matching::Order& order) {
    ipc_ns::order_eliminated_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.peg == matching::Peg::None ? order.price : order.limit == matching::market(order.side) ? 0 : order.limit);
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Session ? '0' : order.lifetime == matching::Lifetime::Immediate ? '3' : order.lifetime == matching::Lifetime::Whole ? '4' : order.lifetime == matching::Lifetime::Extended ? '5' : order.lifetime == matching::Lifetime::Timed ? '6' : order.lifetime == matching::Lifetime::Hours ? 'M' : order.lifetime == matching::Lifetime::Opening ? '2' : '7'));
    detail_ipc::emit_framed(emit, IpcType::OrderEliminated, m);
}


template <typename Emit>
inline void emit_replaced(Emit& emit, std::uint64_t replaced, const matching::Order& order) {
    ipc_ns::order_replaced_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(replaced);
    m.replacement_id.set(order.id);
    m.symbol.set(instrument_symbol);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.peg == matching::Peg::None ? order.price : order.limit == matching::market(order.side) ? 0 : order.limit);
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::OrderReplaced, m);
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
inline void emit_book_order(Emit& emit, const matching::Order& order, char where, std::uint64_t sequence) {
    ipc_ns::book_order_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument_symbol);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.peg == matching::Peg::None ? order.price : order.limit == matching::market(order.side) ? 0 : order.limit);
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
// The trading day's events: an order the engine did not take - the phase it arrived in does not,
// or its type or time in force is none the engine has - and why, and what the venue announces as
// its phases change, each as the internal messaging model codes it.
template <typename Emit>
inline void emit_refused(Emit& emit, const matching::Order& order, Refusal reason) {
    ipc_ns::order_refused_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.refusal_reason.set(static_cast<std::remove_cvref_t<decltype(m.refusal_reason.get().value())>>(reason == Refusal::Type ? 'T' : reason == Refusal::Lifetime ? 'L' : reason == Refusal::Phase ? 'P' : reason == Refusal::Cross ? 'C' : 'Q'));
    detail_ipc::emit_framed(emit, IpcType::OrderRefused, m);
}

inline char announced(matching::Announcement announcement) {
    switch (announcement) {
        case matching::Announcement::StartOfMessages: return 'O';
        case matching::Announcement::StartOfSystemHours: return 'S';
        case matching::Announcement::StartOfMarketHours: return 'Q';
        case matching::Announcement::Halted: return 'H';
        case matching::Announcement::Resumed: return 'T';
        case matching::Announcement::EndOfMarketHours: return 'M';
        case matching::Announcement::EndOfSystemHours: return 'E';
        case matching::Announcement::EndOfMessages: return 'C';
    }
    return '?';
}

template <typename Emit>
inline void emit_announced(Emit& emit, matching::Announcement announcement) {
    ipc_ns::phase_announced_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument_symbol);
    m.announcement.set(static_cast<std::remove_cvref_t<decltype(m.announcement.get().value())>>(announced(announcement)));
    detail_ipc::emit_framed(emit, IpcType::PhaseAnnounced, m);
}

// The day's request of the engine: a change of phase, framed onto the day's link.
template <typename Emit>
inline void emit_phase_request(Emit& emit, PhaseChange change) {
    ipc_ns::phase_request_message m{};
    m.timestamp.set(send_time_nanos());
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
        default: break;
    }
    return '?';
}

template <typename Emit>
inline void emit_crossed(Emit& emit, matching::Cross cross, const matching::Indicative& at, std::uint64_t match) {
    ipc_ns::cross_completed_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument_symbol);
    m.cross.set(static_cast<std::remove_cvref_t<decltype(m.cross.get().value())>>(crossed(cross)));
    m.price.set(at.price);
    m.quantity.set(static_cast<std::uint32_t>(at.qty));
    m.execution_id.set(match);
    detail_ipc::emit_framed(emit, IpcType::CrossCompleted, m);
}

// The day's question of the engine: what would the auction it collects for cross at now?
template <typename Emit>
inline void emit_imbalance_request(Emit& emit) {
    ipc_ns::imbalance_request_message m{};
    m.timestamp.set(send_time_nanos());
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

template <typename Emit>
inline void emit_indicated(Emit& emit, matching::Cross cross, const matching::Indicative& at) {
    ipc_ns::imbalance_indicated_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument_symbol);
    m.cross.set(static_cast<std::remove_cvref_t<decltype(m.cross.get().value())>>(crossed(cross)));
    m.price.set(at.price);
    m.paired_quantity.set(at.qty);
    m.imbalance_quantity.set(at.surplus);
    m.imbalance_direction.set(static_cast<std::remove_cvref_t<decltype(m.imbalance_direction.get().value())>>(imbalanced(at)));
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

// A cancel the engine did not take - its phase refuses it, or the order is no longer in the book.
template <typename Emit>
inline void emit_cancel_refused(Emit& emit, std::uint64_t order) {
    ipc_ns::cancel_refused_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order);
    detail_ipc::emit_framed(emit, IpcType::CancelRefused, m);
}}
