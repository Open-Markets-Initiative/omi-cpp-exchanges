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
#include "structs/ReduceOrderMessage.hpp"
#include "structs/OrderReducedMessage.hpp"
#include "structs/BookRequestMessage.hpp"
#include "structs/BookOrderMessage.hpp"
#include "structs/BookEndMessage.hpp"
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

// The auction a trade ended, as the internal messaging model codes it.
inline char auctioned(matching::Cross cross) {
    switch (cross) {
        case matching::Cross::None: return 'N';
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
        default: return std::nullopt;
    }
}


// Why the engine refused an order: its type, or how long it lives, is none the engine has.
enum class Refusal { Lifetime, Phase, Cross, Peg, Type };

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
};

// The instrument the engine trades — stamped on the symbol field of every Ipc message.
inline constexpr std::string_view instrument_symbol = "ZVZZT";

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
        m.price.set(order.peg == matching::Peg::None ? (order.price == matching::market(order.side) ? 2147483647 : order.price) : order.limit == matching::market(order.side) ? 2147483647 : order.limit);
        m.quantity.set(static_cast<std::uint32_t>(order.qty));
        m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Session ? '0' : order.lifetime == matching::Lifetime::Immediate ? '3' : order.lifetime == matching::Lifetime::Extended ? '5' : '6'));
        m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
        m.cross_type.set(static_cast<std::remove_cvref_t<decltype(m.cross_type.get().value())>>('N'));
        m.peg.set(static_cast<std::remove_cvref_t<decltype(m.peg.get().value())>>(order.peg == matching::Peg::Near ? 'R' : order.peg == matching::Peg::Far ? 'P' : order.peg == matching::Peg::Midpoint ? 'M' : 'L'));
        m.expire_time.set(static_cast<std::remove_cvref_t<decltype(m.expire_time.get().value())>>(order.expires == 0 ? 0 : (order.expires - static_cast<std::int64_t>(send_time_nanos())) / 1000000000));
        detail_ipc::emit_framed(emit_, IpcType::NewOrder, m);
    }

    bool on_cancel(std::uint64_t id) {
        ipc_ns::cancel_order_message m{};
        m.order_id.set(id);
        m.symbol.set(instrument_symbol);
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
                const bool market = m->price.get().value() == 2147483647 || m->price.get().value() == 2000000000;   // its price says so
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                const std::optional<matching::Cross> only = cross_of(m->cross_type.get().value());
                const std::optional<matching::Peg> peg = peg_of(m->peg.get().value());
                const bool pegged = peg && *peg != matching::Peg::None;
                if (!peg || (*peg != matching::Peg::None) != pegged || !lifetime || (!only || *only == matching::Cross::Opening || *only == matching::Cross::Intraday || *only == matching::Cross::Closing)) {
                    matching::Order refused{};
                    refused.id    = m->order_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value();
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, !peg || (*peg != matching::Peg::None) != pegged ? Refusal::Peg : !lifetime ? Refusal::Lifetime : Refusal::Cross);
                    return true;
                }
                engine_.on_new_order(matching::Order{
                    m->order_id.get().value(),
                    side,
                    market ? matching::market(side) : m->price.get().value(),
                    m->quantity.get().value(),
                    *lifetime,
                    static_cast<char>(m->display.get().value()) != 'N',
                    *peg,
                    market ? matching::market(side) : m->price.get().value(),
                    static_cast<std::int64_t>(m->peg_offset.get().value()),
                    (m->expire_time.get().value() == 0 ? std::int64_t{0} : static_cast<std::int64_t>(m->timestamp.get().value()) + static_cast<std::int64_t>(m->expire_time.get().value()) * 1000000000),
                    static_cast<std::uint64_t>(m->min_qty.get().value()),
                    static_cast<char>(m->post_only.get().value()) == 'P',
                    static_cast<char>(m->trade_now.get().value()) == 'Y',
                    static_cast<char>(m->short_sale.get().value()) == 'T',
                    static_cast<std::uint64_t>(m->max_floor.get().value()),
                    static_cast<char>(m->display.get().value()) == 'A',
                    attribution_of(m->attribution.get().value()),
                    static_cast<std::uint64_t>(m->self_match_group.get().value()),
                    static_cast<long long>(m->self_match_level.get().value()) == 'M' ? std::uint8_t{0} : static_cast<std::uint8_t>(m->self_match_level.get().value()),
                    prevention_of(m->self_match_strategy.get().value()).value_or(matching::Prevention::None),
                    static_cast<char>(m->aiq_strategy.get().value()),
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
                const bool market = m->price.get().value() == 2147483647 || m->price.get().value() == 2000000000;   // its price says so
                const std::optional<matching::Lifetime> lifetime = lifetime_of(m->time_in_force.get().value());
                const std::optional<matching::Cross> only = cross_of(m->cross_type.get().value());
                const std::optional<matching::Peg> peg = peg_of(m->peg.get().value());
                const bool pegged = peg && *peg != matching::Peg::None;
                if (!peg || (*peg != matching::Peg::None) != pegged || !lifetime || (!only || *only == matching::Cross::Opening || *only == matching::Cross::Intraday || *only == matching::Cross::Closing)) {
                    matching::Order refused{};
                    refused.id    = m->replacement_id.get().value();
                    refused.side  = side;
                    refused.price = m->price.get().value();
                    refused.qty   = m->quantity.get().value();
                    on_refused_(refused, !peg || (*peg != matching::Peg::None) != pegged ? Refusal::Peg : !lifetime ? Refusal::Lifetime : Refusal::Cross);
                    return true;
                }
                const matching::Order order = matching::Order{
                    m->replacement_id.get().value(),
                    side,
                    market ? matching::market(side) : m->price.get().value(),
                    m->quantity.get().value(),
                    *lifetime,
                    static_cast<char>(m->display.get().value()) != 'N',
                    *peg,
                    market ? matching::market(side) : m->price.get().value(),
                    static_cast<std::int64_t>(m->peg_offset.get().value()),
                    0,
                    static_cast<std::uint64_t>(m->min_qty.get().value()),
                    static_cast<char>(m->post_only.get().value()) == 'P',
                    static_cast<char>(m->trade_now.get().value()) == 'Y',
                    static_cast<char>(m->short_sale.get().value()) == 'T',
                    static_cast<std::uint64_t>(m->max_floor.get().value()),
                    static_cast<char>(m->display.get().value()) == 'A',
                    attribution_of(m->attribution.get().value()),
                    static_cast<std::uint64_t>(m->self_match_group.get().value()),
                    static_cast<long long>(m->self_match_level.get().value()) == 'M' ? std::uint8_t{0} : static_cast<std::uint8_t>(m->self_match_level.get().value()),
                    prevention_of(m->self_match_strategy.get().value()).value_or(matching::Prevention::None),
                    static_cast<char>(m->aiq_strategy.get().value()),
                };
                // To the day, which replaces; an engine without one - a test's bare book - replaces nothing.
                if constexpr (requires { engine_.on_replace(order, m->order_id.get().value()); })
                    engine_.on_replace(order, m->order_id.get().value());
                return true;
            }
            case IpcType::ReduceOrder: {
                if (bytes.size() < 3 + sizeof(ipc_ns::reduce_order_message)) return false;
                const auto* m = ipc_ns::reduce_order_message::parse(const_cast<std::byte*>(data));
                if constexpr (requires { engine_.on_reduce(m->order_id.get().value(), m->quantity.get().value()); })
                    engine_.on_reduce(m->order_id.get().value(), m->quantity.get().value());
                return true;
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
    // Why the change of phase being made is made, as its request said; nothing while none is.
    std::string_view phase_reason() const { return phase_reason_; }

private:
    Engine& engine_;
    std::string phase_reason_;   // why the change of phase being made is made
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
template <typename OnTrade, typename OnRest, typename OnCanceled = IgnoreEvent, typename OnAccepted = IgnoreEvent, typename OnEliminated = IgnoreEvent, typename OnRefused = IgnoreEvent, typename OnReplaced = IgnoreEvent, typename OnReduced = IgnoreEvent, typename OnCancelPending = IgnoreEvent, typename OnReduceRefused = IgnoreEvent>
class IpcEventServer {
public:
    IpcEventServer(OnTrade on_trade, OnRest on_rest)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_() {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled) {}

    IpcEventServer(OnTrade on_trade, OnRest on_rest, OnCanceled on_canceled, OnAccepted on_accepted, OnEliminated on_eliminated, OnRefused on_refused, OnReplaced on_replaced, OnReduced on_reduced, OnCancelPending on_cancel_pending, OnReduceRefused on_reduce_refused)
        : on_trade_(on_trade), on_rest_(on_rest), on_canceled_(on_canceled), on_accepted_(on_accepted), on_eliminated_(on_eliminated), on_refused_(on_refused), on_replaced_(on_replaced), on_reduced_(on_reduced), on_cancel_pending_(on_cancel_pending), on_reduce_refused_(on_reduce_refused) {}
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
    OnReplaced  on_replaced_;
    OnReduced   on_reduced_;
    OnCancelPending on_cancel_pending_;
    OnReduceRefused on_reduce_refused_;
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
    m.resting_midpoint.set(static_cast<std::remove_cvref_t<decltype(m.resting_midpoint.get().value())>>(trade.resting_midpoint ? 'Y' : 'N'));
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
    m.quantity.set(static_cast<std::uint32_t>(matching::showing(order)));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? (order.attributed ? 'A' : 'Y') : 'N'));
    m.attribution.set(std::string_view{ order.attribution.data(), order.attribution.size() });
    m.refreshed_from.set(order.renamed);   // the id it stood under, where what it shows was refreshed
    detail_ipc::emit_framed(emit, IpcType::OrderRested, m);
}

