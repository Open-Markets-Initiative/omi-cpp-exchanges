#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "FixAcceptor.hpp"
#include "../ipc/IpcBus.hpp"
#include "../ipc/structs/NewOrderMessage.hpp"
#include "../ipc/structs/CancelOrderMessage.hpp"
#include "../ipc/structs/TradeMessage.hpp"
#include "../ipc/structs/OrderCanceledMessage.hpp"
#include "../ipc/structs/OrderEliminatedMessage.hpp"

namespace sim::orderentry {

namespace protocol = ::odxodxsecuritytoken::order_entry;
namespace ipc_ns = ::odx::odxsecuritytoken::matchingengine::ipc::v2026;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// The tags the gateway reads and writes, as Odx.OdxSecurityToken.OrderEntry.Fix.v1.1 numbers them.
struct Tag {
    static constexpr std::uint32_t AvgPx = 6;
    static constexpr std::uint32_t ClOrdID = 11;
    static constexpr std::uint32_t CumQty = 14;
    static constexpr std::uint32_t CxlRejReason = 102;
    static constexpr std::uint32_t CxlRejResponseTo = 434;
    static constexpr std::uint32_t ExecID = 17;
    static constexpr std::uint32_t ExecRestatementReason = 378;
    static constexpr std::uint32_t ExecTransType = 20;
    static constexpr std::uint32_t ExecType = 150;
    static constexpr std::uint32_t LastPx = 31;
    static constexpr std::uint32_t LastShares = 32;
    static constexpr std::uint32_t LeavesQty = 151;
    static constexpr std::uint32_t OrdRejReason = 103;
    static constexpr std::uint32_t OrdStatus = 39;
    static constexpr std::uint32_t OrdType = 40;
    static constexpr std::uint32_t OrderID = 37;
    static constexpr std::uint32_t OrderQty = 38;
    static constexpr std::uint32_t OrigClOrdID = 41;
    static constexpr std::uint32_t Price = 44;
    static constexpr std::uint32_t Rule80A = 47;
    static constexpr std::uint32_t SenderCompID = 49;
    static constexpr std::uint32_t Side = 54;
    static constexpr std::uint32_t Symbol = 55;
    static constexpr std::uint32_t TimeInForce = 59;
    static constexpr std::uint32_t TransactTime = 60;
    static constexpr std::uint32_t TrdMatchID = 880;
};

// An order the gateway admitted: its id, the client's reference to it, the shares still open,
// and the client that owns it.
// It also keeps what the client entered it with that its reports name: OrdType, Price, Side, Symbol.
struct Order {
    std::uint64_t id = 0;
    std::string reference;
    std::uint64_t remaining = 0;
    std::size_t client = 0;
    std::string entered_ord_type;
    std::string entered_price;
    std::string entered_side;
    std::string entered_symbol;
    std::uint64_t quantity = 0;   // what it was admitted with, which its reports count executions against
    std::string requested;   // the ClOrdID the latest request naming it was sent under: its own until another names it
    std::string request;   // the request that admitted it, until the engine has taken or refused it: what that answer is written from
};

// NewOrderSingle -> new_order_message  (OdxSecurityToken.Ipc.Transforms.xml: fix_new_order)
inline ipc_ns::new_order_message to_fix_new_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::new_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>('0'));
    out.orderbook.set(protocol::field(m, Tag::Symbol));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case '1': return 'B'; case '2': return 'S'; default: return value; } }(protocol::field_char(m, Tag::Side))));
    out.order_type.set(static_cast<value_of<decltype(out.order_type)>>(protocol::field_char(m, Tag::OrdType)));
    out.price.set(static_cast<value_of<decltype(out.price)>>(protocol::field_scaled(m, Tag::Price, 2)));
    out.quantity.set(static_cast<value_of<decltype(out.quantity)>>(protocol::field_scaled(m, Tag::OrderQty, 0)));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// OrderCancelRequest -> cancel_order_message  (OdxSecurityToken.Ipc.Transforms.xml: fix_cancel_order)
