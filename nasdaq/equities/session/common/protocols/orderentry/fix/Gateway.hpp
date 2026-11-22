#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "FixAcceptor.hpp"
#include "../../ipc/IpcBus.hpp"
#include "../../ipc/structs/NewOrderMessage.hpp"
#include "../../ipc/structs/CancelOrderMessage.hpp"
#include "../../ipc/structs/ReplaceOrderMessage.hpp"
#include "../../ipc/structs/TradeMessage.hpp"
#include "../../ipc/structs/OrderCanceledMessage.hpp"
#include "../../ipc/structs/OrderEliminatedMessage.hpp"
#include "../../ipc/structs/CancelPendingMessage.hpp"
#include "../../ipc/structs/OrderReplacedMessage.hpp"
#include "../../ipc/structs/PhaseAnnouncedMessage.hpp"

namespace sim::orderentry::fix {

namespace protocol = ::nasdaqnsmequities::order_entry;
namespace ipc_ns = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// The tags the gateway reads and writes, as Nasdaq.NsmEquities.OrderEntry.Fix.v4.2 numbers them.
struct Tag {
    static constexpr std::uint32_t AvgPx = 6;
    static constexpr std::uint32_t ClOrdID = 11;
    static constexpr std::uint32_t ClientID = 109;
    static constexpr std::uint32_t CumQty = 14;
    static constexpr std::uint32_t CxlRejReason = 102;
    static constexpr std::uint32_t CxlRejResponseTo = 434;
    static constexpr std::uint32_t DisplayInst = 9140;
    static constexpr std::uint32_t ExecBroker = 76;
    static constexpr std::uint32_t ExecID = 17;
    static constexpr std::uint32_t ExecTransType = 20;
    static constexpr std::uint32_t ExecType = 150;
    static constexpr std::uint32_t ExtendedReasonCode = 22204;
    static constexpr std::uint32_t LastPx = 31;
    static constexpr std::uint32_t LastShares = 32;
    static constexpr std::uint32_t LeavesQty = 151;
    static constexpr std::uint32_t LiquidityFlag = 9882;
    static constexpr std::uint32_t LocateReqd = 114;
    static constexpr std::uint32_t MinQty = 110;
    static constexpr std::uint32_t OrdRejReason = 103;
    static constexpr std::uint32_t OrdStatus = 39;
    static constexpr std::uint32_t OrdType = 40;
    static constexpr std::uint32_t OrderID = 37;
    static constexpr std::uint32_t OrderQty = 38;
    static constexpr std::uint32_t OrigClOrdID = 41;
    static constexpr std::uint32_t Price = 44;
    static constexpr std::uint32_t SenderCompID = 49;
    static constexpr std::uint32_t Side = 54;
    static constexpr std::uint32_t Symbol = 55;
    static constexpr std::uint32_t Text = 58;
    static constexpr std::uint32_t TimeInForce = 59;
    static constexpr std::uint32_t TransactTime = 60;
};

// An order the gateway admitted: its id, the client's reference to it, the shares still open,
// the comp id's session that entered it, which its reports go to, and the Account that owns it -
// its port's, whichever comp id entered it.
// It also keeps what the client entered it with that its reports name: Symbol, Side, TimeInForce, DisplayInst.
struct Order {
    std::uint64_t id = 0;
    std::string reference;
    std::uint64_t remaining = 0;
    std::size_t client = 0;
    std::size_t account = 0;   // the Account that owns it: its port's
    std::string entered_symbol;
    std::string entered_side;
    std::string entered_time_in_force;
    std::string entered_display_inst;
    std::uint64_t quantity = 0;   // what it was admitted with, which its reports count executions against
    std::string requested;   // the ClOrdID the latest request naming it was sent under: its own until another names it
    std::uint64_t replaced_id = 0;   // the order it replaces, until the engine has replaced it
    std::string replaced_reference;   // and that order's reference, which its report names
    std::string asked;   // the replace's own text, until the engine has answered it: what a refusal of it answers
    std::string request;   // the request that admitted it, until the engine has taken or refused it: what that answer is written from
    std::optional<std::int64_t> price;   // the price the engine was told: entered, or as a replace restated it; none for a market order
    char display = 0;   // whether the engine was told to show it, its own code
    std::uint64_t executed_value = 0;   // each execution's price times its shares, summed: what its average price is read from
    char status = 0;   // FIX's OrdStatus once it has left the book - 2 filled, 4 canceled, 8 rejected - and 0 while it is open
    std::string owner{};   // the username of the client that entered it, whose risk controls count it
    long double valued = 0;   // the price its open shares are valued at by its client's exposure: the Ipc's, or a market order's at its security's declared price
    char pending = 0;   // the mark of the breach of its client's risk controls that canceled it, which tells the answer to that cancel apart; 0 until one does
};

// The quantity a replace leaves open. FIX's OrderQty is the order's total, executions included,
// which its replacement carries on; a replace may raise it, so what it leaves open is that less
// what the order chain has executed.
inline std::uint64_t liable(std::uint64_t size, const Order& order) {
    const std::uint64_t executed = order.quantity - order.remaining;
    return size > executed ? size - executed : 0;
}

// NewOrderSingle -> new_order_message  (NsmEquities.Ipc.Transforms.xml: fix_new_order)
inline ipc_ns::new_order_message to_fix_new_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::new_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.price.set(static_cast<value_of<decltype(out.price)>>(2147483647));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>('L'));
    out.peg_offset.set(static_cast<value_of<decltype(out.peg_offset)>>(0));
    out.trade_now.set(static_cast<value_of<decltype(out.trade_now)>>('N'));
    out.max_floor.set(static_cast<value_of<decltype(out.max_floor)>>(0));
    out.random_reserve.set(static_cast<value_of<decltype(out.random_reserve)>>(0));
    out.discretion_price.set(static_cast<value_of<decltype(out.discretion_price)>>(0));
    out.discretion_type.set(static_cast<value_of<decltype(out.discretion_type)>>('L'));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>(0));
    out.self_match_strategy.set(static_cast<value_of<decltype(out.self_match_strategy)>>('N'));
    out.self_match_level.set(static_cast<value_of<decltype(out.self_match_level)>>('N'));
    out.aiq_strategy.set(static_cast<value_of<decltype(out.aiq_strategy)>>('N'));
    out.expire_time.set(static_cast<value_of<decltype(out.expire_time)>>(0));
    out.late_limit.set(static_cast<value_of<decltype(out.late_limit)>>('R'));
    out.extended_close.set(static_cast<value_of<decltype(out.extended_close)>>('Y'));
    out.extended_life_book.set(static_cast<value_of<decltype(out.extended_life_book)>>('N'));
    out.symbol.set(protocol::field(m, Tag::Symbol));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case '1': return 'B'; case '2': return 'S'; case '5': return 'S'; case '6': return 'S'; default: return value; } }(protocol::field_char(m, Tag::Side))));
    out.short_sale.set(static_cast<value_of<decltype(out.short_sale)>>([](char value) { switch (value) { case '1': return 'B'; case '2': return 'S'; case '5': return 'T'; case '6': return 'E'; default: return value; } }(protocol::field_char(m, Tag::Side))));
    out.quantity.set(static_cast<value_of<decltype(out.quantity)>>(protocol::field_scaled(m, Tag::OrderQty, 0)));
    if (!protocol::field(m, Tag::Price).empty()) out.price.set(static_cast<value_of<decltype(out.price)>>(protocol::field_scaled(m, Tag::Price, 4)));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>([](char value) { switch (value) { case '1': return '5'; case 'A': return '5'; case '2': return '0'; case '7': return '0'; case '\0': return '0'; default: return value; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.cross_type.set(static_cast<value_of<decltype(out.cross_type)>>([](char value) { switch (value) { case '2': return 'O'; case '7': return 'C'; case 'E': return 'A'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.display.set(static_cast<value_of<decltype(out.display)>>([](char value) { switch (value) { case 'A': return 'A'; case 'N': return 'N'; case 'L': return 'A'; case '\0': return 'Y'; default: return 'Y'; } }(protocol::field_char(m, Tag::DisplayInst))));
    out.post_only.set(static_cast<value_of<decltype(out.post_only)>>([](char value) { switch (value) { case 'P': return 'P'; case 'L': return 'P'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::DisplayInst))));
    out.imbalance_only.set(static_cast<value_of<decltype(out.imbalance_only)>>([](char value) { switch (value) { case 'I': return 'Y'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::DisplayInst))));
    out.min_qty.set(static_cast<value_of<decltype(out.min_qty)>>(protocol::field_scaled(m, Tag::MinQty, 0)));
    out.attribution.set(protocol::field(m, Tag::ClientID));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// OrderCancelRequest -> cancel_order_message  (NsmEquities.Ipc.Transforms.xml: fix_cancel_order)
inline ipc_ns::cancel_order_message to_fix_cancel_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.symbol.set(protocol::field(m, Tag::Symbol));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// OrderCancelReplaceRequest -> replace_order_message  (NsmEquities.Ipc.Transforms.xml: fix_replace_order)
inline ipc_ns::replace_order_message to_fix_replace_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::replace_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.replaced_id));
    out.replacement_id.set(static_cast<value_of<decltype(out.replacement_id)>>(order.id));
    out.price.set(static_cast<value_of<decltype(out.price)>>(2147483647));
    out.display.set(static_cast<value_of<decltype(out.display)>>(order.display));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>('L'));
    out.peg_offset.set(static_cast<value_of<decltype(out.peg_offset)>>(0));
    out.trade_now.set(static_cast<value_of<decltype(out.trade_now)>>('N'));
    out.max_floor.set(static_cast<value_of<decltype(out.max_floor)>>(0));
    out.random_reserve.set(static_cast<value_of<decltype(out.random_reserve)>>(0));
    out.discretion_price.set(static_cast<value_of<decltype(out.discretion_price)>>(0));
    out.discretion_type.set(static_cast<value_of<decltype(out.discretion_type)>>('L'));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>(0));
    out.self_match_strategy.set(static_cast<value_of<decltype(out.self_match_strategy)>>('N'));
    out.self_match_level.set(static_cast<value_of<decltype(out.self_match_level)>>('N'));
    out.aiq_strategy.set(static_cast<value_of<decltype(out.aiq_strategy)>>('N'));
    out.extended_close.set(static_cast<value_of<decltype(out.extended_close)>>('Y'));
    out.extended_life_book.set(static_cast<value_of<decltype(out.extended_life_book)>>('N'));
    out.symbol.set(protocol::field(m, Tag::Symbol));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case '1': return 'B'; case '2': return 'S'; case '5': return 'S'; case '6': return 'S'; default: return value; } }(protocol::field_char(m, Tag::Side))));
    out.short_sale.set(static_cast<value_of<decltype(out.short_sale)>>([](char value) { switch (value) { case '1': return 'B'; case '2': return 'S'; case '5': return 'T'; case '6': return 'E'; default: return value; } }(protocol::field_char(m, Tag::Side))));
    out.quantity.set(static_cast<value_of<decltype(out.quantity)>>(liable(static_cast<std::uint64_t>(static_cast<value_of<decltype(out.quantity)>>(protocol::field_scaled(m, Tag::OrderQty, 0))), order)));
    if (!protocol::field(m, Tag::Price).empty()) out.price.set(static_cast<value_of<decltype(out.price)>>(protocol::field_scaled(m, Tag::Price, 4)));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>([](char value) { switch (value) { case '1': return '5'; case 'A': return '5'; case '2': return '0'; case '7': return '0'; case '\0': return '0'; default: return value; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.cross_type.set(static_cast<value_of<decltype(out.cross_type)>>([](char value) { switch (value) { case '2': return 'O'; case '7': return 'C'; case 'E': return 'A'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.post_only.set(static_cast<value_of<decltype(out.post_only)>>([](char value) { switch (value) { case 'P': return 'P'; case 'L': return 'P'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::DisplayInst))));
    out.imbalance_only.set(static_cast<value_of<decltype(out.imbalance_only)>>([](char value) { switch (value) { case 'I': return 'Y'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::DisplayInst))));
    out.min_qty.set(static_cast<value_of<decltype(out.min_qty)>>(protocol::field_scaled(m, Tag::MinQty, 0)));
    out.attribution.set(protocol::field(m, Tag::ClientID));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_order_new)
inline void to_fix_order_new(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"0"});
    out.put(Tag::OrdStatus, std::string_view{"0"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    if (!protocol::field(m, Tag::Price).empty()) { out.put(Tag::Price, protocol::field(m, Tag::Price)); }
    if (!protocol::field(m, Tag::DisplayInst).empty()) { out.put(Tag::DisplayInst, protocol::field(m, Tag::DisplayInst)); }
    if (!protocol::field(m, Tag::TimeInForce).empty()) { out.put(Tag::TimeInForce, protocol::field(m, Tag::TimeInForce)); }
    out.put(Tag::OrdType, protocol::field(m, Tag::OrdType));
    out.put_time(Tag::TransactTime, now_ns);
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_order_rejected)
inline void to_fix_order_rejected(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    if (!protocol::field(m, Tag::Price).empty()) { out.put(Tag::Price, protocol::field(m, Tag::Price)); }
    out.put(Tag::OrdType, protocol::field(m, Tag::OrdType));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_quantity)
inline void to_fix_rejected_quantity(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"3"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_symbol)
inline void to_fix_rejected_symbol(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"1"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_side)
inline void to_fix_rejected_side(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_type)
inline void to_fix_rejected_type(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_unpriced)
inline void to_fix_rejected_unpriced(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_price)
inline void to_fix_rejected_price(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_locate_short)
inline void to_fix_rejected_locate_short(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_rejected_locate_exempt)
inline void to_fix_rejected_locate_exempt(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// trade_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_filled_resting)
inline void to_fix_filled_resting(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"2"});
    out.put(Tag::OrdStatus, std::string_view{"2"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    if (order.price) { out.put_scaled(Tag::Price, *order.price, 4); }
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::LiquidityFlag, std::string_view{"A"});
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
}

// trade_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_partially_filled_resting)
inline void to_fix_partially_filled_resting(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"1"});
    out.put(Tag::OrdStatus, std::string_view{"1"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    if (order.price) { out.put_scaled(Tag::Price, *order.price, 4); }
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::LiquidityFlag, std::string_view{"A"});
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
}

// trade_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_filled_aggressor)
inline void to_fix_filled_aggressor(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"2"});
    out.put(Tag::OrdStatus, std::string_view{"2"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    if (order.price) { out.put_scaled(Tag::Price, *order.price, 4); }
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::LiquidityFlag, std::string_view{"R"});
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
}

// trade_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_partially_filled_aggressor)
inline void to_fix_partially_filled_aggressor(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"1"});
    out.put(Tag::OrdStatus, std::string_view{"1"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    if (order.price) { out.put_scaled(Tag::Price, *order.price, 4); }
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::LiquidityFlag, std::string_view{"R"});
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
}

// order_canceled_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_canceled)
inline void to_fix_canceled(protocol::Writer& out, const ipc_ns::order_canceled_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.requested});
    if (order.price) { out.put_scaled(Tag::Price, *order.price, 4); }
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
}

// order_eliminated_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_eliminated)
inline void to_fix_eliminated(protocol::Writer& out, const ipc_ns::order_eliminated_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    if (order.price) { out.put_scaled(Tag::Price, *order.price, 4); }
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
}

// cancel_pending_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_cancel_pending)
inline void to_fix_cancel_pending(protocol::Writer& out, const ipc_ns::cancel_pending_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"6"});
    out.put(Tag::OrdStatus, std::string_view{"6"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.requested});
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
}