template <typename Emit>
inline void emit_canceled(Emit& emit, const matching::Order& order) {
    ipc_ns::order_canceled_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument_symbol);
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.prevented.set(static_cast<std::remove_cvref_t<decltype(m.prevented.get().value())>>(order.prevented.qty > 0 ? 'Y' : 'N'));
    if (order.prevented.qty > 0) m.prevented_liquidity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_liquidity.get().value())>>(order.prevented.resting ? 'A' : 'R'));
    m.prevented_quantity.set(static_cast<std::remove_cvref_t<decltype(m.prevented_quantity.get().value())>>(order.prevented.qty));
    m.prevented_price.set(static_cast<std::remove_cvref_t<decltype(m.prevented_price.get().value())>>(order.prevented.price));
    m.prevented_strategy.set(static_cast<std::remove_cvref_t<decltype(m.prevented_strategy.get().value())>>(order.prevented.strategy));
    detail_ipc::emit_framed(emit, IpcType::OrderCanceled, m);
}

template <typename Emit>
inline void emit_accepted(Emit& emit, const matching::Order& order) {
    ipc_ns::order_accepted_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument_symbol);
    m.price.set(order.peg == matching::Peg::None ? (order.price == matching::market(order.side) ? 2147483647 : order.price) : order.limit == matching::market(order.side) ? 2147483647 : order.limit);
    detail_ipc::emit_framed(emit, IpcType::OrderAccepted, m);
}