inline ipc_ns::cancel_order_message to_fix_cancel_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// NewOrderSingle -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_order_new)
inline void to_fix_order_new(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::ExecID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::OrdStatus, std::string_view{"0"});
    out.put(Tag::OrdType, protocol::field(m, Tag::OrdType));
    if (!protocol::field(m, Tag::Price).empty()) { out.put(Tag::Price, protocol::field(m, Tag::Price)); }
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExecType, std::string_view{"0"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
}

// NewOrderSingle -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_order_rejected)
inline void to_fix_order_rejected(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::ExecID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, protocol::field(m, Tag::OrdType));
    if (!protocol::field(m, Tag::Price).empty()) { out.put(Tag::Price, protocol::field(m, Tag::Price)); }
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"99"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
}

// trade_message -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_filled_resting)
inline void to_fix_filled_resting(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(m.execution_id.get().value()));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"2"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    out.put(Tag::Price, std::string_view{order.entered_price});
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::ExecType, std::string_view{"2"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::TrdMatchID, static_cast<std::int64_t>(m.execution_id.get().value()));
}

// trade_message -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_partially_filled_resting)
inline void to_fix_partially_filled_resting(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(m.execution_id.get().value()));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"1"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    out.put(Tag::Price, std::string_view{order.entered_price});
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::ExecType, std::string_view{"1"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::TrdMatchID, static_cast<std::int64_t>(m.execution_id.get().value()));
}

// trade_message -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_filled_aggressor)
inline void to_fix_filled_aggressor(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(m.execution_id.get().value()));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"2"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    out.put(Tag::Price, std::string_view{order.entered_price});
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::ExecType, std::string_view{"2"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::TrdMatchID, static_cast<std::int64_t>(m.execution_id.get().value()));
}

// trade_message -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_partially_filled_aggressor)
inline void to_fix_partially_filled_aggressor(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(m.execution_id.get().value()));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 2);
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"1"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    out.put(Tag::Price, std::string_view{order.entered_price});
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::ExecType, std::string_view{"1"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::TrdMatchID, static_cast<std::int64_t>(m.execution_id.get().value()));
}

// order_canceled_message -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_canceled)
inline void to_fix_canceled(protocol::Writer& out, const ipc_ns::order_canceled_message& m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, std::string_view{order.requested});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
    out.put(Tag::Price, std::string_view{order.entered_price});
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
}

// order_eliminated_message -> ExecutionReport  (OdxSecurityToken.Ipc.Transforms.xml: fix_expired)
inline void to_fix_expired(protocol::Writer& out, const ipc_ns::order_eliminated_message& m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    out.put(Tag::Price, std::string_view{order.entered_price});
    out.put(Tag::Rule80A, std::string_view{"P"});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, std::string_view{"0"});
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::ExecRestatementReason, std::string_view{"103"});
}

// OrderCancelRequest -> OrderCancelReject  (OdxSecurityToken.Ipc.Transforms.xml: fix_cancel_refused)
inline void to_fix_cancel_refused(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::CxlRejReason, std::string_view{"0"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"1"});
}

// The order-entry gateway: Fix clients in, the Ipc to the engine out, and the engine's
// events back to the client each order belongs to. `Emit` is a callable(const char*, std::size_t)
// taking framed Ipc messages to the engine; `Clock` tells the time.
template <typename Emit, typename Clock = sim::clock::Nanos>   // the venue's clock: SendingTime, and what its timers wait from
class Gateway {
public:
    using Send = std::function<void(const char*, std::size_t)>;

    // One of the venue's gateways, a gateway for each trader. Each numbers the orders it admits
    // in an interleave of its own - the k-th of n gateways k, k + n, k + 2n ... - so no two of them
    // give one number, and none has to ask another what it gave: the simulator's own scheme. A
    // gateway on its own numbers from one, as the one gateway does.
    explicit Gateway(Emit emit, std::uint64_t first = 1, std::uint64_t step = 1, Clock clock = {})
        : emit_(emit), clock_(clock), next_id_(first), step_(step) {
        config_.comp_id = "ODXPTS";
    }

    // A login the gateway accepts, by the SenderCompID a client logs on with; the model's Logon carries no password, so none is checked.
    // With none registered, any login is accepted.
    void allow(std::string username, std::string password) { credentials_[std::move(username)] = std::move(password); }

