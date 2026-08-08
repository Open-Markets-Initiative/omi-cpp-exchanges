#pragma once

#include <cstddef>
#include <cstdint>
#include <charconv>
#include <string_view>
#include "FixWire.hpp"
#include "FixSession.hpp"
#include "MatchingEngine.hpp"

namespace iex::order_entry {

// FIX order-entry gateway. Owns the matching engine for an instrument and turns
// client order messages into engine calls; engine callbacks become execution reports.
// `Engine` is a matching::MatchingEngine<…>; `Send` writes framed FIX bytes back to
// the originating client (socket / shared-mem ring / in-proc queue — a wiring choice).
template <typename Engine, typename Send>
class Gateway {
public:
    Gateway(Engine& engine, Send send) : engine_(engine), send_(send) {}

    // Route one decoded application message (from a client Session's OnApp) into the engine.
    void on_app(char msg_type, std::string_view body) {
        switch (msg_type) {
            case 'D': on_new_order(body); break;   // NewOrderSingle
            case 'F': on_cancel(body);    break;   // OrderCancelRequest
            case 'G': on_replace(body);   break;   // OrderCancelReplaceRequest
            default:  break;                       // TODO: BusinessMessageReject
        }
    }

private:
    void on_new_order(std::string_view body) {
        NewOrderSingle msg{};
        if (!decode(body, msg)) return;
        // cl_ord_id carries a decimal id (see the Orders adapter) → the engine order id,
        // so cancels resolve without a shared side map. price is limit ticks.
        matching::Order order{
            parse_id(msg.cl_ord_id),
            msg.side == '1' ? matching::Side::Buy : matching::Side::Sell,
            static_cast<std::int64_t>(msg.price),
            static_cast<std::uint64_t>(msg.order_qty),
        };
        engine_.on_new_order(order);   // trades/rests -> ExecutionReports via callbacks
    }

    void on_cancel(std::string_view body) {
        OrderCancelRequest msg{};
        if (!decode(body, msg)) return;
        engine_.on_cancel(parse_id(msg.orig_cl_ord_id));
    }

    void on_replace(std::string_view body) {
        // v1: cancel-then-new is deferred; atomic modify lands with the replace slice.
        (void)body;
    }

    // Recover the numeric id encoded into a cl_ord_id buffer (digits then NUL padding).
    template <std::size_t N>
    static std::uint64_t parse_id(const char (&s)[N]) {
        std::uint64_t id = 0;
        std::from_chars(s, s + N, id);
        return id;
    }

    Engine& engine_;
    Send    send_;
    std::uint64_t next_id_ = 1;
};}
