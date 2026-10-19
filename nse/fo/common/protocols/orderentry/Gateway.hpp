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
#include "messages/OrderEntryRequestMessage.hpp"
#include "messages/OrderCancellationRequestMessage.hpp"
#include "messages/OrderConfirmationMessage.hpp"
#include "messages/OrderCancellationConfirmationOrderEntryBody.hpp"
#include "messages/TradeConfirmationMessage.hpp"
#include "structs/MessageHeader.hpp"
#include "structs/MessageHeader.hpp"
#include "NnfSession.hpp"
#include "../ipc/IpcBus.hpp"
#include "../ipc/structs/NewOrderMessage.hpp"
#include "../ipc/structs/CancelOrderMessage.hpp"
#include "../ipc/structs/OrderCanceledMessage.hpp"
#include "../ipc/structs/TradeMessage.hpp"

namespace sim::orderentry {

namespace protocol = ::nse::nsefo::orderentry::nnf::v9_50;
namespace ipc_ns = ::nse::nsefo::matchingengine::ipc::v1_0;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

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

// order_entry_request_message -> new_order_message  (NseFo.Ipc.Transforms.xml: nnf_order_entry)
inline ipc_ns::new_order_message to_nnf_order_entry(const protocol::order_entry_request_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::new_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](auto value) { switch (static_cast<long long>(value)) { case 1: return 'B'; case 2: return 'S'; default: return 'B'; } }(m.fields.order_entry_body.buy_sell_indicator.get().value())));
    out.quantity.set(static_cast<value_of<decltype(out.quantity)>>(m.fields.order_entry_body.volume.get().value()));
    out.price.set(static_cast<value_of<decltype(out.price)>>(m.fields.order_entry_body.price.get().value()));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// order_cancellation_request_message -> cancel_order_message  (NseFo.Ipc.Transforms.xml: nnf_order_cancel)
inline ipc_ns::cancel_order_message to_nnf_order_cancel(const protocol::order_cancellation_request_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// order_entry_request_message -> order_confirmation_message  (NseFo.Ipc.Transforms.xml: nnf_order_confirmation)
inline protocol::order_confirmation_message to_nnf_order_confirmation(const protocol::order_entry_request_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_confirmation_message out{};
    out.fields.order_entry_body.order_number.set(static_cast<value_of<decltype(out.fields.order_entry_body.order_number)>>(order.id));
    out.fields.order_entry_body.token_no.set(m.fields.order_entry_body.token_no.get().value());
    out.fields.order_entry_body.buy_sell_indicator.set(m.fields.order_entry_body.buy_sell_indicator.get().value());
    out.fields.order_entry_body.volume.set(m.fields.order_entry_body.volume.get().value());
    out.fields.order_entry_body.price.set(m.fields.order_entry_body.price.get().value());
    return out;
}

// order_canceled_message -> order_cancellation_confirmation_order_entry_body  (NseFo.Ipc.Transforms.xml: nnf_cancel_confirmation)
inline protocol::order_cancellation_confirmation_order_entry_body to_nnf_cancel_confirmation(const ipc_ns::order_canceled_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_cancellation_confirmation_order_entry_body out{};
    out.fields.order_entry_body.order_number.set(static_cast<value_of<decltype(out.fields.order_entry_body.order_number)>>(order.reference));
    out.fields.order_entry_body.total_volume_remaining.set(static_cast<value_of<decltype(out.fields.order_entry_body.total_volume_remaining)>>(order.remaining));
    return out;
}

// trade_message -> trade_confirmation_message  (NseFo.Ipc.Transforms.xml: nnf_trade_resting)
inline protocol::trade_confirmation_message to_nnf_trade_resting(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::trade_confirmation_message out{};
    out.fields.response_order_number.set(static_cast<value_of<decltype(out.fields.response_order_number)>>(order.reference));
    out.fields.fill_quantity.set(static_cast<value_of<decltype(out.fields.fill_quantity)>>(m.quantity.get().value()));
    out.fields.fill_price.set(static_cast<value_of<decltype(out.fields.fill_price)>>(m.price.get().value()));
    return out;
}

// trade_message -> trade_confirmation_message  (NseFo.Ipc.Transforms.xml: nnf_trade_aggressor)
inline protocol::trade_confirmation_message to_nnf_trade_aggressor(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::trade_confirmation_message out{};
    out.fields.response_order_number.set(static_cast<value_of<decltype(out.fields.response_order_number)>>(order.reference));
    out.fields.fill_quantity.set(static_cast<value_of<decltype(out.fields.fill_quantity)>>(m.quantity.get().value()));
    out.fields.fill_price.set(static_cast<value_of<decltype(out.fields.fill_price)>>(m.price.get().value()));
    return out;
}

// The order-entry gateway: Nse Nnf clients in, the Ipc to the engine out, and the engine's
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

    // The engine's order_canceled_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_canceled_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_nnf_cancel_confirmation(m, found->second));
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's trade_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::trade_message& m) {
        if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_nnf_trade_resting(m, found->second));
        if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_nnf_trade_aggressor(m, found->second));
        fill(m.resting_order_id.get().value(), m.quantity.get().value());
        fill(m.aggressor_order_id.get().value(), m.quantity.get().value());
    }