    // A new client connection, writing through `send`; returns the session's own number, which is
    // never handed out again - an order carries the number of the session that entered it.
    std::size_t connect(Send send) {
        const auto client = next_client_++;
        clients_.emplace(client, std::make_unique<Client>(*this, client, std::move(send)));
        return client;
    }

    // A client that has gone: its session is over. What it left resting stays in the book.
    void disconnect(std::size_t client) { clients_.erase(client); }

    // Bytes from a client, in whatever pieces they came.
    void on_bytes(std::size_t client, const char* data, std::size_t length) {
        if (const auto held = clients_.find(client); held != clients_.end()) {
            held->second->foreign = !held->second->session.on_bytes(data, length, clock_());
        }
    }

    // Is a client's session over - logged out, refused, gone silent, or not FIX at all? Its
    // connection is then the loop's to close.
    bool ended(std::size_t client) const {
        const auto held = clients_.find(client);
        return held == clients_.end() || held->second->foreign || held->second->session.ended();
    }

    // The time: every session fires what is due by it - heartbeats, test requests, the end of a
    // session gone silent.
    void tick() {
        const auto now = clock_();
        for (auto& held : clients_) held.second->session.tick(now);
    }

    std::size_t open_orders() const { return orders_.size(); }

    // The engine's trade_message: answered to the client owning each order it names, as the event leaves it.
    void on_event(const ipc_ns::trade_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            answered.remaining -= std::min<std::uint64_t>(m.quantity.get().value(), answered.remaining);
            if (answered.remaining == 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_filled_resting(out, m, answered, now); }, now);
            if (answered.remaining != 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_partially_filled_resting(out, m, answered, now); }, now);
        }
        if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            answered.remaining -= std::min<std::uint64_t>(m.quantity.get().value(), answered.remaining);
            if (answered.remaining == 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_filled_aggressor(out, m, answered, now); }, now);
            if (answered.remaining != 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_partially_filled_aggressor(out, m, answered, now); }, now);
        }
        fill(m.resting_order_id.get().value(), m.quantity.get().value());
        fill(m.aggressor_order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_canceled_message: answered to the client owning each order it names, as the event leaves it.
    void on_event(const ipc_ns::order_canceled_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_canceled(out, m, answered, now); }, now);
        }
        close(m.order_id.get().value());
    }

    // The engine's order_eliminated_message: answered to the client owning each order it names, as the event leaves it.
    void on_event(const ipc_ns::order_eliminated_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_expired(out, m, answered, now); }, now);
        }
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_accepted_message: the request that admitted the order answered, now the engine has it.
    void on_event(const ipc_ns::order_accepted_message& m) {
        const auto now = clock_();
        const auto found = orders_.find(m.order_id.get().value());
        if (found == orders_.end()) return;
        Order& answered = found->second;
        respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_order_new(out, answered.request, answered, now); }, now);
        answered.request.clear();
    }

    // The engine's order_refused_message: the request that admitted the order answered, now the engine has it.
    void on_event(const ipc_ns::order_refused_message& m) {
        const auto now = clock_();
        const auto found = orders_.find(m.order_id.get().value());
        if (found == orders_.end()) return;
        Order& answered = found->second;
        respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_order_rejected(out, answered.request, answered, now); }, now);
        close(m.order_id.get().value());   // refused: it never stood in the book
    }

