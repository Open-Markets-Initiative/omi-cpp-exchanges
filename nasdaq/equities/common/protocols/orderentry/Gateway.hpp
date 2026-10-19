#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <chrono>
#include "messages/EnterOrderMessage.hpp"
#include "messages/CancelOrderMessage.hpp"
#include "messages/OrderAcceptedMessage.hpp"
#include "messages/OrderExecutedMessage.hpp"
#include "messages/CanceledMessage.hpp"
#include "structs/ServerPacketHeader.hpp"
#include "structs/ClientPacketHeader.hpp"
#include "messages/UnsequencedDataPacket.hpp"
#include "SoupBinSession.hpp"
#include "../ipc/IpcBus.hpp"
#include "../ipc/structs/NewOrderMessage.hpp"
#include "../ipc/structs/CancelOrderMessage.hpp"
#include "../ipc/structs/TradeMessage.hpp"
#include "../ipc/structs/OrderCanceledMessage.hpp"

namespace sim::orderentry {

namespace protocol = ::nasdaq::nsmequities::orders::ouch::v5_0;
namespace ipc_ns = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// Nanoseconds since the Unix epoch as nanoseconds since midnight in a time zone.
inline std::uint64_t nanoseconds_since_midnight(std::uint64_t unix_nanoseconds, const std::chrono::time_zone* zone) {
    const auto local = zone->to_local(std::chrono::sys_time<std::chrono::nanoseconds>{ std::chrono::nanoseconds{ unix_nanoseconds } });
    return static_cast<std::uint64_t>((local - std::chrono::floor<std::chrono::days>(local)).count());
}

// The UTC time zone a timestamp is counted in.
inline const std::chrono::time_zone* utc() {
    static const auto* zone = std::chrono::locate_zone("UTC");
    return zone;
}

// The bytes a message's fixed part takes: all of it, less the tail a variable-length one reserves.
template <typename Message>
constexpr std::size_t fixed_size() {
    if constexpr (requires { Message::tail_capacity; }) return sizeof(Message) - Message::tail_capacity;
    else return sizeof(Message);
}

// An order the gateway admitted: its id, the client's reference to it, the shares still open,
// and the client that owns it.
struct Order {
    std::uint64_t id = 0;
    std::uint64_t reference = 0;
    std::uint64_t remaining = 0;
    std::size_t client = 0;
};

// enter_order_message -> new_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_enter_order)
inline ipc_ns::new_order_message to_ouch_enter_order(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::new_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case 'T': return 'S'; case 'E': return 'S'; default: return value; } }(static_cast<char>(m.fields.side.get().value()))));
    out.quantity.set(m.fields.quantity.get().value());
    out.symbol.set(m.fields.symbol.get().value());
    out.price.set(static_cast<value_of<decltype(out.price)>>(m.fields.price.get().value()));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// cancel_order_message -> cancel_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_cancel_order)
inline ipc_ns::cancel_order_message to_ouch_cancel_order(const protocol::cancel_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// enter_order_message -> order_accepted_message  (NsmEquities.Ipc.Transforms.xml: ouch_order_accepted)
inline protocol::order_accepted_message to_ouch_order_accepted(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_accepted_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), utc())));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(order.id));
    out.fields.order_state.set(static_cast<value_of<decltype(out.fields.order_state)>>('L'));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(static_cast<char>(m.fields.side.get().value())));
    out.fields.quantity.set(m.fields.quantity.get().value());
    out.fields.symbol.set(m.fields.symbol.get().value());
    out.fields.price.set(m.fields.price.get().value());
    out.fields.time_in_force.set(static_cast<value_of<decltype(out.fields.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.fields.display.set(static_cast<value_of<decltype(out.fields.display)>>(static_cast<char>(m.fields.display.get().value())));
    out.fields.capacity.set(static_cast<value_of<decltype(out.fields.capacity)>>(static_cast<char>(m.fields.capacity.get().value())));
    out.fields.inter_market_sweep_eligibility.set(static_cast<value_of<decltype(out.fields.inter_market_sweep_eligibility)>>(static_cast<char>(m.fields.inter_market_sweep_eligibility.get().value())));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>(static_cast<char>(m.fields.cross_type.get().value())));
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_resting)
inline protocol::order_executed_message to_ouch_executed_resting(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>('A'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), utc()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_aggressor)
inline protocol::order_executed_message to_ouch_executed_aggressor(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>('R'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), utc()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// order_canceled_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_canceled)
inline protocol::canceled_message to_ouch_canceled(const ipc_ns::order_canceled_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(order.remaining));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('U'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), utc()));
    return out;
}

// The order-entry gateway: SoupBin Tcp clients in, the Ipc to the engine out, and the engine's
// events back to the client each order belongs to. `Emit` is a callable(const char*, std::size_t)
// taking framed Ipc messages to the engine.
template <typename Emit>
class Gateway {
public:
    using Send = std::function<void(const char*, std::size_t)>;

    explicit Gateway(Emit emit) : emit_(emit) {}

    // A login the gateway accepts; with none registered, any login is accepted.
    void allow(std::string username, std::string password) { credentials_[std::move(username)] = std::move(password); }

