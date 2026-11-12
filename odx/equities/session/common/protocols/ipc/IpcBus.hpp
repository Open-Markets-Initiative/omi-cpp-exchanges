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
#include "structs/OrderRefusedMessage.hpp"
#include "structs/PhaseAnnouncedMessage.hpp"
#include "structs/PhaseRequestMessage.hpp"
#include "structs/CrossCompletedMessage.hpp"
#include "structs/ImbalanceRequestMessage.hpp"
#include "structs/ImbalanceIndicatedMessage.hpp"
#include "MatchingEngine.hpp"
#include "TradingPhases.hpp"

namespace sim {

namespace ipc_ns = odx::odxequities::matchingengine::ipc::v2026;

// The changes of phase the trading day answers, as its chart names them.
enum class PhaseChange : std::uint8_t { Start, PreOpen, Open, Halt, Close, Resume, End };

// A change as the internal messaging model codes it, and back.
inline char requested(PhaseChange change) {
    switch (change) {
        case PhaseChange::Start: return 'S';
        case PhaseChange::PreOpen: return 'P';
        case PhaseChange::Open: return 'O';
        case PhaseChange::Halt: return 'H';
        case PhaseChange::Close: return 'C';
        case PhaseChange::Resume: return 'R';
        case PhaseChange::End: return 'E';
    }
    return '?';
}

inline std::optional<PhaseChange> phase_change(char code) {
    switch (code) {
        case 'S': return PhaseChange::Start;
        case 'P': return PhaseChange::PreOpen;
        case 'O': return PhaseChange::Open;
        case 'H': return PhaseChange::Halt;
        case 'C': return PhaseChange::Close;
        case 'R': return PhaseChange::Resume;
        case 'E': return PhaseChange::End;
    }
    return std::nullopt;
}

inline const char* phase_name(PhaseChange change) {
    switch (change) {
        case PhaseChange::Start: return "Start";
        case PhaseChange::PreOpen: return "PreOpen";
        case PhaseChange::Open: return "Open";
        case PhaseChange::Halt: return "Halt";
        case PhaseChange::Close: return "Close";
        case PhaseChange::Resume: return "Resume";
        case PhaseChange::End: return "End";
    }
    return "?";
}

// What serves a Phase Request: a trading day, which answers every change its chart names. A book
// on its own has no phases to change.
template <typename Day>
concept TradingDay = requires(Day& day) { day.start(); day.preopen(); day.open(); day.halt(); day.close(); day.resume(); day.end(); };

// Make the change the day was asked for: false when the phase it is in has no such change.
template <TradingDay Day>
bool change_phase(Day& day, PhaseChange change) {
    bool moved = false;
    switch (change) {
        case PhaseChange::Start: moved = day.start(); break;
        case PhaseChange::PreOpen: moved = day.preopen(); break;
        case PhaseChange::Open: moved = day.open(); break;
        case PhaseChange::Halt: moved = day.halt(); break;
        case PhaseChange::Close: moved = day.close(); break;
        case PhaseChange::Resume: moved = day.resume(); break;
        case PhaseChange::End: moved = day.end(); break;
    }
    std::printf("PHASE %s %s\n", moved ? "moved" : "refused", phase_name(change));
    return moved;
}

// How long an order lives, by the code of its time in force, as the venue codes it; none for a code that means nothing to the engine yet.
template <typename Code>
constexpr std::optional<matching::Lifetime> lifetime_of(Code code) {
    switch (static_cast<long long>(code)) {
        case 0: return matching::Lifetime::Immediate;
        case 99999: return matching::Lifetime::Session;
        default: return std::nullopt;
    }
}


// Why the engine refused an order: its type, or how long it lives, is none the engine has.
enum class Refusal { Lifetime, Phase, Halt };

// What a server built straight over a book - a test's - does with an order it refuses, with
// nobody to tell: says so, as the engine does of an order for an instrument it does not trade.
struct UnheardRefusal {
    template <typename... Instrument>
    void operator()(const matching::Order& order, Refusal, Instrument...) const {
        std::printf("REFUSED order %llu: nobody listens for refusals here\n", (unsigned long long)order.id);
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
};

// The instrument the engine trades — stamped on the symbol field of every Ipc message.
inline constexpr std::string_view instrument_symbol = "9999";

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
        m.orderbook.set(instrument_symbol);
        m.order_id.set(order.id);
        m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
        m.price.set(order.price);
        m.quantity.set(static_cast<std::uint32_t>(order.qty));
        m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Immediate ? 0 : 99999));
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

        switch (bytes[2]) {
            case IpcType::NewOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::new_order_message)) return false;
                const auto* m = ipc_ns::new_order_message::parse(const_cast<std::byte*>(data));
                const matching::Side side = static_cast<char>(m->side.get().value()) == 'B' ? matching::Side::Buy : matching::Side::Sell;
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                if (!lifetime) {
                    matching::Order refused{};
                    refused.id    = m->order_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value();
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, Refusal::Lifetime);
                    return true;
                }
                engine_.on_new_order(matching::Order{
                    m->order_id.get().value(),
                    side,
                    m->price.get().value(),
                    m->quantity.get().value(),
                    *lifetime,
                });
                return true;
            }
            case IpcType::CancelOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::cancel_order_message)) return false;
                const auto* m = ipc_ns::cancel_order_message::parse(const_cast<std::byte*>(data));
                engine_.on_cancel(m->order_id.get().value());
                return true;
            }
            case IpcType::ReplaceOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::replace_order_message)) return false;
                const auto* m = ipc_ns::replace_order_message::parse(const_cast<std::byte*>(data));
                const matching::Side side = static_cast<char>(m->side.get().value()) == 'B' ? matching::Side::Buy : matching::Side::Sell;
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                if (!lifetime) {
                    matching::Order refused{};
                    refused.id    = m->replacement_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value();
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, Refusal::Lifetime);
                    return true;
                }
                const matching::Order order = matching::Order{
                    m->replacement_id.get().value(),
                    side,
                    m->price.get().value(),
                    m->quantity.get().value(),
                    *lifetime,
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
            default:
                return false;
        }
    }