private:
    struct Client {
        Gateway& gateway;
        std::size_t index;
        Send send;
        nnf::Session<Send, Client> session;

        Client(Gateway& owner, std::size_t at, Send writer) : gateway(owner), index(at), send(std::move(writer)), session(send, *this) {}

        std::optional<nnf::status> login(auto username, auto password) {
            return gateway.login(username, password);
        }
        void message(std::string_view packet) { gateway.message(index, packet); }
        void logout() {}
    };

    // A login is refused with the reason the exchange states, or the first the model gives.
    std::optional<nnf::status> login(auto username, auto password) const {
        if (credentials_.empty()) return std::nullopt;
        // Credentials are configured as text, whatever the protocol presents them as.
        const auto named = [](auto value) {
            if constexpr (std::is_arithmetic_v<decltype(value)>) return std::to_string(value);
            else return std::string(value);
        };
        const auto found = credentials_.find(named(username));
        if (found != credentials_.end() && found->second == named(password)) return std::nullopt;
        return static_cast<nnf::status>(16006);
    }

    // One unsequenced data packet from a client, dispatched by its message's code.
    void message(std::size_t client, std::string_view packet) {
        if (packet.size() <= sizeof(protocol::message_header)) return;
        const auto* bytes = reinterpret_cast<const std::byte*>(packet.data());
        auto selector = protocol::message_header::parse(bytes)->transaction_code.get().value();
        switch (selector) {
            case static_cast<decltype(selector)>(2000): {
                if (packet.size() < fixed_size<protocol::order_entry_request_message>()) return;
                const auto& m = *protocol::order_entry_request_message::parse(bytes);
                Order order{ ++next_id_, static_cast<std::uint64_t>(m.fields.order_entry_body.order_number.get().value()), 0, client };
                auto ipc = to_nnf_order_entry(m, order);
                order.remaining = ipc.quantity.get().value();
                orders_[order.id] = order;
                references_[{ client, order.reference }] = order.id;
                respond(client, to_nnf_order_confirmation(m, order));
                detail_ipc::emit_framed(emit_, 'N', ipc);
                return;
            }
            case static_cast<decltype(selector)>(2070): {
                if (packet.size() < fixed_size<protocol::order_cancellation_request_message>()) return;
                const auto& m = *protocol::order_cancellation_request_message::parse(bytes);
                const auto key = std::make_pair(client, static_cast<std::uint64_t>(m.fields.order_entry_body.order_number.get().value()));
                const auto found = references_.find(key);
                if (found == references_.end()) return;
                const Order& order = orders_.at(found->second);
                auto ipc = to_nnf_order_cancel(m, order);
                detail_ipc::emit_framed(emit_, 'C', ipc);
                return;
            }
            default:
                return;   // a message the order entry does not map
        }
    }

    // A response to a client, whole: its session publishes it as it stands.
    template <typename Response>
    void respond(std::size_t client, const Response& response) {
        const auto* bytes = reinterpret_cast<const char*>(&response);
        // A client that has gone is not written to: the order it left may still trade, and what
        // the venue would have told it goes nowhere.
        const auto held = clients_.find(client);
        if (held == clients_.end()) { return; }

        held->second->session.message(bytes, response.header.message_length.get().value());
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
