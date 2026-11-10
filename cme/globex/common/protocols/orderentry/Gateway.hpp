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
#include "messages/NewOrderSingle.hpp"
#include "messages/OrderCancelRequest.hpp"
#include "messages/ExecutionReportNew.hpp"
#include "messages/ExecutionReportCancel.hpp"
#include "messages/ExecutionReportTradeOutright.hpp"
#include "structs/SimpleOpenFramingHeader.hpp"
#include "structs/SimpleOpenFramingHeader.hpp"
#include "ILink3Session.hpp"
#include "../ipc/IpcBus.hpp"
#include "../ipc/structs/NewOrderMessage.hpp"
#include "../ipc/structs/CancelOrderMessage.hpp"
#include "../ipc/structs/OrderCanceledMessage.hpp"
#include "../ipc/structs/TradeMessage.hpp"

namespace sim::orderentry {

namespace protocol = ::cme::ilink3::v9;
namespace ipc_ns = ::cme::matchingengine::v0;

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
    std::string reference;
    std::uint64_t remaining = 0;
    std::size_t client = 0;
    protocol::new_order_single::fields_type entered{};   // what the client entered it with, which a response restates
};

// new_order_single -> new_order_message  (CmeGlobex.Ipc.Transforms.xml: ilink3_new_order)
inline ipc_ns::new_order_message to_ilink3_new_order(const protocol::new_order_single& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::new_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.side.set(static_cast<value_of<decltype(out.side)>>(m.fields.side.get().value()));
    out.quantity.set(m.fields.order_qty.get().value());
    out.price.set(m.fields.price_optional.get().value());
    out.security_id.set(m.fields.security_id.get().value());
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// order_cancel_request -> cancel_order_message  (CmeGlobex.Ipc.Transforms.xml: ilink3_order_cancel)
inline ipc_ns::cancel_order_message to_ilink3_order_cancel(const protocol::order_cancel_request& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.security_id.set(m.fields.security_id.get().value());
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// new_order_single -> execution_report_new  (CmeGlobex.Ipc.Transforms.xml: ilink3_new)
inline protocol::execution_report_new to_ilink3_new(const protocol::new_order_single& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::execution_report_new out{};
    out.fields.order_id.set(static_cast<value_of<decltype(out.fields.order_id)>>(order.id));
    out.fields.exec_id.set(std::to_string(order.id));
    out.fields.clordid.set(std::string_view{order.reference});
    out.fields.transact_time.set(static_cast<value_of<decltype(out.fields.transact_time)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.sending_time_epoch.set(static_cast<value_of<decltype(out.fields.sending_time_epoch)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.poss_retrans_flag.set(static_cast<value_of<decltype(out.fields.poss_retrans_flag)>>(0));
    out.fields.price.set(m.fields.price_optional.get().value());
    out.fields.order_qty.set(m.fields.order_qty.get().value());
    out.fields.security_id.set(m.fields.security_id.get().value());
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(m.fields.side.get().value()));
    out.fields.sender_id.set(m.fields.sender_id.get().value());
    out.fields.location.set(m.fields.location.get().value());
    out.fields.party_details_list_req_id.set(m.fields.party_details_list_req_id.get().value());
    out.fields.order_request_id.set(m.fields.order_request_id.get().value());
    out.fields.manual_order_indicator.set(static_cast<value_of<decltype(out.fields.manual_order_indicator)>>(m.fields.manual_order_indicator.get().value()));
    return out;
}

// order_canceled_message -> execution_report_cancel  (CmeGlobex.Ipc.Transforms.xml: ilink3_cancel)
inline protocol::execution_report_cancel to_ilink3_cancel(const ipc_ns::order_canceled_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::execution_report_cancel out{};
    out.fields.order_id.set(static_cast<value_of<decltype(out.fields.order_id)>>(order.id));
    out.fields.exec_id.set(std::to_string(order.id));
    out.fields.clordid.set(std::string_view{order.reference});
    out.fields.transact_time.set(static_cast<value_of<decltype(out.fields.transact_time)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.sending_time_epoch.set(static_cast<value_of<decltype(out.fields.sending_time_epoch)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.poss_retrans_flag.set(static_cast<value_of<decltype(out.fields.poss_retrans_flag)>>(0));
    out.fields.order_qty.set(order.entered.order_qty.get().value());
    out.fields.sender_id.set(order.entered.sender_id.get().value());
    out.fields.location.set(order.entered.location.get().value());
    out.fields.party_details_list_req_id.set(order.entered.party_details_list_req_id.get().value());
    out.fields.order_request_id.set(order.entered.order_request_id.get().value());
    out.fields.manual_order_indicator.set(static_cast<value_of<decltype(out.fields.manual_order_indicator)>>(order.entered.manual_order_indicator.get().value()));
    out.fields.security_id.set(m.security_id.get().value());
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(m.side.get().value()));
    out.fields.price.set(m.price.get().value());
    return out;
}

// trade_message -> execution_report_trade_outright  (CmeGlobex.Ipc.Transforms.xml: ilink3_trade_resting)
inline protocol::execution_report_trade_outright to_ilink3_trade_resting(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::execution_report_trade_outright out{};
    out.fields.order_id.set(static_cast<value_of<decltype(out.fields.order_id)>>(order.id));
    out.fields.clordid.set(std::string_view{order.reference});
    out.fields.transact_time.set(static_cast<value_of<decltype(out.fields.transact_time)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.sending_time_epoch.set(static_cast<value_of<decltype(out.fields.sending_time_epoch)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.poss_retrans_flag.set(static_cast<value_of<decltype(out.fields.poss_retrans_flag)>>(0));
    out.fields.aggressor_indicator.set(static_cast<value_of<decltype(out.fields.aggressor_indicator)>>(0));
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(order.entered.side.get().value()));
    out.fields.price.set(order.entered.price_optional.get().value());
    out.fields.order_qty.set(order.entered.order_qty.get().value());
    out.fields.sender_id.set(order.entered.sender_id.get().value());
    out.fields.location.set(order.entered.location.get().value());
    out.fields.party_details_list_req_id.set(order.entered.party_details_list_req_id.get().value());
    out.fields.order_request_id.set(order.entered.order_request_id.get().value());
    out.fields.manual_order_indicator.set(static_cast<value_of<decltype(out.fields.manual_order_indicator)>>(order.entered.manual_order_indicator.get().value()));
    out.fields.exec_id.set(std::to_string(m.execution_id.get().value()));
    out.fields.sec_exec_id.set(m.execution_id.get().value());
    out.fields.md_trade_entry_id.set(static_cast<value_of<decltype(out.fields.md_trade_entry_id)>>(m.execution_id.get().value()));
    out.fields.side_trade_id.set(static_cast<value_of<decltype(out.fields.side_trade_id)>>(m.execution_id.get().value()));
    out.fields.security_id.set(m.security_id.get().value());
    out.fields.last_px.set(m.price.get().value());
    out.fields.last_qty.set(m.quantity.get().value());
    protocol::start_group_write(out).start_fills().fill_px(m.price.get().value()).fill_qty(m.quantity.get().value()).fill_yield_type({}).end_fills().start_outright_order_events().end_outright_order_events();
    return out;
}

// trade_message -> execution_report_trade_outright  (CmeGlobex.Ipc.Transforms.xml: ilink3_trade_aggressor)
inline protocol::execution_report_trade_outright to_ilink3_trade_aggressor(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::execution_report_trade_outright out{};
    out.fields.order_id.set(static_cast<value_of<decltype(out.fields.order_id)>>(order.id));
    out.fields.clordid.set(std::string_view{order.reference});
    out.fields.transact_time.set(static_cast<value_of<decltype(out.fields.transact_time)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.sending_time_epoch.set(static_cast<value_of<decltype(out.fields.sending_time_epoch)>>(static_cast<std::uint64_t>(send_time_nanos())));
    out.fields.poss_retrans_flag.set(static_cast<value_of<decltype(out.fields.poss_retrans_flag)>>(0));
    out.fields.aggressor_indicator.set(static_cast<value_of<decltype(out.fields.aggressor_indicator)>>(1));
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(order.entered.side.get().value()));
    out.fields.price.set(order.entered.price_optional.get().value());
    out.fields.order_qty.set(order.entered.order_qty.get().value());
    out.fields.sender_id.set(order.entered.sender_id.get().value());
    out.fields.location.set(order.entered.location.get().value());
    out.fields.party_details_list_req_id.set(order.entered.party_details_list_req_id.get().value());
    out.fields.order_request_id.set(order.entered.order_request_id.get().value());
    out.fields.manual_order_indicator.set(static_cast<value_of<decltype(out.fields.manual_order_indicator)>>(order.entered.manual_order_indicator.get().value()));
    out.fields.exec_id.set(std::to_string(m.execution_id.get().value()));
    out.fields.sec_exec_id.set(m.execution_id.get().value());
    out.fields.md_trade_entry_id.set(static_cast<value_of<decltype(out.fields.md_trade_entry_id)>>(m.execution_id.get().value()));
    out.fields.side_trade_id.set(static_cast<value_of<decltype(out.fields.side_trade_id)>>(m.execution_id.get().value()));
    out.fields.security_id.set(m.security_id.get().value());
    out.fields.last_px.set(m.price.get().value());
    out.fields.last_qty.set(m.quantity.get().value());
    protocol::start_group_write(out).start_fills().fill_px(m.price.get().value()).fill_qty(m.quantity.get().value()).fill_yield_type({}).end_fills().start_outright_order_events().end_outright_order_events();
    return out;
}

// The order-entry gateway: Cme iLink3 clients in, the Ipc to the engine out, and the engine's
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
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ilink3_cancel(m, found->second));
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's trade_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::trade_message& m) {
        if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ilink3_trade_resting(m, found->second));
        if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ilink3_trade_aggressor(m, found->second));
        fill(m.resting_order_id.get().value(), m.quantity.get().value());
        fill(m.aggressor_order_id.get().value(), m.quantity.get().value());
    }

private:
    struct Client {
        Gateway& gateway;
        std::size_t index;
        Send send;
        ilink3::Session<Send, Client> session;

        Client(Gateway& owner, std::size_t at, Send writer) : gateway(owner), index(at), send(std::move(writer)), session(send, *this) {}

        std::optional<ilink3::status> login(auto username, auto password) {
            return gateway.login(username, password);
        }
        void message(std::string_view packet) { gateway.message(index, packet); }
        void logout() {}
    };

    // A login is refused with the reason the exchange states, or the first the model gives.
    std::optional<ilink3::status> login(auto username, auto password) const {
        if (credentials_.empty()) return std::nullopt;
        // Credentials are configured as text, whatever the protocol presents them as.
        const auto named = [](auto value) {
            if constexpr (std::is_arithmetic_v<decltype(value)>) return std::to_string(value);
            else return std::string(value);
        };
        const auto found = credentials_.find(named(username));
        if (found != credentials_.end() && found->second == named(password)) return std::nullopt;
        return static_cast<ilink3::status>(10);
    }

    // One unsequenced data packet from a client, dispatched by its message's code.
    void message(std::size_t client, std::string_view packet) {
        if (packet.size() <= sizeof(protocol::simple_open_framing_header)) return;
        const auto* bytes = reinterpret_cast<const std::byte*>(packet.data());
        auto selector = protocol::message_header::parse(bytes + 4)->template_id.get().value();
        switch (selector) {
            case static_cast<decltype(selector)>(514): {
                if (packet.size() < fixed_size<protocol::new_order_single>()) return;
                const auto& m = *protocol::new_order_single::parse(bytes);
                Order order{ ++next_id_, std::string(m.fields.clordid.get_trimmed().value()), 0, client };
                auto ipc = to_ilink3_new_order(m, order);
                order.remaining = ipc.quantity.get().value();
                order.entered = m.fields;
                orders_[order.id] = order;
                references_[{ client, order.reference }] = order.id;
                respond(client, to_ilink3_new(m, order));
                detail_ipc::emit_framed(emit_, 'N', ipc);
                return;
            }
            case static_cast<decltype(selector)>(516): {
                if (packet.size() < fixed_size<protocol::order_cancel_request>()) return;
                const auto& m = *protocol::order_cancel_request::parse(bytes);
                const auto key = std::make_pair(client, std::string(m.fields.clordid.get_trimmed().value()));
                const auto found = references_.find(key);
                if (found == references_.end()) return;
                const Order& order = orders_.at(found->second);
                auto ipc = to_ilink3_order_cancel(m, order);
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
    std::map<std::pair<std::size_t, std::string>, std::uint64_t> references_;
    std::uint64_t next_id_ = 0;
};

}