    // A new client connection, writing through `send`; returns the session's own number.
    // 
    // Numbered once and never again: an order carries the number of the session that entered
    // it, and so does the reference it was entered under. A number handed out twice would
    // make one client's reference name another client's order.
    std::size_t connect(Send send) {
        const auto client = next_client_++;
        clients_.emplace(client, std::make_unique<Client>(*this, client, std::move(send)));
        return client;
    }

    // A client that has gone: its session is over, and what it held is let go. Whatever it
    // left resting stays in the book - a venue does not cancel an order because a line
    // dropped - and answers about those orders find nobody, which is what has happened.
    void disconnect(std::size_t client) { clients_.erase(client); }

    void on_bytes(std::size_t client, const char* data, std::size_t length) {
        if (const auto held = clients_.find(client); held != clients_.end()) {
            held->second->session.on_bytes(data, length);
        }
    }

    std::size_t open_orders() const { return orders_.size(); }

    // The engine's trade_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::trade_message& m) {
        if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_executed_resting(m, found->second));
        if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_executed_aggressor(m, found->second));
        fill(m.resting_order_id.get().value(), m.quantity.get().value());
        fill(m.aggressor_order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_canceled_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_canceled_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_canceled(m, found->second));
        close(m.order_id.get().value());
    }

private:
    struct Client {
        Gateway& gateway;
        std::size_t index;
        Send send;
        soupbin::Session<Send, Client> session;

        Client(Gateway& owner, std::size_t at, Send writer) : gateway(owner), index(at), send(std::move(writer)), session("NSMOUCH001", send, *this) {}

        std::optional<soupbin::status> login(auto username, auto password, auto, auto) {
            return gateway.login(username, password);
        }
        void message(std::string_view packet) { gateway.message(index, packet); }
        void logout() {}
    };

    // A login is refused with the reason the exchange states, or the first the model gives.
    std::optional<soupbin::status> login(auto username, auto password) const {
        if (credentials_.empty()) return std::nullopt;
        // Credentials are configured as text, whatever the protocol presents them as.
        const auto named = [](auto value) {
            if constexpr (std::is_arithmetic_v<decltype(value)>) return std::to_string(value);
            else return std::string(value);
        };
        const auto found = credentials_.find(named(username));
        if (found != credentials_.end() && found->second == named(password)) return std::nullopt;
        return static_cast<soupbin::status>('A');
    }

    // One unsequenced data packet from a client, dispatched by its message's code.
    void message(std::size_t client, std::string_view packet) {
        if (packet.size() <= sizeof(protocol::client_packet_header)) return;
        const auto* bytes = reinterpret_cast<const std::byte*>(packet.data());
        auto selector = protocol::unsequenced_data_packet::parse(bytes)->fields.unsequenced_message_type.get().value();
        switch (selector) {
            case static_cast<decltype(selector)>('O'): {
                if (packet.size() < fixed_size<protocol::enter_order_message>()) return;
                const auto& m = *protocol::enter_order_message::parse(bytes);
                Order order{ ++next_id_, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client };
                auto ipc = to_ouch_enter_order(m, order);
                order.remaining = ipc.quantity.get().value();
                orders_[order.id] = order;
                references_[{ client, order.reference }] = order.id;
                respond(client, to_ouch_order_accepted(m, order));
                detail_ipc::emit_framed(emit_, 'N', ipc);
                return;
            }
            case static_cast<decltype(selector)>('X'): {
                if (packet.size() < fixed_size<protocol::cancel_order_message>()) return;
                const auto& m = *protocol::cancel_order_message::parse(bytes);
                if (!(m.fields.quantity.get().value() == 0)) return;
                const auto key = std::make_pair(client, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()));
                const auto found = references_.find(key);
                if (found == references_.end()) return;
                const Order& order = orders_.at(found->second);
                auto ipc = to_ouch_cancel_order(m, order);
                detail_ipc::emit_framed(emit_, 'C', ipc);
                return;
            }
            default:
                return;   // a message the order entry does not map
        }
    }

    // A response to a client, as the body of the message its session publishes.
    template <typename Response>
    void respond(std::size_t client, const Response& response) {
        const auto* bytes = reinterpret_cast<const char*>(&response) + sizeof(protocol::server_packet_header);
        // A client that has gone is not written to: the order it left may still trade, and what
        // the venue would have told it goes nowhere.
        const auto held = clients_.find(client);
        if (held == clients_.end()) { return; }

        held->second->session.sequenced(bytes, response.header.packet_length.get().value() + 2 - sizeof(protocol::server_packet_header));
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
        references_.erase({ found->second.client, found->second.reference });
        orders_.erase(found);
    }

    Emit emit_;
    std::map<std::size_t, std::unique_ptr<Client>> clients_;
    std::size_t next_client_ = 0;   // session numbers, never reused
    std::map<std::string, std::string, std::less<>> credentials_;
    std::map<std::uint64_t, Order> orders_;
    std::map<std::pair<std::size_t, std::uint64_t>, std::uint64_t> references_;
    std::uint64_t next_id_ = 0;
};

}
