#pragma once

#include <cstdint>
#include <string_view>
#include "FixWire.hpp"
#include "FixSession.hpp"

namespace iex::order_entry {

// A named FIX trading bot (client). Wraps a Session; `Strategy` decides what to send;
// ExecutionReports (35=8) are surfaced via `OnExec` for verification. This is the
// end-to-end harness: submit -> match -> feed -> observe the exec report.
template <typename Session, typename Strategy, typename OnExec>
class Bot {
public:
    Bot(std::string_view name, Session& session, Strategy strategy, OnExec on_exec)
        : name_(name), session_(session), strategy_(strategy), on_exec_(on_exec) {}

    std::string_view name() const { return name_; }

    // Connect / initiate the FIX session.
    void start() { session_.logon(); }

    // Drive one strategy decision (submit / cancel). Called by the owner's event loop.
    // TODO(bot): the strategy encodes NewOrderSingle / OrderCancelRequest and hands the
    // body to session_.send_app(...); track cl_ord_ids for reconciliation.
    void step() { strategy_.step(session_); }

    // Route a decoded application message from the session.
    void on_app(char msg_type, std::string_view body) {
        if (msg_type == '8') {          // ExecutionReport
            // TODO(bot): decode fills / ord-status; the harness asserts on these.
            on_exec_(body);
        }
    }

private:
    std::string_view name_;
    Session&         session_;
    Strategy         strategy_;
    OnExec           on_exec_;
};}