private:
    Engine& engine_;
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
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreEvent, typename OnAccepted = IgnoreEvent, typename OnEliminated = IgnoreEvent, typename OnRefused = IgnoreEvent, typename OnAnnounced = IgnoreEvent, typename OnReplaced = IgnoreEvent>
class IpcEventServer {
public:
    IpcEventServer(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_() {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled) {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnAccepted on_accepted, OnEliminated on_eliminated, OnRefused on_refused, OnAnnounced on_announced, OnReplaced on_replaced)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_accepted_(on_accepted), on_eliminated_(on_eliminated), on_refused_(on_refused), on_announced_(on_announced), on_replaced_(on_replaced) {}
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
            case IpcType::OrderReplaced: {
                if (bytes.size() < 3 + sizeof(ipc_ns::order_replaced_message)) return false;
                on_replaced_(*ipc_ns::order_replaced_message::parse(const_cast<std::byte*>(data)));
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
    OnReplaced  on_replaced_;
};

// Event-side encoders: matching-engine events as Ipc messages (consumed by feed
// publishers / the gateway / recorders in a multi-process deployment).
template <typename Emit>
inline void emit_trade(Emit& emit, const matching::Trade& trade) {
    ipc_ns::trade_message m{};
    m.timestamp.set(send_time_nanos());
    m.orderbook.set(instrument_symbol);
    m.aggressor_order_id.set(trade.aggressor_id);
    m.resting_order_id.set(trade.resting_id);
    m.execution_id.set(trade.match_id);
    m.price.set(trade.price);
    m.quantity.set(static_cast<std::uint32_t>(trade.qty));
    detail_ipc::emit_framed(emit, IpcType::Trade, m);
}

template <typename Emit>
inline void emit_rested(Emit& emit, const matching::Order& order) {
    ipc_ns::order_rested_message m{};
    m.timestamp.set(send_time_nanos());
    m.orderbook.set(instrument_symbol);
    m.order_id.set(order.id);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.price);
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    detail_ipc::emit_framed(emit, IpcType::OrderRested, m);
}

template <typename Emit>
inline void emit_canceled(Emit& emit, const matching::Order& order) {
    ipc_ns::order_canceled_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
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
    m.price.set(order.price);
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Immediate ? 0 : 99999));
    detail_ipc::emit_framed(emit, IpcType::OrderEliminated, m);
}


template <typename Emit>
inline void emit_replaced(Emit& emit, std::uint64_t replaced, const matching::Order& order) {
    ipc_ns::order_replaced_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(replaced);
    m.replacement_id.set(order.id);
    m.orderbook.set(instrument_symbol);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.price);
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    detail_ipc::emit_framed(emit, IpcType::OrderReplaced, m);
}

// The trading day's events: an order the engine did not take - the phase it arrived in does not,
// or its type or time in force is none the engine has - and why, and what the venue announces as
// its phases change, each as the internal messaging model codes it.
template <typename Emit>
inline void emit_refused(Emit& emit, const matching::Order& order, Refusal reason) {
    ipc_ns::order_refused_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.refusal_reason.set(static_cast<std::remove_cvref_t<decltype(m.refusal_reason.get().value())>>(reason == Refusal::Lifetime ? 'O' : reason == Refusal::Phase ? 'R' : 'H'));
    detail_ipc::emit_framed(emit, IpcType::OrderRefused, m);
}

inline char announced(matching::Announcement announcement) {
    switch (announcement) {
        case matching::Announcement::StartOfMessages: return 'O';
        case matching::Announcement::Trading: return 'T';
        case matching::Announcement::StartOfSystemHours: return 'S';
        case matching::Announcement::StartOfMarketHours: return 'Q';
        case matching::Announcement::Suspended: return 'V';
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
    m.orderbook.set(instrument_symbol);
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
}}