template <typename Emit>
inline void emit_eliminated(Emit& emit, const matching::Order& order) {
    ipc_ns::order_eliminated_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument_symbol);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.peg == matching::Peg::None ? (order.price == matching::market(order.side) ? 2147483647 : order.price) : order.limit == matching::market(order.side) ? 2147483647 : order.limit);
    m.quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    m.time_in_force.set(static_cast<std::remove_cvref_t<decltype(m.time_in_force.get().value())>>(order.lifetime == matching::Lifetime::Session ? '0' : order.lifetime == matching::Lifetime::Immediate ? '3' : order.lifetime == matching::Lifetime::Extended ? '5' : '6'));
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
    m.price.set(order.peg == matching::Peg::None ? (order.price == matching::market(order.side) ? 2147483647 : order.price) : order.limit == matching::market(order.side) ? 2147483647 : order.limit);
    m.quantity.set(static_cast<std::uint32_t>(matching::showing(order)));
    m.open_quantity.set(static_cast<std::uint32_t>(order.qty));
    m.display.set(static_cast<std::remove_cvref_t<decltype(m.display.get().value())>>(order.shown ? 'Y' : 'N'));
    detail_ipc::emit_framed(emit, IpcType::OrderReplaced, m);
}


template <typename Emit>
inline void emit_reduced(Emit& emit, const matching::Order& order, std::uint64_t taken) {
    ipc_ns::order_reduced_message m{};
    m.timestamp.set(send_time_nanos());
    m.symbol.set(instrument_symbol);
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
inline void emit_book_order(Emit& emit, const matching::Order& order, char where, std::uint64_t sequence) {
    ipc_ns::book_order_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order.id);
    m.symbol.set(instrument_symbol);
    m.side.set(static_cast<std::remove_cvref_t<decltype(m.side.get().value())>>(order.side == matching::Side::Buy ? 'B' : 'S'));
    m.price.set(order.peg == matching::Peg::None ? (order.price == matching::market(order.side) ? 2147483647 : order.price) : order.limit == matching::market(order.side) ? 2147483647 : order.limit);
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
    m.symbol.set(instrument_symbol);
    m.refusal_reason.set(static_cast<std::remove_cvref_t<decltype(m.refusal_reason.get().value())>>(reason == Refusal::Phase ? 'E' : reason == Refusal::Peg ? 'P' : reason == Refusal::Type ? 'T' : reason == Refusal::Lifetime ? 'O' : 'C'));
    detail_ipc::emit_framed(emit, IpcType::OrderRefused, m);
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
inline void emit_cancel_pending(Emit& emit, std::uint64_t order) {
    ipc_ns::cancel_pending_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order);
    m.symbol.set(instrument_symbol);
    detail_ipc::emit_framed(emit, IpcType::CancelPending, m);
}

// A reduce the engine refuses: its order waits for a cross, and its cancels wait with it.
template <typename Emit>
inline void emit_reduce_refused(Emit& emit, std::uint64_t order) {
    ipc_ns::reduce_refused_message m{};
    m.timestamp.set(send_time_nanos());
    m.order_id.set(order);
    m.symbol.set(instrument_symbol);
    detail_ipc::emit_framed(emit, IpcType::ReduceRefused, m);
}}
