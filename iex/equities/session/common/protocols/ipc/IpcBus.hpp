#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <cstdio>
#include <optional>
#include <string_view>
#include <type_traits>
#include "structs/NewOrderMessage.hpp"
#include "structs/CancelOrderMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/OrderCanceledMessage.hpp"
#include "structs/OrderRefusedMessage.hpp"
#include "structs/PhaseAnnouncedMessage.hpp"
#include "structs/PhaseRequestMessage.hpp"
#include "structs/CrossCompletedMessage.hpp"
#include "structs/ImbalanceRequestMessage.hpp"
#include "structs/ImbalanceIndicatedMessage.hpp"
#include "MatchingEngine.hpp"
#include "TradingPhases.hpp"

namespace sim {

namespace ipc_ns = iex::iexequities::matchingengine::ipc::v1_0;

// The price the trading day's uncross is drawn towards when two prices would cross as much: the
// instrument's reference, as the venue declares it.
inline constexpr std::int64_t instrument_reference = 10000;

// The changes of phase the trading day answers, as its chart names them.
enum class PhaseChange : std::uint8_t { PreOpen, Open, Halt, PreClose, Resume, Close, End };

// A change as the internal messaging model codes it, and back.
inline char requested(PhaseChange change) {
    switch (change) {
        case PhaseChange::PreOpen: return 'P';
        case PhaseChange::Open: return 'O';
        case PhaseChange::Halt: return 'H';
        case PhaseChange::PreClose: return 'L';
        case PhaseChange::Resume: return 'R';
        case PhaseChange::Close: return 'C';
        case PhaseChange::End: return 'E';
    }
    return '?';
}

inline std::optional<PhaseChange> phase_change(char code) {
    switch (code) {
        case 'P': return PhaseChange::PreOpen;
        case 'O': return PhaseChange::Open;
        case 'H': return PhaseChange::Halt;
        case 'L': return PhaseChange::PreClose;
        case 'R': return PhaseChange::Resume;
        case 'C': return PhaseChange::Close;
        case 'E': return PhaseChange::End;
    }
    return std::nullopt;
}

inline const char* phase_name(PhaseChange change) {
    switch (change) {
        case PhaseChange::PreOpen: return "PreOpen";
        case PhaseChange::Open: return "Open";
        case PhaseChange::Halt: return "Halt";
        case PhaseChange::PreClose: return "PreClose";
        case PhaseChange::Resume: return "Resume";
        case PhaseChange::Close: return "Close";
        case PhaseChange::End: return "End";
    }
    return "?";
}

// What serves a Phase Request: a trading day, which answers every change its chart names. A book
// on its own has no phases to change.
template <typename Day>
concept TradingDay = requires(Day& day) { day.preopen(); day.open(); day.halt(); day.preclose(); day.resume(); day.close(); day.end(); };

// Make the change the day was asked for: false when the phase it is in has no such change.
template <TradingDay Day>
bool change_phase(Day& day, PhaseChange change) {
    bool moved = false;
    switch (change) {
        case PhaseChange::PreOpen: moved = day.preopen(); break;
        case PhaseChange::Open: moved = day.open(); break;
        case PhaseChange::Halt: moved = day.halt(); break;
        case PhaseChange::PreClose: moved = day.preclose(); break;
        case PhaseChange::Resume: moved = day.resume(); break;
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
        default: return 0;
    }
}

// Send-time clock (Ipc SendTime action): wall-clock nanoseconds since the POSIX epoch,
// captured as each message is framed onto the bus. Carried through the transforms onto
// every feed message's timestamp field.
inline std::int64_t send_time_nanos() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
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
        m.price.set(order.price);
        m.quantity.set(static_cast<std::uint32_t>(order.qty));
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
template <typename Engine>
class IpcEngineServer {
public:
    explicit IpcEngineServer(Engine& engine) : engine_(engine) {}

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
                engine_.on_new_order(matching::Order{
                    m->order_id.get().value(),
                    static_cast<char>(m->side.get().value()) == 'B' ? matching::Side::Buy : matching::Side::Sell,
                    m->price.get().value(),
                    m->quantity.get().value(),
                });
                return true;
            }
            case IpcType::CancelOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::cancel_order_message)) return false;
                const auto* m = ipc_ns::cancel_order_message::parse(const_cast<std::byte*>(data));
                engine_.on_cancel(m->order_id.get().value());
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
    std::uint64_t sequenced_ = 0;   // the last place in its sequencer's order it read
};

// The event callback of a consumer that does not listen for that event.
struct IgnoreEvent {
    template <typename Message>
    void operator()(const Message&) const {}
};

// Event-side endpoint: decodes engine events off the bus and hands them to the
// consumers (feed publishers / gateway exec reports) as matching types.
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreEvent, typename OnRefused = IgnoreEvent, typename OnAnnounced = IgnoreEvent, typename OnCrossed = IgnoreEvent, typename OnIndicated = IgnoreEvent>
class IpcEventServer {
public:
    IpcEventServer(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_() {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled) {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnRefused on_refused, OnAnnounced on_announced, OnCrossed on_crossed, OnIndicated on_indicated)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_refused_(on_refused), on_announced_(on_announced), on_crossed_(on_crossed), on_indicated_(on_indicated) {}
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
            default:
                return false;
        }
    }

private:
    OnTrade    on_trade_;
    OnRest     on_rest_;
    OnCanceled on_canceled_;
    OnRefused   on_refused_;
    OnAnnounced on_announced_;
    OnCrossed   on_crossed_;
    OnIndicated on_indicated_;
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
    detail_ipc::emit_framed(emit, IpcType::OrderRested, m);
}

template <typename Emit>
inline void emit_canceled(Emit& emit, const matching::Order& order) {
    ipc_ns::order_canceled_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    detail_ipc::emit_framed(emit, IpcType::OrderCanceled, m);
}
// The trading day's events: an order the phase it arrived in does not accept, and what the
// venue announces as its phases change, each as the internal messaging model codes it.
template <typename Emit>
inline void emit_refused(Emit& emit, const matching::Order& order) {
    ipc_ns::order_refused_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
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
}}