// order_replaced_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_replaced)
inline void to_fix_replaced(protocol::Writer& out, const ipc_ns::order_replaced_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"5"});
    out.put(Tag::OrdStatus, std::string_view{"0"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.requested});
    out.put_scaled(Tag::Price, static_cast<std::int64_t>(m.price.get().value()), 4);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::OrigClOrdID, std::string_view{order.replaced_reference});
}

// OrderCancelRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_cancel_too_late)
inline void to_fix_cancel_too_late(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::CxlRejReason, std::string_view{"0"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"1"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_replace_too_late)
inline void to_fix_replace_too_late(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::CxlRejReason, std::string_view{"0"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
}

// OrderCancelRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_cancel_unknown)
inline void to_fix_cancel_unknown(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, std::string_view{"Unknown"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::CxlRejReason, std::string_view{"1"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"1"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_replace_unknown)
inline void to_fix_replace_unknown(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, std::string_view{"Unknown"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::CxlRejReason, std::string_view{"1"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_share_size)
inline void to_fix_risk_share_size(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0031"});
    out.put(Tag::OrdRejReason, std::string_view{"3"});
    out.put(Tag::Text, std::string_view{"p"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_share_size_replace)
inline void to_fix_risk_share_size_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0031"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_notional)
inline void to_fix_risk_notional(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0030"});
    out.put(Tag::OrdRejReason, std::string_view{"3"});
    out.put(Tag::Text, std::string_view{"n"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_notional_unpriced)
inline void to_fix_risk_notional_unpriced(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0030"});
    out.put(Tag::OrdRejReason, std::string_view{"3"});
    out.put(Tag::Text, std::string_view{"n"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_notional_replace)
inline void to_fix_risk_notional_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0030"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_notional_unpriced_replace)
inline void to_fix_risk_notional_unpriced_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0030"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_restricted)
inline void to_fix_risk_restricted(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0022"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"a"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_hard_to_borrow)
inline void to_fix_risk_hard_to_borrow(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0023"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"b"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_pre_market)
inline void to_fix_risk_pre_market(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x002D"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"o"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_post_market)
inline void to_fix_risk_post_market(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x002E"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"o"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_after_hours)
inline void to_fix_risk_after_hours(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x002E"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"o"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_pre_market_replace)
inline void to_fix_risk_pre_market_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x002D"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_post_market_replace)
inline void to_fix_risk_post_market_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x002E"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_after_hours_replace)
inline void to_fix_risk_after_hours_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x002E"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_short_sale)
inline void to_fix_risk_short_sale(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x002B"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"o"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_short_sale_exempt)
inline void to_fix_risk_short_sale_exempt(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x002F"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"o"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_market_impact_buy)
inline void to_fix_risk_market_impact_buy(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0021"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"r"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_market_impact_sell)
inline void to_fix_risk_market_impact_sell(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0021"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"r"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_market_impact_buy_replace)
inline void to_fix_risk_market_impact_buy_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0021"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_market_impact_sell_replace)
inline void to_fix_risk_market_impact_sell_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0021"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_average_daily_volume)
inline void to_fix_risk_average_daily_volume(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0025"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"d"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_average_daily_volume_replace)
inline void to_fix_risk_average_daily_volume_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0025"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_port_rate)
inline void to_fix_risk_port_rate(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0029"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"u"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_port_rate_replace)
inline void to_fix_risk_port_rate_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0029"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_symbol_rate)
inline void to_fix_risk_symbol_rate(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0028"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"v"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_symbol_rate_replace)
inline void to_fix_risk_symbol_rate_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0028"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_duplicate)
inline void to_fix_risk_duplicate(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x002A"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"w"});
}

// NewOrderSingle -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_stopped)
inline void to_fix_risk_stopped(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::Symbol, protocol::field(m, Tag::Symbol));
    out.put(Tag::Side, protocol::field(m, Tag::Side));
    out.put(Tag::OrderQty, protocol::field(m, Tag::OrderQty));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, std::string_view{"0"});
    out.put(Tag::AvgPx, std::string_view{"0"});
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put_time(Tag::TransactTime, now_ns);
    out.put(Tag::ExtendedReasonCode, std::string_view{"0x0020"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (NsmEquities.Ipc.Transforms.xml: fix_risk_stopped_replace)
inline void to_fix_risk_stopped_replace(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"0x0020"});
}

// order_canceled_message -> ExecutionReport  (NsmEquities.Ipc.Transforms.xml: fix_risk_canceled)
inline void to_fix_risk_canceled(protocol::Writer& out, const ipc_ns::order_canceled_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::ExecBroker, std::string_view{"INET"});
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::LeavesQty, std::string_view{"0"});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put_scaled(Tag::AvgPx, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.requested});
    if (order.price) { out.put_scaled(Tag::Price, *order.price, 4); }
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
    out.put(Tag::Text, std::string_view{"Z"});
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
    // Its place in that interleave is its port's too, and so names the Account the port is bound to.
    explicit Gateway(Emit emit, std::uint64_t first = 1, std::uint64_t step = 1, Clock clock = {})
        : emit_(emit), clock_(clock), account_(first), next_id_(first), step_(step) {
        config_.comp_id = "INET";
        next_report_ = first;
    }

    // A login the gateway accepts, by the SenderCompID a client logs on with; the model's Logon carries no password, so none is checked.
    // With none registered, any login is accepted.
    void allow(std::string username, std::string password) { credentials_[std::move(username)] = std::move(password); }

    // A new connection, writing through `send`; returns its number. Whose session it carries is
    // known at its first message, by the comp id that names it sends from.
    std::size_t connect(Send send) {
        const auto connection = next_connection_++;
        connections_.emplace(connection, Connection{ std::move(send), npos, {}, false });
        return connection;
    }

    // A connection that has gone. The session it carried waits for its comp id's next - told the
    // connection dropped, where it had not ended already - and what it left resting stays in the book.
    void disconnect(std::size_t connection) {
        const auto found = connections_.find(connection);
        if (found == connections_.end()) return;
        if (found->second.client != npos) {
            auto& client = *clients_.at(found->second.client);
            client.connection = npos;
            client.session.dropped(clock_());
            if (client.risk != nullptr && !client.logged_out) disconnected(*client.risk);   // its session ended without its logout: what a breach of that cancels
        }
        connections_.erase(found);
    }

    // Bytes from a connection, in whatever pieces they came. Until its first whole message it carries
    // no session: that message names the comp id it is from, whose session it takes up - kept from
    // its last connection, or new. A comp id already carried on another connection is not taken up twice.
    void on_bytes(std::size_t connection, const char* data, std::size_t length) {
        const auto found = connections_.find(connection);
        if (found == connections_.end()) return;
        auto& held = found->second;
        if (held.client != npos) {
            held.foreign = !clients_.at(held.client)->session.on_bytes(data, length, clock_());
            return;
        }

        held.pending.append(data, length);
        using Session = protocol::Acceptor<Send, Client&>;
        const auto cut = Session::framed(held.pending);
        if (cut.is == Session::Cuts::Partial) return;
        if (cut.is != Session::Cuts::Whole) { held.foreign = true; return; }
        auto& client = taken(protocol::field(std::string_view{ held.pending }.substr(0, cut.length), Tag::SenderCompID));
        if (client.connection != npos) { held.foreign = true; return; }

        client.connection = connection;
        held.client = client.index;
        client.session.reconnect(clock_());   // a session that had ended takes this connection up; a new one is connected already
        const std::string pending = std::move(held.pending);
        held.foreign = !client.session.on_bytes(pending.data(), pending.size(), clock_());
    }

    // Is a connection over - its session logged out, refused or gone silent, or the connection not
    // FIX at all? It is then the loop's to close; the session waits for the next.
    bool ended(std::size_t connection) const {
        const auto found = connections_.find(connection);
        if (found == connections_.end() || found->second.foreign) return true;
        return found->second.client != npos && clients_.at(found->second.client)->session.ended();
    }

    // The time: every session fires what is due by it - heartbeats, test requests, the end of a
    // session gone silent.
    void tick() {
        const auto now = clock_();
        for (auto& held : clients_) held.second->session.tick(now);
    }

    std::size_t open_orders() const { return orders_.size(); }

    // The engine's trade_message: answered to the session that entered each order it names, as the event leaves it.
    void on_event(const ipc_ns::trade_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) {
            found->second.executed_value += static_cast<std::uint64_t>(m.price.get().value()) * m.quantity.get().value();
            Order answered = found->second;
            answered.remaining -= std::min<std::uint64_t>(m.quantity.get().value(), answered.remaining);
            if (answered.remaining == 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_filled_resting(out, m, answered, now, reported()); }, now);
            if (answered.remaining != 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_partially_filled_resting(out, m, answered, now, reported()); }, now);
        }
        if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) {
            found->second.executed_value += static_cast<std::uint64_t>(m.price.get().value()) * m.quantity.get().value();
            Order answered = found->second;
            answered.remaining -= std::min<std::uint64_t>(m.quantity.get().value(), answered.remaining);
            if (answered.remaining == 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_filled_aggressor(out, m, answered, now, reported()); }, now);
            if (answered.remaining != 0) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_partially_filled_aggressor(out, m, answered, now, reported()); }, now);
        }
        { Risk* exposed_by = exposed(m.resting_order_id.get().value(), m.quantity.get().value(), static_cast<long double>(m.price.get().value_or(0))); fill(m.resting_order_id.get().value(), m.quantity.get().value()); if (exposed_by != nullptr) broken(*exposed_by, 0, 0); }
        { Risk* exposed_by = exposed(m.aggressor_order_id.get().value(), m.quantity.get().value(), static_cast<long double>(m.price.get().value_or(0))); fill(m.aggressor_order_id.get().value(), m.quantity.get().value()); if (exposed_by != nullptr) broken(*exposed_by, 0, 0); }
    }

    // The engine's order_canceled_message: answered to the session that entered each order it names, as the event leaves it.
    void on_event(const ipc_ns::order_canceled_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            if (answered.pending != static_cast<char>('Z')) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_canceled(out, m, answered, now, reported()); }, now);
            if (answered.pending == static_cast<char>('Z')) respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_risk_canceled(out, m, answered, now, reported()); }, now);
        }
        close(m.order_id.get().value());
    }

    // The engine's order_eliminated_message: answered to the session that entered each order it names, as the event leaves it.
    void on_event(const ipc_ns::order_eliminated_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_eliminated(out, m, answered, now, reported()); }, now);
        }
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's cancel_pending_message: answered to the session that entered each order it names, as the event leaves it.
    void on_event(const ipc_ns::cancel_pending_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_cancel_pending(out, m, answered, now, reported()); }, now);
        }
    }

    // The engine's order_replaced_message: the replacement known by the number the engine gives it, then answered.
    void on_event(const ipc_ns::order_replaced_message& m) {
        const auto now = clock_();
        const std::uint64_t replaced = m.order_id.get().value();
        const std::uint64_t stands = m.replacement_id.get().value();
        const auto given = replacing_.find(replaced);
        if (given == replacing_.end()) return;   // a replace this gateway did not ask for
        const std::uint64_t asked = given->second;
        replacing_.erase(given);
        if (stands == replaced) {
            // It kept its place: the order keeps its number, and takes the replacement's reference.
            auto kept = orders_.find(replaced);
            auto replacement = orders_.find(asked);
            if (kept == orders_.end() || replacement == orders_.end()) return;
            references_.erase({ kept->second.client, kept->second.reference });
            references_[{ replacement->second.client, replacement->second.reference }] = replaced;
            replacement->second.id = replaced;
            kept->second = replacement->second;
            orders_.erase(replacement);
        } else {
            close(replaced);
        }
        const auto found = orders_.find(stands);
        if (found == orders_.end()) return;
        found->second.remaining = m.quantity.get().value();   // what of it rests
        found->second.replaced_id = 0;
        found->second.asked.clear();
        const Order& answered = found->second;
        respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_replaced(out, m, answered, now, reported()); }, now);
        // What waited on the replace is taken now, in the order it came.
        const std::size_t client = answered.client;
        if (auto waited = held_.find(asked); waited != held_.end()) {
            auto requests = std::move(waited->second);
            held_.erase(waited);
            for (const auto& waiting : requests) { awaiting_.erase({ client, waiting.introduces }); message(client, waiting.type, waiting.text); }
        }
    }

    // The engine's phase_announced_message: what a client risk control reads of it, kept.
    void on_event(const ipc_ns::phase_announced_message& m) {
        if (const auto heard = m.announcement.get().value(); static_cast<char>(heard) == 'O' || static_cast<char>(heard) == 'S' || static_cast<char>(heard) == 'Q' || static_cast<char>(heard) == 'M' || static_cast<char>(heard) == 'E' || static_cast<char>(heard) == 'C') heard_phase_announced_message_announcement_ = static_cast<std::int64_t>(heard);   // the latest it said of those the order entry keeps, which a client risk control reads
    }

    // The engine's order_accepted_message: the request that admitted the order answered, now the engine has it.
    void on_event(const ipc_ns::order_accepted_message& m) {
        const auto now = clock_();
        const auto found = orders_.find(m.order_id.get().value());
        if (found == orders_.end()) return;
        Order& answered = found->second;
        respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_order_new(out, answered.request, answered, now, reported()); }, now);
        answered.request.clear();
    }

    // The engine's order_refused_message: the request that admitted the order answered, now the engine has it.
    void on_event(const ipc_ns::order_refused_message& m) {
        const auto now = clock_();
        const auto found = orders_.find(m.order_id.get().value());
        if (found == orders_.end()) return;
        if (found->second.replaced_id != 0) {
            Order answered = found->second;
            replacing_.erase(answered.replaced_id);
            references_.erase({ answered.client, answered.reference });
            orders_.erase(found);
            answered.id = answered.replaced_id;   // the order it was to replace, which stands
            respond(answered.client, "9", [&](protocol::Writer& out) { to_fix_replace_too_late(out, answered.asked, answered, now); }, now);
            if (auto waited = held_.find(m.order_id.get().value()); waited != held_.end()) {
                auto requests = std::move(waited->second);
                held_.erase(waited);
                for (const auto& waiting : requests) { awaiting_.erase({ answered.client, waiting.introduces }); message(answered.client, waiting.type, waiting.text); }
            }
            return;
        }
        Order& answered = found->second;
        respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_order_rejected(out, answered.request, answered, now, reported()); }, now);
        close(m.order_id.get().value(), '8');   // refused: it never stood in the book
    }

private:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    // A connection: what writes to it, the session it carries once its first message says whose,
    // what came before that whole, and whether it is FIX at all.
    struct Connection {
        Send send;
        std::size_t client;
        std::string pending;
        bool foreign;
    };

    // A comp id's session: kept from its last connection, or begun.
    struct Client;
    Client& taken(std::string_view comp_id) {
        if (const auto known = comp_ids_.find(comp_id); known != comp_ids_.end()) return *clients_.at(known->second);
        const auto client = next_client_++;
        comp_ids_.emplace(std::string(comp_id), client);
        return *clients_.emplace(client, std::make_unique<Client>(*this, client)).first->second;
    }

    // What a session writes, onto the connection carrying it; nothing while it has none.
    void write(std::size_t connection, const char* data, std::size_t length) {
        if (const auto found = connections_.find(connection); found != connections_.end()) found->second.send(data, length);
    }

    // The risk controls the venue declares of a client, each as its <Client> states it: a control it leaves out is
    // off, and a client stating none has no limit.
    struct Controls {
        bool share_size_control = false;   // ShareSizeControl
        long double share_size_control_shares = 0;   // its Shares
        bool maximum_single_order_notional = false;   // MaximumSingleOrderNotional
        long double maximum_single_order_notional_dollars = 0;   // its Dollars
        bool restricted_stock_list = false;   // RestrictedStockList
        std::vector<std::string_view> restricted_stock_list_symbol{};   // its Symbol list
        bool hard_to_borrow_list = false;   // HardToBorrowList
        std::vector<std::string_view> hard_to_borrow_list_symbol{};   // its Symbol list
        bool order_type_check_pre_market = false;   // OrderTypeCheck.PreMarket
        bool order_type_check_post_market = false;   // OrderTypeCheck.PostMarket
        bool order_type_check_short_sale = false;   // OrderTypeCheck.ShortSale
        bool order_type_check_short_sale_exempt = false;   // OrderTypeCheck.ShortSaleExempt
        bool market_impact_check = false;   // MarketImpactCheck
        bool average_daily_volume_check = false;   // AverageDailyVolumeCheck
        long double average_daily_volume_check_minimum_shares = 0;   // its MinimumShares
        long double average_daily_volume_check_percent = 0;   // its Percent
        bool rate_thresholds_port = false;   // RateThresholds.Port
        long double rate_thresholds_port_messages = 0;   // its Messages
        long double rate_thresholds_port_milliseconds = 0;   // its Milliseconds
        bool rate_thresholds_symbol = false;   // RateThresholds.Symbol
        long double rate_thresholds_symbol_messages = 0;   // its Messages
        long double rate_thresholds_symbol_milliseconds = 0;   // its Milliseconds
        bool duplication_control = false;   // DuplicationControl
        long double duplication_control_seconds = 0;   // its Seconds
        bool kill_switch_gross_executed_exposure = false;   // KillSwitch.GrossExecutedExposure
        long double kill_switch_gross_executed_exposure_dollars = 0;   // its Dollars
        bool kill_switch_gross_notional_exposure = false;   // KillSwitch.GrossNotionalExposure
        long double kill_switch_gross_notional_exposure_dollars = 0;   // its Dollars
        bool gross_exposure_check = false;   // GrossExposureCheck
        std::vector<std::string_view> gross_exposure_check_measure{};   // its Measure list
        std::vector<std::string_view> gross_exposure_check_cancel_open_orders{};   // its CancelOpenOrders list
        long double gross_exposure_check_dollars = 0;   // its Dollars
        bool cancel_on_disconnect = false;   // CancelOnDisconnect
    };

    static Controls controlled(std::string_view username) {
        Controls controls{};
        if (username == "RISKST") {
            controls.share_size_control = true;
            controls.share_size_control_shares = 5000;
            controls.maximum_single_order_notional = true;
            controls.maximum_single_order_notional_dollars = 2000;
            controls.order_type_check_pre_market = true;
            controls.order_type_check_post_market = true;
            controls.order_type_check_short_sale = true;
            controls.order_type_check_short_sale_exempt = true;
            controls.hard_to_borrow_list = true;
            controls.hard_to_borrow_list_symbol = { "ZWZZT" };
            controls.market_impact_check = true;
            controls.average_daily_volume_check = true;
            controls.average_daily_volume_check_minimum_shares = 100000;
            controls.average_daily_volume_check_percent = 1;
        }
        if (username == "RSTRCT") {
            controls.restricted_stock_list = true;
            controls.restricted_stock_list_symbol = { "ZXZZT" };
        }
        if (username == "RATEDP") {
            controls.rate_thresholds_port = true;
            controls.rate_thresholds_port_messages = 10;
            controls.rate_thresholds_port_milliseconds = 1000;
            controls.rate_thresholds_symbol = true;
            controls.rate_thresholds_symbol_messages = 4;
            controls.rate_thresholds_symbol_milliseconds = 1000;
            controls.duplication_control = true;
            controls.duplication_control_seconds = 2;
        }
        if (username == "KILLSW") {
            controls.kill_switch_gross_executed_exposure = true;
            controls.kill_switch_gross_executed_exposure_dollars = 300;
            controls.kill_switch_gross_notional_exposure = true;
            controls.kill_switch_gross_notional_exposure_dollars = 2000;
            controls.cancel_on_disconnect = true;
        }
        if (username == "GROSSX") {
            controls.gross_exposure_check = true;
            controls.gross_exposure_check_measure = { "Open" };
            controls.gross_exposure_check_cancel_open_orders = { "Yes" };
            controls.gross_exposure_check_dollars = 1000;
        }
        if (username == "FIXRSK") {
            controls.share_size_control = true;
            controls.share_size_control_shares = 5000;
            controls.maximum_single_order_notional = true;
            controls.maximum_single_order_notional_dollars = 2000;
            controls.order_type_check_pre_market = true;
            controls.order_type_check_post_market = true;
            controls.order_type_check_short_sale = true;
            controls.order_type_check_short_sale_exempt = true;
            controls.restricted_stock_list = true;
            controls.restricted_stock_list_symbol = { "ZXZZT" };
            controls.market_impact_check = true;
            controls.duplication_control = true;
            controls.duplication_control_seconds = 2;
            controls.gross_exposure_check = true;
            controls.gross_exposure_check_measure = { "Open" };
            controls.gross_exposure_check_cancel_open_orders = { "Yes" };
            controls.gross_exposure_check_dollars = 1000;
            controls.cancel_on_disconnect = true;
        }
        return controls;
    }

    // What the gateway holds of a client for its risk controls, by the username it logs in as: kept the day,
    // across every connection it logs in on, as the port's Account is. Its orders are those it entered.
    struct Risk {
        std::string username{};   // the client's
        Controls controls{};   // its risk controls, as the venue declares them
        std::deque<std::int64_t> rate_thresholds_port{};   // RateThresholds.Port: when each of its messages within the window came, on the venue's clock
        std::map<std::string, std::deque<std::int64_t>, std::less<>> rate_thresholds_symbol{};   // RateThresholds.Symbol: the same, for the messages naming each instrument
        std::deque<std::pair<std::int64_t, std::string>> duplication_control{};   // DuplicationControl: when each order it entered within the window came, and what it is told apart by
        long double executed = 0;   // what its orders have executed: shares times the price, in the Ipc's price units, each side counted positive
        bool stopped = false;   // has a breach stopped it entering orders for the rest of the day?
    };

    // A request held behind a replace the engine has not answered: its type, its text, and the
    // reference it gives, where it is a replace itself.
    struct Held {
        std::string type;
        std::string text;
        std::string introduces;
    };

    // A comp id's session, kept all day: its numbers, what it sent, and the orders it entered
    // outlive any one connection, and it writes through whichever carries it - none while away.
    struct Client {
        Gateway& gateway;
        std::size_t index;
        std::size_t connection = npos;
        Send send;
        protocol::Acceptor<Send, Client&> session;
        Controls controls{};   // the risk controls the venue declares of the client it logged on as: none until it does
        Risk* risk = nullptr;   // what the gateway holds of the client it logged on as, for its risk controls: none until it does
        bool logged_out = false;   // did its session end by its logout, or its logon's refusal, rather than by a disruption?
        bool unextended = false;   // does its port keep its LOC Orders out of the Extended Trading Close, as the venue declares the client it logged on as
        std::uint32_t protection_percent = 0;   // how far through the book's best opposite price its port lets an order be priced, in hundredths of a percent of it, as the venue declares the client: 0 for none
        std::int64_t protection_amount = 0;   // and as an amount, at the Ipc's price decimals: 0 for none

        Client(Gateway& owner, std::size_t at)
            : gateway(owner), index(at), send([this](const char* data, std::size_t length) { gateway.write(connection, data, length); }),
              session(owner.config_, send, *this) {}

        bool login(std::string_view, std::string_view text, std::string& refusal) {
            const bool accepted = gateway.login(text, refusal);
            if (accepted) controls = controlled(protocol::field(text, Tag::SenderCompID));   // the risk controls the venue declares of it
            if (accepted) { unextended = unextending(protocol::field(text, Tag::SenderCompID)); }   // its port's choice for its LOC Orders
            if (accepted) { protecting(protocol::field(text, Tag::SenderCompID), protection_percent, protection_amount); }   // its port's limit on how far through the book's best opposite price an order is priced
            if (accepted) { risk = &gateway.risks_[std::string(protocol::field(text, Tag::SenderCompID))]; risk->username = std::string(protocol::field(text, Tag::SenderCompID)); risk->controls = controls; }   // what the gateway holds of the client, kept the day across its sessions
            logged_out = !accepted;
            return accepted;
        }
        void message(std::string_view type, std::string_view text) { gateway.message(index, type, text); }
        void logout() { logged_out = true; }   // the session ended by its logout, no disruption
    };

    // Does a client's port keep its LOC Orders out of the Extended Trading Close, as the venue declares it:
    // Nasdaq's "a Participant may opt to disable an LOC Order from participating" (Equity 4, Rule
    // 4702(b)(12)(A)).
    static bool unextending(std::string_view username) {
        return username == "FIXOPT";
    }

    // How far through the book's best opposite price each client's port lets an order be priced, as the venue
    // declares it: Nasdaq's Fat Finger Protection, "percentage-based and dollar-based controls are available"
    // (Equity 6, Section 5(m)) - a percent of that price, in hundredths of a percent, and an amount at the Ipc's
    // price decimals, each 0 for none. The engine measures it, holding the book.
    static void protecting(std::string_view username, std::uint32_t& percent, std::int64_t& amount) {
        if (username == "FATFNG") { percent = 500; amount = 200; return; }
        percent = 0;
        amount = 0;
    }

    // An order or a replace of a client, carrying its port's limit to the engine.
    template <typename Ipc>
    static void protect(Ipc& ipc, std::uint32_t percent, std::int64_t amount) {
        ipc.protection_percent.set(static_cast<std::remove_cvref_t<decltype(ipc.protection_percent.get().value())>>(percent));
        ipc.protection_amount.set(static_cast<std::remove_cvref_t<decltype(ipc.protection_amount.get().value())>>(amount));
    }

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
        if (clients_.at(client)->risk == nullptr) return;   // not logged in: nothing it sends is taken
        Risk& risk = *clients_.at(client)->risk;   // what the gateway holds of the client, for its risk controls
        if (risk.controls.rate_thresholds_port) tallied(risk.rate_thresholds_port, now, risk.controls.rate_thresholds_port_milliseconds * 1000000);   // RateThresholds.Port: every message it sends, within the window

        if (type == "D") {   // NewOrderSingle: fix_new_order
            if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(protocol::field(m, Tag::Symbol))], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
            const auto rejected = [&](auto&& report, std::string_view kind) {
                Order order{};
                order.id = numbered();
                order.reference = std::string(protocol::field(m, Tag::ClOrdID));
                order.client = client;
                order.account = account_;
                order.requested = order.reference;
                order.entered_symbol = std::string(protocol::field(m, Tag::Symbol));
                order.entered_side = std::string(protocol::field(m, Tag::Side));
                order.entered_time_in_force = std::string(protocol::field(m, Tag::TimeInForce));
                order.entered_display_inst = std::string(protocol::field(m, Tag::DisplayInst));
                const auto ipc = to_fix_new_order(m, order);
                order.remaining = ipc.quantity.get().value();
                order.price = ipc.price.get();   // none for a market order
                order.display = static_cast<char>(ipc.display.get().value());
                order.quantity = order.remaining;
                respond(client, kind, [&](protocol::Writer& out) { report(out, order); }, now);
            };
            if (risk.stopped) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_stopped(out, m, order, now, reported()); }, "8"); return; }   // a breach stopped its client for the day
            if (!([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 1 && value <= 999999); }(protocol::field(m, Tag::OrderQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_quantity(out, m, order, now, reported()); }, "8"); return; }
            if (!(protocol::field(m, Tag::Symbol) == "ZVZZT" || protocol::field(m, Tag::Symbol) == "ZWZZT" || protocol::field(m, Tag::Symbol) == "ZXZZT")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_symbol(out, m, order, now, reported()); }, "8"); return; }
            if (!(protocol::field(m, Tag::Side) == "1" || protocol::field(m, Tag::Side) == "2" || protocol::field(m, Tag::Side) == "5" || protocol::field(m, Tag::Side) == "6")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_side(out, m, order, now, reported()); }, "8"); return; }
            if (!(protocol::field(m, Tag::OrdType) == "1" || protocol::field(m, Tag::OrdType) == "2")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_type(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::OrdType) == "2" && !(!protocol::field(m, Tag::Price).empty())) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_unpriced(out, m, order, now, reported()); }, "8"); return; }
            if (!([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 199999); }(protocol::field(m, Tag::Price)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_price(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::Side) == "5" && !(protocol::field(m, Tag::LocateReqd) == "N")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_locate_short(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::Side) == "6" && !(protocol::field(m, Tag::LocateReqd) == "N")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_rejected_locate_exempt(out, m, order, now, reported()); }, "8"); return; }
            const auto& controls = clients_.at(client)->controls;   // the risk controls the venue declares of the client
            if (controls.share_size_control && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) > controls.share_size_control_shares) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_share_size(out, m, order, now, reported()); }, "8"); return; }
            if (controls.maximum_single_order_notional && !(protocol::field(m, Tag::OrdType) == "1") && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) * static_cast<long double>(protocol::field_scaled(m, Tag::Price, 4)) > controls.maximum_single_order_notional_dollars * 10000) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_notional(out, m, order, now, reported()); }, "8"); return; }
            if (controls.maximum_single_order_notional && protocol::field(m, Tag::OrdType) == "1" && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) * listing_luld_reference(protocol::field(m, Tag::Symbol)) > controls.maximum_single_order_notional_dollars * 10000) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_notional_unpriced(out, m, order, now, reported()); }, "8"); return; }
            if (controls.restricted_stock_list && listed_in(controls.restricted_stock_list_symbol, protocol::field(m, Tag::Symbol))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_restricted(out, m, order, now, reported()); }, "8"); return; }
            if (controls.hard_to_borrow_list && (protocol::field(m, Tag::Side) == "5" || protocol::field(m, Tag::Side) == "6") && listed_in(controls.hard_to_borrow_list_symbol, protocol::field(m, Tag::Symbol))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_hard_to_borrow(out, m, order, now, reported()); }, "8"); return; }
            if (controls.order_type_check_pre_market && (heard_phase_announced_message_announcement_ == 'O' || heard_phase_announced_message_announcement_ == 'S') && (protocol::field(m, Tag::TimeInForce) == "1" || protocol::field(m, Tag::TimeInForce) == "3" || protocol::field(m, Tag::TimeInForce) == "5" || protocol::field(m, Tag::TimeInForce) == "6" || protocol::field(m, Tag::TimeInForce) == "A")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_pre_market(out, m, order, now, reported()); }, "8"); return; }
            if (controls.order_type_check_post_market && (heard_phase_announced_message_announcement_ == 'M' || heard_phase_announced_message_announcement_ == 'E') && (protocol::field(m, Tag::TimeInForce) == "1" || protocol::field(m, Tag::TimeInForce) == "3" || protocol::field(m, Tag::TimeInForce) == "5" || protocol::field(m, Tag::TimeInForce) == "6" || protocol::field(m, Tag::TimeInForce) == "A")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_post_market(out, m, order, now, reported()); }, "8"); return; }
            if (controls.order_type_check_post_market && protocol::field(m, Tag::TimeInForce) == "E") { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_after_hours(out, m, order, now, reported()); }, "8"); return; }
            if (controls.order_type_check_short_sale && protocol::field(m, Tag::Side) == "5") { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_short_sale(out, m, order, now, reported()); }, "8"); return; }
            if (controls.order_type_check_short_sale_exempt && protocol::field(m, Tag::Side) == "6") { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_short_sale_exempt(out, m, order, now, reported()); }, "8"); return; }
            if (controls.market_impact_check && protocol::field(m, Tag::Side) == "1" && !(protocol::field(m, Tag::OrdType) == "1") && static_cast<long double>(protocol::field_scaled(m, Tag::Price, 4)) > listing_luld_upper(protocol::field(m, Tag::Symbol))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_market_impact_buy(out, m, order, now, reported()); }, "8"); return; }
            if (controls.market_impact_check && (protocol::field(m, Tag::Side) == "2" || protocol::field(m, Tag::Side) == "5" || protocol::field(m, Tag::Side) == "6") && !(protocol::field(m, Tag::OrdType) == "1") && static_cast<long double>(protocol::field_scaled(m, Tag::Price, 4)) < listing_luld_lower(protocol::field(m, Tag::Symbol))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_market_impact_sell(out, m, order, now, reported()); }, "8"); return; }
            if (controls.average_daily_volume_check && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) > std::max(listing_average_daily_volume(protocol::field(m, Tag::Symbol)), controls.average_daily_volume_check_minimum_shares) * controls.average_daily_volume_check_percent / 100) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_average_daily_volume(out, m, order, now, reported()); }, "8"); return; }
            if (controls.rate_thresholds_port && static_cast<long double>(risk.rate_thresholds_port.size()) > controls.rate_thresholds_port_messages) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_port_rate(out, m, order, now, reported()); }, "8"); return; }
            if (controls.rate_thresholds_symbol && static_cast<long double>(risk.rate_thresholds_symbol[std::string(protocol::field(m, Tag::Symbol))].size()) > controls.rate_thresholds_symbol_messages) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_symbol_rate(out, m, order, now, reported()); }, "8"); return; }
            const std::string key_fix_risk_duplicate = std::string(protocol::field(m, Tag::Symbol)) + '\x1f' + std::string(protocol::field(m, Tag::Side)) + '\x1f' + std::string(protocol::field(m, Tag::OrderQty)) + '\x1f' + std::string(protocol::field(m, Tag::Price)) + '\x1f' + std::string(protocol::field(m, Tag::TimeInForce));   // what it is told apart by, for a duplicate
            if (controls.duplication_control && duplicated(risk.duplication_control, now, controls.duplication_control_seconds * 1000000000, key_fix_risk_duplicate)) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_duplicate(out, m, order, now, reported()); }, "8"); return; }
            const auto asked = to_fix_new_order(m, Order{});   // what the engine would be told of it, which its client's exposure values
            const long double priced = (asked.price.get().has_value() ? static_cast<long double>(asked.price.get().value()) : listing_luld_reference(protocol::field(m, Tag::Symbol)));   // what its open shares are valued at by its client's exposure
            if (broken(risk, static_cast<long double>(asked.quantity.get().value()) * priced, 0)) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_risk_stopped(out, m, order, now, reported()); }, "8"); return; }   // admitted, it would breach a limit of its client's
            Order order{ numbered(), std::string(protocol::field(m, Tag::ClOrdID)), 0, client, account_, {}, {}, {}, {}, 0, {}, 0, {}, {}, {}, 0, 0 };
            auto ipc = to_fix_new_order(m, order);
            if (clients_.at(client)->unextended) { ipc.extended_close.set(static_cast<std::remove_cvref_t<decltype(ipc.extended_close.get().value())>>('N')); }   // its port keeps its LOC Orders out of the Extended Trading Close
            protect(ipc, clients_.at(client)->protection_percent, clients_.at(client)->protection_amount);   // its port's limit on how far through the book's best opposite price an order is priced
            order.remaining = ipc.quantity.get().value();
            order.price = ipc.price.get();   // none for a market order
            order.display = static_cast<char>(ipc.display.get().value());
            order.valued = priced;
            order.owner = risk.username;   // the client that entered it
            order.quantity = order.remaining;
            order.requested = order.reference;
            order.entered_symbol = std::string(protocol::field(m, Tag::Symbol));
            order.entered_side = std::string(protocol::field(m, Tag::Side));
            order.entered_time_in_force = std::string(protocol::field(m, Tag::TimeInForce));
            order.entered_display_inst = std::string(protocol::field(m, Tag::DisplayInst));
            order.request = std::string(m);   // answered once the engine has it
            orders_[order.id] = order;
            references_[{ client, order.reference }] = order.id;
            detail_ipc::emit_framed(emit_, 'N', ipc);
            if (controls.duplication_control) risk.duplication_control.emplace_back(now, key_fix_risk_duplicate);   // DuplicationControl: an order it entered
            return;
        }

        if (type == "F") {   // OrderCancelRequest: fix_cancel_order
            const auto named = std::make_pair(client, std::string(protocol::field(m, Tag::OrigClOrdID)));
            const auto found = references_.find(named);
            if (found == references_.end()) {
                if (const auto awaited = awaiting_.find(named); awaited != awaiting_.end()) {   // the reference a held replace will give
                    Held waiting{ std::string(type), std::string(m), std::string{} };
                    if (!waiting.introduces.empty()) awaiting_[{ client, waiting.introduces }] = awaited->second;
                    held_[awaited->second].push_back(std::move(waiting));
                    return;
                }
                if (const auto gone = closed_.find(named); gone != closed_.end()) {
                    Order order{};
                    order.id = gone->second.id;
                    order.status = gone->second.status;
                    order.reference = named.second;
                    order.client = client;
                    order.account = account_;
                    respond(client, "9", [&](protocol::Writer& out) { to_fix_cancel_too_late(out, m, order, now); }, now);
                    return;
                }
                Order unknown{};
                unknown.reference = named.second;
                unknown.client = client;
                unknown.account = account_;
                respond(client, "9", [&](protocol::Writer& out) { to_fix_cancel_unknown(out, m, unknown, now); }, now);
                return;
            }
            if (orders_.at(found->second).replaced_id != 0) {
                Held waiting{ std::string(type), std::string(m), std::string{} };
                if (!waiting.introduces.empty()) awaiting_[{ client, waiting.introduces }] = found->second;
                held_[found->second].push_back(std::move(waiting));
                return;
            }
            if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(orders_.at(found->second).entered_symbol)], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
            Order& order = orders_.at(found->second);
            order.requested = std::string(protocol::field(m, Tag::ClOrdID));
            auto ipc = to_fix_cancel_order(m, order);
            detail_ipc::emit_framed(emit_, 'C', ipc);
            return;
        }

        if (type == "G") {   // OrderCancelReplaceRequest: fix_replace_order
            const auto named = std::make_pair(client, std::string(protocol::field(m, Tag::OrigClOrdID)));
            const auto found = references_.find(named);
            if (found == references_.end()) {
                if (const auto awaited = awaiting_.find(named); awaited != awaiting_.end()) {   // the reference a held replace will give
                    Held waiting{ std::string(type), std::string(m), std::string(protocol::field(m, Tag::ClOrdID)) };
                    if (!waiting.introduces.empty()) awaiting_[{ client, waiting.introduces }] = awaited->second;
                    held_[awaited->second].push_back(std::move(waiting));
                    return;
                }
                if (const auto gone = closed_.find(named); gone != closed_.end()) {
                    Order order{};
                    order.id = gone->second.id;
                    order.status = gone->second.status;
                    order.reference = named.second;
                    order.client = client;
                    order.account = account_;
                    respond(client, "9", [&](protocol::Writer& out) { to_fix_replace_too_late(out, m, order, now); }, now);
                    return;
                }
                Order unknown{};
                unknown.reference = named.second;
                unknown.client = client;
                unknown.account = account_;
                respond(client, "9", [&](protocol::Writer& out) { to_fix_replace_unknown(out, m, unknown, now); }, now);
                return;
            }
            if (orders_.at(found->second).replaced_id != 0) {
                Held waiting{ std::string(type), std::string(m), std::string(protocol::field(m, Tag::ClOrdID)) };
                if (!waiting.introduces.empty()) awaiting_[{ client, waiting.introduces }] = found->second;
                held_[found->second].push_back(std::move(waiting));
                return;
            }
            const Order& replaced = orders_.at(found->second);
            if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(std::string_view{replaced.entered_symbol})], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
            if (risk.stopped) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_stopped_replace(out, m, replaced, now); }, now); return; }   // a breach stopped its client for the day
            const auto& controls = clients_.at(client)->controls;   // the risk controls the venue declares of the client
            if (controls.share_size_control && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) > controls.share_size_control_shares) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_share_size_replace(out, m, replaced, now); }, now); return; }
            if (controls.maximum_single_order_notional && !(protocol::field(m, Tag::OrdType) == "1") && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) * static_cast<long double>(protocol::field_scaled(m, Tag::Price, 4)) > controls.maximum_single_order_notional_dollars * 10000) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_notional_replace(out, m, replaced, now); }, now); return; }
            if (controls.maximum_single_order_notional && protocol::field(m, Tag::OrdType) == "1" && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) * listing_luld_reference(std::string_view{replaced.entered_symbol}) > controls.maximum_single_order_notional_dollars * 10000) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_notional_unpriced_replace(out, m, replaced, now); }, now); return; }
            if (controls.order_type_check_pre_market && (heard_phase_announced_message_announcement_ == 'O' || heard_phase_announced_message_announcement_ == 'S') && (std::string_view{replaced.entered_time_in_force} == "1" || std::string_view{replaced.entered_time_in_force} == "3" || std::string_view{replaced.entered_time_in_force} == "5" || std::string_view{replaced.entered_time_in_force} == "6" || std::string_view{replaced.entered_time_in_force} == "A")) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_pre_market_replace(out, m, replaced, now); }, now); return; }
            if (controls.order_type_check_post_market && (heard_phase_announced_message_announcement_ == 'M' || heard_phase_announced_message_announcement_ == 'E') && (std::string_view{replaced.entered_time_in_force} == "1" || std::string_view{replaced.entered_time_in_force} == "3" || std::string_view{replaced.entered_time_in_force} == "5" || std::string_view{replaced.entered_time_in_force} == "6" || std::string_view{replaced.entered_time_in_force} == "A")) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_post_market_replace(out, m, replaced, now); }, now); return; }
            if (controls.order_type_check_post_market && std::string_view{replaced.entered_time_in_force} == "E") { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_after_hours_replace(out, m, replaced, now); }, now); return; }
            if (controls.market_impact_check && std::string_view{replaced.entered_side} == "1" && !(protocol::field(m, Tag::OrdType) == "1") && static_cast<long double>(protocol::field_scaled(m, Tag::Price, 4)) > listing_luld_upper(std::string_view{replaced.entered_symbol})) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_market_impact_buy_replace(out, m, replaced, now); }, now); return; }
            if (controls.market_impact_check && (std::string_view{replaced.entered_side} == "2" || std::string_view{replaced.entered_side} == "5" || std::string_view{replaced.entered_side} == "6") && !(protocol::field(m, Tag::OrdType) == "1") && static_cast<long double>(protocol::field_scaled(m, Tag::Price, 4)) < listing_luld_lower(std::string_view{replaced.entered_symbol})) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_market_impact_sell_replace(out, m, replaced, now); }, now); return; }
            if (controls.average_daily_volume_check && static_cast<long double>(protocol::field_scaled(m, Tag::OrderQty, 0)) > std::max(listing_average_daily_volume(std::string_view{replaced.entered_symbol}), controls.average_daily_volume_check_minimum_shares) * controls.average_daily_volume_check_percent / 100) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_average_daily_volume_replace(out, m, replaced, now); }, now); return; }
            if (controls.rate_thresholds_port && static_cast<long double>(risk.rate_thresholds_port.size()) > controls.rate_thresholds_port_messages) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_port_rate_replace(out, m, replaced, now); }, now); return; }
            if (controls.rate_thresholds_symbol && static_cast<long double>(risk.rate_thresholds_symbol[std::string(std::string_view{replaced.entered_symbol})].size()) > controls.rate_thresholds_symbol_messages) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_symbol_rate_replace(out, m, replaced, now); }, now); return; }
            std::string carried(m);   // the replace, with what it leaves out as the order was entered with it
            if (protocol::field(m, Tag::TimeInForce).empty() && !replaced.entered_time_in_force.empty()) carried.append(std::to_string(static_cast<std::uint32_t>(Tag::TimeInForce))).append("=").append(replaced.entered_time_in_force).append("\x01");
            if (protocol::field(m, Tag::DisplayInst).empty() && !replaced.entered_display_inst.empty()) carried.append(std::to_string(static_cast<std::uint32_t>(Tag::DisplayInst))).append("=").append(replaced.entered_display_inst).append("\x01");
            const auto asked = to_fix_replace_order(carried, replaced);   // what the engine would be told of it, which its client's exposure values
            const long double priced = (asked.price.get().has_value() ? static_cast<long double>(asked.price.get().value()) : listing_luld_reference(std::string_view{replaced.entered_symbol}));   // what its open shares are valued at by its client's exposure
            if (broken(risk, static_cast<long double>(asked.quantity.get().value()) * priced, static_cast<long double>(replaced.remaining) * replaced.valued)) { respond(client, "9", [&](protocol::Writer& out) { to_fix_risk_stopped_replace(out, m, replaced, now); }, now); return; }   // admitted, it would breach a limit of its client's
            Order order = replaced;
            if (!protocol::field(m, Tag::TimeInForce).empty()) order.entered_time_in_force = std::string(protocol::field(m, Tag::TimeInForce));
            if (!protocol::field(m, Tag::DisplayInst).empty()) order.entered_display_inst = std::string(protocol::field(m, Tag::DisplayInst));
            order.id = numbered();
            order.reference = std::string(protocol::field(m, Tag::ClOrdID));
            order.requested = order.reference;
            order.replaced_id = replaced.id;
            order.replaced_reference = replaced.reference;
            order.asked = carried;
            auto ipc = to_fix_replace_order(carried, order);
            if (clients_.at(client)->unextended) { ipc.extended_close.set(static_cast<std::remove_cvref_t<decltype(ipc.extended_close.get().value())>>('N')); }   // its port keeps its LOC Orders out of the Extended Trading Close
            protect(ipc, clients_.at(client)->protection_percent, clients_.at(client)->protection_amount);   // its port's limit on how far through the book's best opposite price an order is priced
            order.quantity = ipc.quantity.get().value() + (replaced.quantity - replaced.remaining);   // the whole, its executions included
            order.remaining = ipc.quantity.get().value();
            order.price = ipc.price.get();   // none for a market order
            order.display = static_cast<char>(ipc.display.get().value());
            order.valued = priced;
            replacing_[replaced.id] = order.id;
            orders_[order.id] = order;
            references_[{ client, order.reference }] = order.id;
            detail_ipc::emit_framed(emit_, 'M', ipc);
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
        if (found->second.remaining == 0) close(id, '2');
    }

    void close(std::uint64_t id, const char status = '4') {
        auto found = orders_.find(id);
        if (found == orders_.end()) return;
        closed_[{ found->second.client, found->second.reference }] = { id, status };
        references_.erase({ found->second.client, found->second.reference });
        orders_.erase(found);
    }

    // Is a value one of a list a client's risk control states?
    static bool listed_in(const std::vector<std::string_view>& list, std::string_view value) {
        return std::find(list.begin(), list.end(), value) != list.end();
    }

    // Each instrument's <Listing><LuldReference>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_luld_reference(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 10000; }
        if (symbol == "ZWZZT") { return 10000; }
        if (symbol == "ZXZZT") { return 10000; }
        return 0;
    }

    // Each instrument's <Listing><LuldUpper>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_luld_upper(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 10500; }
        if (symbol == "ZWZZT") { return 10500; }
        if (symbol == "ZXZZT") { return 10500; }
        return 0;
    }

    // Each instrument's <Listing><LuldLower>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_luld_lower(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 9500; }
        if (symbol == "ZWZZT") { return 9500; }
        if (symbol == "ZXZZT") { return 9500; }
        return 0;
    }

    // Each instrument's <Listing><AverageDailyVolume>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_average_daily_volume(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 200000; }
        if (symbol == "ZWZZT") { return 200000; }
        if (symbol == "ZXZZT") { return 0; }
        return 0;
    }

    // A client's message tallied in a rolling window: when each within it came, this one last, those older let
    // go. How many there are.
    static std::size_t tallied(std::deque<std::int64_t>& times, std::int64_t now, long double window) {
        times.push_back(now);
        while (!times.empty() && times.front() <= now - static_cast<std::int64_t>(window)) times.pop_front();
        return times.size();
    }

    // Is an order a duplicate of one its client entered within the window, told apart by the same? Those older
    // are let go.
    static bool duplicated(std::deque<std::pair<std::int64_t, std::string>>& entered, std::int64_t now, long double window, const std::string& key) {
        while (!entered.empty() && entered.front().first <= now - static_cast<std::int64_t>(window)) entered.pop_front();
        return std::any_of(entered.begin(), entered.end(), [&](const auto& each) { return each.second == key; });
    }

    // What a client's open orders leave open: each one's open shares at the price it is valued at.
    long double opened(const Risk& risk) const {
        long double open = 0;
        for (const auto& held : orders_) if (held.second.owner == risk.username) open += static_cast<long double>(held.second.remaining) * held.second.valued;
        return open;
    }

    // An execution of an order a client entered, counted in what its orders have executed: the client, or none.
    Risk* exposed(std::uint64_t id, std::uint64_t shares, long double price) {
        const auto found = orders_.find(id);
        if (found == orders_.end() || found->second.owner.empty()) return nullptr;
        const auto risk = risks_.find(found->second.owner);
        if (risk == risks_.end()) return nullptr;
        risk->second.executed += static_cast<long double>(shares) * price;
        return &risk->second;
    }

    // Is a limit of a client's broken, were `entering` more left open and `leaving` less: what its orders have
    // executed, or leave open, past what it declares? Broken, the client is stopped for the rest of the day and
    // what that breach cancels is canceled. A client stopped stays so.
    bool broken(Risk& risk, long double entering, long double leaving) {
        if (risk.stopped) return true;
        const long double executed = risk.executed;
        const long double open = opened(risk) + entering - leaving;
        if (!risk.stopped && risk.controls.kill_switch_gross_executed_exposure && executed > risk.controls.kill_switch_gross_executed_exposure_dollars * 10000) { risk.stopped = true; canceled_fix_kill_switch_executed(risk); }   // KillSwitch.GrossExecutedExposure
        if (!risk.stopped && risk.controls.kill_switch_gross_notional_exposure && (executed + open) > risk.controls.kill_switch_gross_notional_exposure_dollars * 10000) { risk.stopped = true; canceled_fix_kill_switch_notional(risk); }   // KillSwitch.GrossNotionalExposure
        if (!risk.stopped && risk.controls.gross_exposure_check && listed_in(risk.controls.gross_exposure_check_measure, "Open") && open > risk.controls.gross_exposure_check_dollars * 10000) { risk.stopped = true; canceled_fix_gross_exposure_open(risk); }   // GrossExposureCheck
        if (!risk.stopped && risk.controls.gross_exposure_check && listed_in(risk.controls.gross_exposure_check_measure, "Executed") && executed > risk.controls.gross_exposure_check_dollars * 10000) { risk.stopped = true; canceled_fix_gross_exposure_executed(risk); }   // GrossExposureCheck
        if (!risk.stopped && risk.controls.gross_exposure_check && listed_in(risk.controls.gross_exposure_check_measure, "Notional") && (executed + open) > risk.controls.gross_exposure_check_dollars * 10000) { risk.stopped = true; canceled_fix_gross_exposure_notional(risk); }   // GrossExposureCheck
        return risk.stopped;
    }

    // The client's open orders fix_kill_switch_executed cancels (KillSwitch.GrossExecutedExposure), each as fix_cancel_order cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_fix_kill_switch_executed(const Risk& risk) {
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.owner == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            std::string asked;   // the cancel its client would send of it, as it was entered: what fix_cancel_order reads
            asked.append("\x01").append(std::to_string(Tag::Symbol)).append("=").append(held->second.entered_symbol).append("\x01");
            auto ipc = to_fix_cancel_order(asked, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders fix_kill_switch_notional cancels (KillSwitch.GrossNotionalExposure), each as fix_cancel_order cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_fix_kill_switch_notional(const Risk& risk) {
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.owner == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            std::string asked;   // the cancel its client would send of it, as it was entered: what fix_cancel_order reads
            asked.append("\x01").append(std::to_string(Tag::Symbol)).append("=").append(held->second.entered_symbol).append("\x01");
            auto ipc = to_fix_cancel_order(asked, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders fix_gross_exposure_open cancels (GrossExposureCheck), each as fix_cancel_order cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_fix_gross_exposure_open(const Risk& risk) {
        if (!(listed_in(risk.controls.gross_exposure_check_cancel_open_orders, "Yes"))) return;   // the client's choice
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.owner == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            std::string asked;   // the cancel its client would send of it, as it was entered: what fix_cancel_order reads
            asked.append("\x01").append(std::to_string(Tag::Symbol)).append("=").append(held->second.entered_symbol).append("\x01");
            auto ipc = to_fix_cancel_order(asked, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders fix_gross_exposure_executed cancels (GrossExposureCheck), each as fix_cancel_order cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_fix_gross_exposure_executed(const Risk& risk) {
        if (!(listed_in(risk.controls.gross_exposure_check_cancel_open_orders, "Yes"))) return;   // the client's choice
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.owner == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            std::string asked;   // the cancel its client would send of it, as it was entered: what fix_cancel_order reads
            asked.append("\x01").append(std::to_string(Tag::Symbol)).append("=").append(held->second.entered_symbol).append("\x01");
            auto ipc = to_fix_cancel_order(asked, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders fix_gross_exposure_notional cancels (GrossExposureCheck), each as fix_cancel_order cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_fix_gross_exposure_notional(const Risk& risk) {
        if (!(listed_in(risk.controls.gross_exposure_check_cancel_open_orders, "Yes"))) return;   // the client's choice
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.owner == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            std::string asked;   // the cancel its client would send of it, as it was entered: what fix_cancel_order reads
            asked.append("\x01").append(std::to_string(Tag::Symbol)).append("=").append(held->second.entered_symbol).append("\x01");
            auto ipc = to_fix_cancel_order(asked, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders fix_cancel_on_disconnect cancels (CancelOnDisconnect), each as fix_cancel_order cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_fix_cancel_on_disconnect(const Risk& risk) {
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.owner == risk.username && !(held.second.entered_time_in_force == "2" || held.second.entered_time_in_force == "7")) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            std::string asked;   // the cancel its client would send of it, as it was entered: what fix_cancel_order reads
            asked.append("\x01").append(std::to_string(Tag::Symbol)).append("=").append(held->second.entered_symbol).append("\x01");
            auto ipc = to_fix_cancel_order(asked, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // A client's session ended other than by its logout: what each breach of its made so does.
    void disconnected(const Risk& risk) {
        if (risk.controls.cancel_on_disconnect) canceled_fix_cancel_on_disconnect(risk);   // CancelOnDisconnect
    }

    Emit emit_;
    Clock clock_;
    protocol::AcceptorConfig config_{};
    std::map<std::size_t, std::unique_ptr<Client>> clients_;
    std::size_t next_client_ = 0;   // session numbers, never reused
    std::map<std::size_t, Connection> connections_;
    std::size_t next_connection_ = 0;   // connection numbers, never reused
    std::map<std::string, std::size_t, std::less<>> comp_ids_;   // each comp id's session, by its number
    std::map<std::string, std::string, std::less<>> credentials_;
    std::int64_t heard_phase_announced_message_announcement_ = 0;   // the latest announcement of the engine's phase_announced_message, which a client risk control reads: 0 before it hears one
    std::map<std::string, Risk, std::less<>> risks_;   // what the gateway holds of each client for its risk controls, by the name it logs on with: kept the day
    std::size_t account_;   // the Account its port is bound to: the gateway's place among the venue's gateways
    std::map<std::uint64_t, Order> orders_;
    std::map<std::pair<std::size_t, std::string>, std::uint64_t, std::less<>> references_;
    struct Closed { std::uint64_t id = 0; char status = 0; };   // an order that has left the book, and how it ended
    std::map<std::pair<std::size_t, std::string>, Closed, std::less<>> closed_;   // the day's orders that have left the book, which a late request is refused naming
    std::map<std::uint64_t, std::uint64_t> replacing_;   // an order a replace is asked of, and the number the gateway gave its replacement
    std::map<std::uint64_t, std::vector<Held>> held_;   // requests on a replacement the engine has not answered, by its number
    std::map<std::pair<std::size_t, std::string>, std::uint64_t, std::less<>> awaiting_;   // the references held replaces will give, and the queue each waits in
    std::uint64_t next_id_;   // the number the next order admitted is given
    std::uint64_t step_;      // how many gateways number in turn

    // The number of the next order admitted, and the gateway's next after it.
    std::uint64_t numbered() {
        const std::uint64_t id = next_id_;
        next_id_ += step_;
        return id;
    }

    // The number of the next report sent, in the same interleave as the orders' numbers, so no two
    // gateways send one ExecID: IEX's is "Unique" (IEX FIX Specification v2.91, p. 14).
    std::uint64_t next_report_ = 0;
    std::uint64_t reported() {
        const std::uint64_t id = next_report_;
        next_report_ += step_;
        return id;
    }
};

}  // namespace sim::orderentry::fix