private:
    struct Client {
        Gateway& gateway;
        std::size_t index;
        Send send;
        protocol::Acceptor<Send, Client&> session;
        bool foreign = false;

        Client(Gateway& owner, std::size_t at, Send writer)
            : gateway(owner), index(at), send(std::move(writer)), session(owner.config_, send, *this) {}

        bool login(std::string_view, std::string_view text, std::string& refusal) { return gateway.login(text, refusal); }
        void message(std::string_view type, std::string_view text) { gateway.message(index, type, text); }
        void logout() {}
    };

    bool login(std::string_view text, std::string& refusal) const {
        if (credentials_.empty()) return true;
        const auto found = credentials_.find(protocol::field(text, Tag::SenderCompID));
        const bool accepted = found != credentials_.end();
        if (!accepted) refusal = "credentials not recognised";
        return accepted;
    }

    // One application message from a client, recognised by its MsgType.
    void message(std::size_t client, std::string_view type, std::string_view m) {
        const auto now = clock_();

        if (type == "D") {   // NewOrderSingle: fix_new_order
            Order order{ numbered(), std::string(protocol::field(m, Tag::ClOrdID)), 0, client, {}, {}, {}, {}, 0, {}, {} };
            auto ipc = to_fix_new_order(m, order);
            order.remaining = ipc.quantity.get().value();
            order.quantity = order.remaining;
            order.requested = order.reference;
            order.entered_ord_type = std::string(protocol::field(m, Tag::OrdType));
            order.entered_price = std::string(protocol::field(m, Tag::Price));
            order.entered_side = std::string(protocol::field(m, Tag::Side));
            order.entered_symbol = std::string(protocol::field(m, Tag::Symbol));
            order.request = std::string(m);   // answered once the engine has it
            orders_[order.id] = order;
            references_[{ client, order.reference }] = order.id;
            detail_ipc::emit_framed(emit_, 'N', ipc);
            return;
        }

        if (type == "F") {   // OrderCancelRequest: fix_cancel_order
            const auto named = std::make_pair(client, std::string(protocol::field(m, Tag::OrigClOrdID)));
            const auto found = references_.find(named);
            if (found == references_.end()) {
                if (const auto gone = closed_.find(named); gone != closed_.end()) {
                    Order order{};
                    order.id = gone->second;
                    order.reference = named.second;
                    order.client = client;
                    respond(client, "9", [&](protocol::Writer& out) { to_fix_cancel_refused(out, m, order, now); }, now);
                }
                return;
            }
            Order& order = orders_.at(found->second);
            order.requested = std::string(protocol::field(m, Tag::ClOrdID));
            auto ipc = to_fix_cancel_order(m, order);
            detail_ipc::emit_framed(emit_, 'C', ipc);
            return;
        }

        // a message the order entry does not map
    }

    // A response to a client, written into a body its session numbers, frames and keeps.
    // A client that has gone is not written to.
    template <typename Write>
    void respond(std::size_t client, std::string_view type, Write write, const std::int64_t now) {
        const auto held = clients_.find(client);
        if (held == clients_.end()) { return; }

        std::array<char, 1024> body{};
        protocol::Writer out{body.data(), body.data() + body.size()};
        write(out);

        if (!out.overflowed) {
            held->second->session.message(type, std::string_view{body.data(), out.used(body.data())}, now);
        }
    }

    // Shares executed on an order; a filled order is no longer open.
    void fill(std::uint64_t id, std::uint64_t shares) {
        auto found = orders_.find(id);
        if (found == orders_.end()) return;
        found->second.remaining -= std::min(shares, found->second.remaining);
        if (found->second.remaining == 0) close(id);
    }

    void close(std::uint64_t id) {
        auto found = orders_.find(id);
        if (found == orders_.end()) return;
        closed_[{ found->second.client, found->second.reference }] = id;
        references_.erase({ found->second.client, found->second.reference });
        orders_.erase(found);
    }

    Emit emit_;
    Clock clock_;
    protocol::AcceptorConfig config_{};
    std::map<std::size_t, std::unique_ptr<Client>> clients_;
    std::size_t next_client_ = 0;   // session numbers, never reused
    std::map<std::string, std::string, std::less<>> credentials_;
    std::map<std::uint64_t, Order> orders_;
    std::map<std::pair<std::size_t, std::string>, std::uint64_t, std::less<>> references_;
    std::map<std::pair<std::size_t, std::string>, std::uint64_t, std::less<>> closed_;   // the day's orders that have left the book, which a late request is refused naming
    std::uint64_t next_id_;   // the number the next order admitted is given
    std::uint64_t step_;      // how many gateways number in turn

    // The number of the next order admitted, and the gateway's next after it.
    std::uint64_t numbered() {
        const std::uint64_t id = next_id_;
        next_id_ += step_;
        return id;
    }
};

}  // namespace sim::orderentry
