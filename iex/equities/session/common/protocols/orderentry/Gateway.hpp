#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <set>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "FixAcceptor.hpp"
#include "../ipc/IpcBus.hpp"
#include "../ipc/structs/NewOrderMessage.hpp"
#include "../ipc/structs/CancelOrderMessage.hpp"
#include "../ipc/structs/ReplaceOrderMessage.hpp"
#include "../ipc/structs/TradeMessage.hpp"
#include "../ipc/structs/OrderCanceledMessage.hpp"
#include "../ipc/structs/OrderEliminatedMessage.hpp"
#include "../ipc/structs/OrderReplacedMessage.hpp"
#include "../ipc/structs/CancelRefusedMessage.hpp"

namespace sim::orderentry {

namespace protocol = ::iexiexequities::order_entry;
namespace ipc_ns = ::iex::iexequities::matchingengine::ipc::v2026;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// The tags the gateway reads and writes, as Iex.IexEquities.OrderEntry.Fix.v2.91 numbers them.
struct Tag {
    static constexpr std::uint32_t Account = 1;
    static constexpr std::uint32_t AntiInternalizationQualifiersAIQID = 7928;
    static constexpr std::uint32_t AvgPrice = 6;
    static constexpr std::uint32_t ClOrdID = 11;
    static constexpr std::uint32_t ContraBroker = 375;
    static constexpr std::uint32_t CumQty = 14;
    static constexpr std::uint32_t CxlRejReason = 102;
    static constexpr std::uint32_t CxlRejResponseTo = 434;
    static constexpr std::uint32_t ExecID = 17;
    static constexpr std::uint32_t ExecInst = 18;
    static constexpr std::uint32_t ExecTransType = 20;
    static constexpr std::uint32_t ExecType = 150;
    static constexpr std::uint32_t ExpireTime = 126;
    static constexpr std::uint32_t LastLiquidityInd = 851;
    static constexpr std::uint32_t LastMkt = 30;
    static constexpr std::uint32_t LastPx = 31;
    static constexpr std::uint32_t LastShares = 32;
    static constexpr std::uint32_t LeavesQty = 151;
    static constexpr std::uint32_t LocateReqd = 114;
    static constexpr std::uint32_t MaxFloor = 111;
    static constexpr std::uint32_t MinQty = 110;
    static constexpr std::uint32_t NoContraBrokers = 382;
    static constexpr std::uint32_t OrdRejReason = 103;
    static constexpr std::uint32_t OrdStatus = 39;
    static constexpr std::uint32_t OrdType = 40;
    static constexpr std::uint32_t OrderCapacity = 47;
    static constexpr std::uint32_t OrderID = 37;
    static constexpr std::uint32_t OrderQty = 38;
    static constexpr std::uint32_t OrigClOrdID = 41;
    static constexpr std::uint32_t PegDifference = 211;
    static constexpr std::uint32_t Price = 44;
    static constexpr std::uint32_t SecondaryOrderID = 198;
    static constexpr std::uint32_t SenderCompID = 49;
    static constexpr std::uint32_t Side = 54;
    static constexpr std::uint32_t Symbol = 55;
    static constexpr std::uint32_t Text = 58;
    static constexpr std::uint32_t TimeInForce = 59;
    static constexpr std::uint32_t TradeLiquidityIndicator = 9730;
    static constexpr std::uint32_t TransactTime = 60;
};

// A UTCTimestamp, YYYYMMDD-HH:MM:SS with up to nine places of a second, as nanoseconds since the

// epoch: the days from the civil date by Howard Hinnant's days-from-civil algorithm. 0 where the

// request carries none, or text that is no timestamp.

inline std::int64_t utc_nanos(std::string_view text) {

    if (text.size() < 17 || text[8] != '-' || text[11] != ':' || text[14] != ':') return 0;

    constexpr std::size_t at[6] = { 0, 4, 6, 9, 12, 15 };

    constexpr std::size_t width[6] = { 4, 2, 2, 2, 2, 2 };

    std::int64_t part[6] = {};

    for (std::size_t which = 0; which < 6; ++which) {

        for (std::size_t digit = 0; digit < width[which]; ++digit) {

            const char c = text[at[which] + digit];

            if (c < '0' || c > '9') return 0;

            part[which] = part[which] * 10 + (c - '0');

        }

    }

    const auto [year, month, day, hour, minute, second] = part;

    if (month < 1 || month > 12 || day < 1 || day > 31 || hour > 23 || minute > 59 || second > 60) return 0;



    std::int64_t fraction = 0;

    std::size_t places = 0;

    if (text.size() > 17) {

        if (text[17] != '.' || text.size() == 18 || text.size() > 27) return 0;

        for (std::size_t digit = 18; digit < text.size(); ++digit, ++places) {

            if (text[digit] < '0' || text[digit] > '9') return 0;

            fraction = fraction * 10 + (text[digit] - '0');

        }

    }

    for (; places < 9; ++places) fraction *= 10;



    const std::int64_t shifted = month <= 2 ? year - 1 : year;

    const std::int64_t era = (shifted >= 0 ? shifted : shifted - 399) / 400;

    const std::int64_t of_era = shifted - era * 400;

    const std::int64_t of_year = (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;

    const std::int64_t days = era * 146'097 + of_era * 365 + of_era / 4 - of_era / 100 + of_year - 719'468;

    return ((days * 86'400 + hour * 3'600 + minute * 60 + second) * 1'000'000'000) + fraction;

}


// An order the gateway admitted: its id, the client's reference to it, the shares still open,
// the comp id's session that entered it, which its reports go to, and the Account that owns it -
// its port's, whichever comp id entered it.
// It also keeps what the client entered it with that its reports name: ExecInst, MinQty, MaxFloor, OrdType, OrderCapacity, Side, Symbol, Account, ExpireTime, AntiInternalizationQualifiersAIQID, TimeInForce.
struct Order {
    std::uint64_t id = 0;
    std::string reference;
    std::uint64_t remaining = 0;
    std::size_t client = 0;
    std::size_t account = 0;   // the Account that owns it: its port's
    std::string entered_exec_inst;
    std::string entered_min_qty;
    std::string entered_max_floor;
    std::string entered_ord_type;
    std::string entered_order_capacity;
    std::string entered_side;
    std::string entered_symbol;
    std::string entered_account;
    std::string entered_expire_time;
    std::string entered_anti_internalization_qualifiers_aiqid;
    std::string entered_time_in_force;
    std::uint64_t quantity = 0;   // what it was admitted with, which its reports count executions against
    std::string requested;   // the ClOrdID the latest request naming it was sent under: its own until another names it
    std::uint64_t replaced_id = 0;   // the order it replaces, until the engine has replaced it
    std::string replaced_reference;   // and that order's reference, which its report names
    std::string asked;   // the replace's own text, until the engine has answered it: what a refusal of it answers
    std::string request;   // the request that admitted it, until the engine has taken or refused it: what that answer is written from
    char time_in_force = 0;   // how long the engine was told it lives, its own code
    char display = 0;   // whether the engine was told to show it, its own code
    std::uint64_t executed_value = 0;   // each execution's price times its shares, summed: what its average price is read from
    char status = 0;   // FIX's OrdStatus once it has left the book - 2 filled, 4 canceled, 8 rejected - and 0 while it is open
};

// The quantity a replace leaves open. FIX's OrderQty is the order's total, executions included,
// which its replacement carries on; a replace may raise it, so what it leaves open is that less
// what the order chain has executed.
inline std::uint64_t liable(std::uint64_t size, const Order& order) {
    const std::uint64_t executed = order.quantity - order.remaining;
    return size > executed ? size - executed : 0;
}

// NewOrderSingle -> new_order_message  (IexEquities.Ipc.Transforms.xml: fix_new_order)
inline ipc_ns::new_order_message to_fix_new_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::new_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.symbol.set(protocol::field(m, Tag::Symbol));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case '1': return 'B'; case '2': return 'S'; case '5': return 'S'; case '6': return 'S'; default: return value; } }(protocol::field_char(m, Tag::Side))));
    out.price.set(static_cast<value_of<decltype(out.price)>>(protocol::field_scaled(m, Tag::Price, 4)));
    out.quantity.set(static_cast<value_of<decltype(out.quantity)>>(protocol::field_scaled(m, Tag::OrderQty, 0)));
    out.order_type.set(static_cast<value_of<decltype(out.order_type)>>(protocol::field_char(m, Tag::OrdType)));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>([](char value) { switch (value) { case '\0': return '0'; default: return value; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.display.set(static_cast<value_of<decltype(out.display)>>([](char value) { switch (value) { case 'P': return 'N'; default: return 'Y'; } }(protocol::field_char(m, Tag::OrdType))));
    if (!protocol::field(m, Tag::MaxFloor).empty()) out.display.set(static_cast<value_of<decltype(out.display)>>([](char value) { switch (value) { case '0': return 'N'; default: return 'Y'; } }([](std::string_view text) { return text.size() == 1 ? text.front() : text.empty() ? '\0' : '\x01'; }(protocol::field(m, Tag::MaxFloor)))));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>([](char value) { switch (value) { case 'M': return 'M'; case 'R': return 'R'; case 'd': return 'd'; case 'z': return 'z'; case 'Q': return 'Q'; case 'o': return 'o'; case 'P': return 'P'; case '\0': return 'N'; default: return 'N'; } }([](std::string_view text) { return text.size() == 1 ? text.front() : text.empty() ? '\0' : '\x01'; }(protocol::field(m, Tag::ExecInst)))));
    out.peg_difference.set(static_cast<value_of<decltype(out.peg_difference)>>(protocol::field_scaled(m, Tag::PegDifference, 4)));
    out.min_qty.set(static_cast<value_of<decltype(out.min_qty)>>(protocol::field_scaled(m, Tag::MinQty, 0)));
    out.self_match_modifier.set(static_cast<value_of<decltype(out.self_match_modifier)>>([](char value) { switch (value) { case 'D': return 'S'; case '\0': return 'O'; default: return value; } }([](std::string_view text) { return text.size() >= 3 ? text[2] : '\0'; }(protocol::field(m, Tag::AntiInternalizationQualifiersAIQID)))));
    out.instruction.set(static_cast<value_of<decltype(out.instruction)>>([](char value) { switch (value) { case 'i': return 'i'; case 'u': return 'u'; case 'r': return 'u'; case 's': return 'u'; case 't': return 'u'; case 'f': return 'f'; case 'y': return 'y'; case 'j': return 'j'; case 'M': return 'i'; case 'R': return 'i'; case 'd': return 'i'; case 'z': return 'i'; case 'Q': return 'i'; case 'o': return 'i'; case 'P': return 'i'; case '\0': return 'i'; default: return '?'; } }([](std::string_view text) { return text.size() == 1 ? text.front() : text.empty() ? '\0' : '\x01'; }(protocol::field(m, Tag::ExecInst)))));
    out.cross_type.set(static_cast<value_of<decltype(out.cross_type)>>([](char value) { switch (value) { case '2': return 'O'; case '7': return 'C'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.expire_time.set(static_cast<value_of<decltype(out.expire_time)>>(utc_nanos(protocol::field(m, Tag::ExpireTime))));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>([&]() -> std::uint64_t { std::uint64_t key = 14695981039346656037ull; const auto mix = [&key](std::string_view text) { for (const char c : text) { key ^= static_cast<unsigned char>(c); key *= 1099511628211ull; } key ^= 0x1f; key *= 1099511628211ull; }; mix(std::to_string(order.account)); { const auto held = protocol::field(m, Tag::AntiInternalizationQualifiersAIQID); std::string_view text{held}; if (text.size() >= 1) text = text.substr(0, 2); else text = {}; if (text.empty() || text == "--") return 0; mix(text); } return key == 0 ? 1 : key; }()));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// OrderCancelRequest -> cancel_order_message  (IexEquities.Ipc.Transforms.xml: fix_cancel_order)
inline ipc_ns::cancel_order_message to_fix_cancel_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// OrderCancelReplaceRequest -> replace_order_message  (IexEquities.Ipc.Transforms.xml: fix_replace_order)
inline ipc_ns::replace_order_message to_fix_replace_order(std::string_view m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::replace_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.replaced_id));
    out.replacement_id.set(static_cast<value_of<decltype(out.replacement_id)>>(order.id));
    out.display.set(static_cast<value_of<decltype(out.display)>>(order.display));
    out.symbol.set(protocol::field(m, Tag::Symbol));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case '1': return 'B'; case '2': return 'S'; case '5': return 'S'; case '6': return 'S'; default: return value; } }(protocol::field_char(m, Tag::Side))));
    out.price.set(static_cast<value_of<decltype(out.price)>>(protocol::field_scaled(m, Tag::Price, 4)));
    out.quantity.set(static_cast<value_of<decltype(out.quantity)>>(liable(static_cast<std::uint64_t>(static_cast<value_of<decltype(out.quantity)>>(protocol::field_scaled(m, Tag::OrderQty, 0))), order)));
    out.order_type.set(static_cast<value_of<decltype(out.order_type)>>(protocol::field_char(m, Tag::OrdType)));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>([](char value) { switch (value) { case '\0': return '0'; default: return value; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>([](char value) { switch (value) { case 'M': return 'M'; case 'R': return 'R'; case 'd': return 'd'; case 'z': return 'z'; case 'Q': return 'Q'; case 'o': return 'o'; case 'P': return 'P'; case '\0': return 'N'; default: return 'N'; } }([](std::string_view text) { return text.size() == 1 ? text.front() : text.empty() ? '\0' : '\x01'; }(protocol::field(m, Tag::ExecInst)))));
    out.peg_difference.set(static_cast<value_of<decltype(out.peg_difference)>>(protocol::field_scaled(m, Tag::PegDifference, 4)));
    out.min_qty.set(static_cast<value_of<decltype(out.min_qty)>>(protocol::field_scaled(m, Tag::MinQty, 0)));
    out.self_match_modifier.set(static_cast<value_of<decltype(out.self_match_modifier)>>([](char value) { switch (value) { case 'D': return 'S'; case '\0': return 'O'; default: return value; } }([](std::string_view text) { return text.size() >= 3 ? text[2] : '\0'; }(std::string_view{order.entered_anti_internalization_qualifiers_aiqid}))));
    out.instruction.set(static_cast<value_of<decltype(out.instruction)>>([](char value) { switch (value) { case 'i': return 'i'; case 'u': return 'u'; case 'r': return 'u'; case 's': return 'u'; case 't': return 'u'; case 'f': return 'f'; case 'y': return 'y'; case 'j': return 'j'; case 'M': return 'i'; case 'R': return 'i'; case 'd': return 'i'; case 'z': return 'i'; case 'Q': return 'i'; case 'o': return 'i'; case 'P': return 'i'; case '\0': return 'i'; default: return '?'; } }([](std::string_view text) { return text.size() == 1 ? text.front() : text.empty() ? '\0' : '\x01'; }(protocol::field(m, Tag::ExecInst)))));
    out.cross_type.set(static_cast<value_of<decltype(out.cross_type)>>([](char value) { switch (value) { case '2': return 'O'; case '7': return 'C'; case '\0': return 'N'; default: return 'N'; } }(protocol::field_char(m, Tag::TimeInForce))));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>([&]() -> std::uint64_t { std::uint64_t key = 14695981039346656037ull; const auto mix = [&key](std::string_view text) { for (const char c : text) { key ^= static_cast<unsigned char>(c); key *= 1099511628211ull; } key ^= 0x1f; key *= 1099511628211ull; }; mix(std::to_string(order.account)); { const auto held = std::string_view{order.entered_anti_internalization_qualifiers_aiqid}; std::string_view text{held}; if (text.size() >= 1) text = text.substr(0, 2); else text = {}; if (text.empty() || text == "--") return 0; mix(text); } return key == 0 ? 1 : key; }()));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_order_new)
inline void to_fix_order_new(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"0"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!protocol::field(m, Tag::Price).empty()) { out.put(Tag::Price, protocol::field(m, Tag::Price)); }
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"0"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_clordid)
inline void to_fix_invalid_clordid(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Duplicate ClOrdID"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_quantity)
inline void to_fix_invalid_quantity(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"3"});
    out.put(Tag::Text, std::string_view{"Order exceeds limit"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_side)
inline void to_fix_invalid_side(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Invalid side"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_peg_difference)
inline void to_fix_invalid_peg_difference(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"PegDifference out of range"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_max_floor)
inline void to_fix_invalid_max_floor(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Reserve orders are not supported"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_routed)
inline void to_fix_invalid_min_qty_routed(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on a routable order"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_sweep)
inline void to_fix_invalid_min_qty_sweep(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on an ISO"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_trade_at)
inline void to_fix_invalid_min_qty_trade_at(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on an ISO"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_d_limit)
inline void to_fix_invalid_min_qty_d_limit(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on a D-Limit order"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_fok)
inline void to_fix_invalid_min_qty_fok(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on a FOK order"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_on_open)
inline void to_fix_invalid_min_qty_on_open(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on an on-open order"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_on_close)
inline void to_fix_invalid_min_qty_on_close(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on an on-close order"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_min_qty_limit)
inline void to_fix_invalid_min_qty_limit(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"MinQty not valid on a displayed order"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_symbol)
inline void to_fix_invalid_symbol(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"1"});
    out.put(Tag::Text, std::string_view{"Unknown symbol"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_price)
inline void to_fix_invalid_price(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Limit order without a price"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_locate_short)
inline void to_fix_invalid_locate_short(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Short sale requires LocateReqd N"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_invalid_locate_exempt)
inline void to_fix_invalid_locate_exempt(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Short sale requires LocateReqd N"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_rejected_p)
inline void to_fix_rejected_p(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"Exchange closed"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_rejected_h)
inline void to_fix_rejected_h(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"Exchange closed"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_rejected_t)
inline void to_fix_rejected_t(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Order type not accepted"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_rejected_l)
inline void to_fix_rejected_l(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Time in force not accepted"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_rejected_c)
inline void to_fix_rejected_c(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Unsupported auction instruction"});
}

// NewOrderSingle -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_rejected_q)
inline void to_fix_rejected_q(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, now_ns);
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"8"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrdRejReason, std::string_view{"0"});
    out.put(Tag::Text, std::string_view{"Invalid quantity"});
}

// trade_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_filled_resting)
inline void to_fix_filled_resting(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::LastMkt, std::string_view{"IEXG"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"2"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"2"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::TradeLiquidityIndicator, [](unsigned first, unsigned second) -> std::string_view { switch (first << 8 | second) { case ('N' << 8 | 'Y'): return "ML"; case ('N' << 8 | 'N'): return "MI"; case ('O' << 8 | 'Y'): return "O"; case ('O' << 8 | 'N'): return "O"; case ('I' << 8 | 'Y'): return "H"; case ('I' << 8 | 'N'): return "H"; case ('C' << 8 | 'Y'): return "C"; case ('C' << 8 | 'N'): return "C"; default: return "ML"; } }(static_cast<unsigned char>(m.auction.get().value()), static_cast<unsigned char>(m.resting_display.get().value())));
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
    out.put(Tag::ContraBroker, std::string_view{"IEXG"});
    out.put(Tag::NoContraBrokers, std::string_view{"1"});
    out.put(Tag::LastLiquidityInd, [](char value) { switch (value) { case 'N': return '1'; default: return '4'; } }(static_cast<char>(m.auction.get().value())));
}

// trade_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_partially_filled_resting)
inline void to_fix_partially_filled_resting(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::LastMkt, std::string_view{"IEXG"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"1"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"1"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::TradeLiquidityIndicator, [](unsigned first, unsigned second) -> std::string_view { switch (first << 8 | second) { case ('N' << 8 | 'Y'): return "ML"; case ('N' << 8 | 'N'): return "MI"; case ('O' << 8 | 'Y'): return "O"; case ('O' << 8 | 'N'): return "O"; case ('I' << 8 | 'Y'): return "H"; case ('I' << 8 | 'N'): return "H"; case ('C' << 8 | 'Y'): return "C"; case ('C' << 8 | 'N'): return "C"; default: return "ML"; } }(static_cast<unsigned char>(m.auction.get().value()), static_cast<unsigned char>(m.resting_display.get().value())));
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
    out.put(Tag::ContraBroker, std::string_view{"IEXG"});
    out.put(Tag::NoContraBrokers, std::string_view{"1"});
    out.put(Tag::LastLiquidityInd, [](char value) { switch (value) { case 'N': return '1'; default: return '4'; } }(static_cast<char>(m.auction.get().value())));
}

// trade_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_filled_aggressor)
inline void to_fix_filled_aggressor(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::LastMkt, std::string_view{"IEXG"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"2"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"2"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::TradeLiquidityIndicator, [](unsigned first, unsigned second) -> std::string_view { switch (first << 8 | second) { case ('N' << 8 | 'Y'): return "TL"; case ('N' << 8 | 'N'): return "TI"; case ('O' << 8 | 'Y'): return "O"; case ('O' << 8 | 'N'): return "O"; case ('I' << 8 | 'Y'): return "H"; case ('I' << 8 | 'N'): return "H"; case ('C' << 8 | 'Y'): return "C"; case ('C' << 8 | 'N'): return "C"; default: return "TL"; } }(static_cast<unsigned char>(m.auction.get().value()), static_cast<unsigned char>(m.resting_display.get().value())));
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
    out.put(Tag::ContraBroker, std::string_view{"IEXG"});
    out.put(Tag::NoContraBrokers, std::string_view{"1"});
    out.put(Tag::LastLiquidityInd, [](char value) { switch (value) { case 'N': return '2'; default: return '4'; } }(static_cast<char>(m.auction.get().value())));
}

// trade_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_partially_filled_aggressor)
inline void to_fix_partially_filled_aggressor(protocol::Writer& out, const ipc_ns::trade_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::LastMkt, std::string_view{"IEXG"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"1"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"1"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::TradeLiquidityIndicator, [](unsigned first, unsigned second) -> std::string_view { switch (first << 8 | second) { case ('N' << 8 | 'Y'): return "TL"; case ('N' << 8 | 'N'): return "TI"; case ('O' << 8 | 'Y'): return "O"; case ('O' << 8 | 'N'): return "O"; case ('I' << 8 | 'Y'): return "H"; case ('I' << 8 | 'N'): return "H"; case ('C' << 8 | 'Y'): return "C"; case ('C' << 8 | 'N'): return "C"; default: return "TL"; } }(static_cast<unsigned char>(m.auction.get().value()), static_cast<unsigned char>(m.resting_display.get().value())));
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.price.get().value()), 4);
    out.put(Tag::ContraBroker, std::string_view{"IEXG"});
    out.put(Tag::NoContraBrokers, std::string_view{"1"});
    out.put(Tag::LastLiquidityInd, [](char value) { switch (value) { case 'N': return '2'; default: return '4'; } }(static_cast<char>(m.auction.get().value())));
}

// order_canceled_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_aiq_canceled)
inline void to_fix_aiq_canceled(protocol::Writer& out, const ipc_ns::order_canceled_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::SecondaryOrderID, static_cast<std::int64_t>(m.prevented_against.get().value()));
    if (!order.entered_anti_internalization_qualifiers_aiqid.empty()) { out.put(Tag::AntiInternalizationQualifiersAIQID, std::string_view{order.entered_anti_internalization_qualifiers_aiqid}); }
    out.put(Tag::TradeLiquidityIndicator, [](unsigned first, unsigned second) -> std::string_view { switch (first << 8 | second) { case ('A' << 8 | 'Y'): return "ML"; case ('A' << 8 | 'N'): return "MI"; case ('R' << 8 | 'Y'): return "TL"; case ('R' << 8 | 'N'): return "TI"; default: return "ML"; } }(static_cast<unsigned char>(m.prevented_liquidity.get().value()), static_cast<unsigned char>(m.prevented_display.get().value())));
    out.put(Tag::LastShares, static_cast<std::int64_t>(m.prevented_quantity.get().value()));
    out.put_scaled(Tag::LastPx, static_cast<std::int64_t>(m.prevented_price.get().value()), 4);
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
}

// order_canceled_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_canceled)
inline void to_fix_canceled(protocol::Writer& out, const ipc_ns::order_canceled_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
}

// order_eliminated_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_eliminated)
inline void to_fix_eliminated(protocol::Writer& out, const ipc_ns::order_eliminated_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.reference});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"4"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"4"});
    out.put(Tag::LeavesQty, std::string_view{"0"});
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
}

// order_replaced_message -> ExecutionReport  (IexEquities.Ipc.Transforms.xml: fix_replaced)
inline void to_fix_replaced(protocol::Writer& out, const ipc_ns::order_replaced_message& m, const Order& order, const std::int64_t now_ns, const std::uint64_t report) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put_scaled(Tag::AvgPrice, order.quantity == order.remaining ? std::int64_t{0} : static_cast<std::int64_t>(order.executed_value * 10000 / (order.quantity - order.remaining)), 4 + 4);
    out.put(Tag::ClOrdID, std::string_view{order.requested});
    out.put(Tag::CumQty, static_cast<std::int64_t>(order.quantity - order.remaining));
    out.put(Tag::ExecID, static_cast<std::int64_t>(report));
    if (!order.entered_exec_inst.empty()) { out.put(Tag::ExecInst, std::string_view{order.entered_exec_inst}); }
    out.put(Tag::ExecTransType, std::string_view{"0"});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrderQty, static_cast<std::int64_t>(order.quantity));
    out.put(Tag::OrdStatus, std::string_view{"5"});
    out.put(Tag::OrdType, std::string_view{order.entered_ord_type});
    out.put_scaled(Tag::Price, static_cast<std::int64_t>(m.price.get().value()), 4);
    if (!order.entered_order_capacity.empty()) { out.put(Tag::OrderCapacity, std::string_view{order.entered_order_capacity}); }
    out.put(Tag::Side, std::string_view{order.entered_side});
    out.put(Tag::Symbol, std::string_view{order.entered_symbol});
    out.put(Tag::TimeInForce, order.time_in_force);
    out.put_time(Tag::TransactTime, static_cast<std::int64_t>(m.timestamp.get().value()));
    if (!order.entered_expire_time.empty()) { out.put(Tag::ExpireTime, std::string_view{order.entered_expire_time}); }
    out.put(Tag::ExecType, std::string_view{"5"});
    out.put(Tag::LeavesQty, static_cast<std::int64_t>(order.remaining));
    if (!order.entered_min_qty.empty()) { out.put(Tag::MinQty, std::string_view{order.entered_min_qty}); }
    if (!order.entered_max_floor.empty()) { out.put(Tag::MaxFloor, std::string_view{order.entered_max_floor}); }
    if (!order.entered_account.empty()) { out.put(Tag::Account, std::string_view{order.entered_account}); }
    out.put(Tag::LastShares, std::string_view{"0"});
    out.put(Tag::LastPx, std::string_view{"0"});
    out.put(Tag::OrigClOrdID, std::string_view{order.replaced_reference});
}

// OrderCancelRequest -> OrderCancelReject  (IexEquities.Ipc.Transforms.xml: fix_cancel_unknown)
inline void to_fix_cancel_unknown(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrderID, std::string_view{"NONE"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::CxlRejReason, std::string_view{"1"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"1"});
    out.put(Tag::Text, std::string_view{"Unknown order"});
}

// OrderCancelRequest -> OrderCancelReject  (IexEquities.Ipc.Transforms.xml: fix_cancel_too_late)
inline void to_fix_cancel_too_late(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::CxlRejReason, std::string_view{"0"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"1"});
    out.put(Tag::Text, std::string_view{"Too late to cancel"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (IexEquities.Ipc.Transforms.xml: fix_replace_unknown)
inline void to_fix_replace_unknown(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrderID, std::string_view{"NONE"});
    out.put(Tag::OrdStatus, std::string_view{"8"});
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::CxlRejReason, std::string_view{"1"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"Unknown order"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (IexEquities.Ipc.Transforms.xml: fix_replace_too_late)
inline void to_fix_replace_too_late(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::OrigClOrdID, protocol::field(m, Tag::OrigClOrdID));
    out.put(Tag::CxlRejReason, std::string_view{"0"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"Too late to cancel"});
}

// OrderCancelReplaceRequest -> OrderCancelReject  (IexEquities.Ipc.Transforms.xml: fix_replace_unmatched)
inline void to_fix_replace_unmatched(protocol::Writer& out, std::string_view m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::ClOrdID, protocol::field(m, Tag::ClOrdID));
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
    out.put(Tag::CxlRejReason, std::string_view{"2"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"2"});
    out.put(Tag::Text, std::string_view{"Must match original order"});
}

// cancel_refused_message -> OrderCancelReject  (IexEquities.Ipc.Transforms.xml: fix_cancel_refused)
inline void to_fix_cancel_refused(protocol::Writer& out, const ipc_ns::cancel_refused_message& m, const Order& order, const std::int64_t now_ns) {
    (void)m;
    (void)order;
    (void)now_ns;
    out.put(Tag::ClOrdID, std::string_view{order.requested});
    out.put(Tag::OrderID, static_cast<std::int64_t>(order.id));
    out.put(Tag::OrdStatus, order.status != 0 ? order.status : order.remaining == 0 ? '2' : order.remaining < order.quantity ? '1' : '0');
    out.put(Tag::OrigClOrdID, std::string_view{order.reference});
    out.put(Tag::CxlRejReason, std::string_view{"0"});
    out.put(Tag::CxlRejResponseTo, std::string_view{"1"});
    out.put(Tag::Text, std::string_view{"Too late to cancel"});
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
        config_.comp_id = "IEXG";
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
        fill(m.resting_order_id.get().value(), m.quantity.get().value());
        fill(m.aggressor_order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_canceled_message: answered to the session that entered each order it names, as the event leaves it.
    void on_event(const ipc_ns::order_canceled_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            if (static_cast<char>(m.prevented.get().value()) == 'Y') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_aiq_canceled(out, m, answered, now, reported()); }, now);
            if (static_cast<char>(m.prevented.get().value()) == 'N') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_canceled(out, m, answered, now, reported()); }, now);
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

    // The engine's cancel_refused_message: answered to the session that entered each order it names, as the event leaves it.
    void on_event(const ipc_ns::cancel_refused_message& m) {
        const auto now = clock_();

        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) {
            Order answered = found->second;
            respond(answered.client, "9", [&](protocol::Writer& out) { to_fix_cancel_refused(out, m, answered, now); }, now);
        }
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
        if (static_cast<char>(m.refusal_reason.get().value()) == 'P') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_rejected_p(out, answered.request, answered, now, reported()); }, now);
        if (static_cast<char>(m.refusal_reason.get().value()) == 'H') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_rejected_h(out, answered.request, answered, now, reported()); }, now);
        if (static_cast<char>(m.refusal_reason.get().value()) == 'T') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_rejected_t(out, answered.request, answered, now, reported()); }, now);
        if (static_cast<char>(m.refusal_reason.get().value()) == 'L') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_rejected_l(out, answered.request, answered, now, reported()); }, now);
        if (static_cast<char>(m.refusal_reason.get().value()) == 'C') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_rejected_c(out, answered.request, answered, now, reported()); }, now);
        if (static_cast<char>(m.refusal_reason.get().value()) == 'Q') respond(answered.client, "8", [&](protocol::Writer& out) { to_fix_rejected_q(out, answered.request, answered, now, reported()); }, now);
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

        Client(Gateway& owner, std::size_t at)
            : gateway(owner), index(at), send([this](const char* data, std::size_t length) { gateway.write(connection, data, length); }),
              session(owner.config_, send, *this) {}

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
            const auto rejected = [&](auto&& report, std::string_view kind) {
                Order order{};
                order.id = numbered();
                order.reference = std::string(protocol::field(m, Tag::ClOrdID));
                order.client = client;
                order.account = account_;
                order.requested = order.reference;
                order.entered_exec_inst = std::string(protocol::field(m, Tag::ExecInst));
                order.entered_min_qty = std::string(protocol::field(m, Tag::MinQty));
                order.entered_max_floor = std::string(protocol::field(m, Tag::MaxFloor));
                order.entered_ord_type = std::string(protocol::field(m, Tag::OrdType));
                order.entered_order_capacity = std::string(protocol::field(m, Tag::OrderCapacity));
                order.entered_side = std::string(protocol::field(m, Tag::Side));
                order.entered_symbol = std::string(protocol::field(m, Tag::Symbol));
                order.entered_account = std::string(protocol::field(m, Tag::Account));
                order.entered_expire_time = std::string(protocol::field(m, Tag::ExpireTime));
                order.entered_anti_internalization_qualifiers_aiqid = std::string(protocol::field(m, Tag::AntiInternalizationQualifiersAIQID));
                order.entered_time_in_force = std::string(protocol::field(m, Tag::TimeInForce));
                const auto ipc = to_fix_new_order(m, order);
                order.remaining = ipc.quantity.get().value();
                order.time_in_force = static_cast<char>(ipc.time_in_force.get().value());
                order.display = static_cast<char>(ipc.display.get().value());
                order.quantity = order.remaining;
                respond(client, kind, [&](protocol::Writer& out) { report(out, order); }, now);
            };
            if (!(!used_.contains({ client, std::string(protocol::field(m, Tag::ClOrdID)) }))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_clordid(out, m, order, now, reported()); }, "8"); return; }
            if (!([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 10000000); }(protocol::field(m, Tag::OrderQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_quantity(out, m, order, now, reported()); }, "8"); return; }
            if (!(protocol::field(m, Tag::Side) == "1" || protocol::field(m, Tag::Side) == "2" || protocol::field(m, Tag::Side) == "5" || protocol::field(m, Tag::Side) == "6")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_side(out, m, order, now, reported()); }, "8"); return; }
            if (!([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= -100 && value <= 100); }(protocol::field(m, Tag::PegDifference)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_peg_difference(out, m, order, now, reported()); }, "8"); return; }
            if (!([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MaxFloor)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_max_floor(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::ExecInst) == "u" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_routed(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::ExecInst) == "f" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_sweep(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::ExecInst) == "y" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_trade_at(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::ExecInst) == "j" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_d_limit(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::TimeInForce) == "4" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_fok(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::TimeInForce) == "2" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_on_open(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::TimeInForce) == "7" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_on_close(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::OrdType) == "2" && protocol::field(m, Tag::MaxFloor) != "0" && !([](std::string_view text) { double value = 0; const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value); return text.empty() || (error == std::errc{} && end == text.data() + text.size() && value >= 0 && value <= 0); }(protocol::field(m, Tag::MinQty)))) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_min_qty_limit(out, m, order, now, reported()); }, "8"); return; }
            if (!(protocol::field(m, Tag::Symbol) == "ZIEXT")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_symbol(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::OrdType) == "2" && !(!protocol::field(m, Tag::Price).empty())) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_price(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::Side) == "5" && !(protocol::field(m, Tag::LocateReqd) == "N")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_locate_short(out, m, order, now, reported()); }, "8"); return; }
            if (protocol::field(m, Tag::Side) == "6" && !(protocol::field(m, Tag::LocateReqd) == "N")) { rejected([&](protocol::Writer& out, const Order& order) { to_fix_invalid_locate_exempt(out, m, order, now, reported()); }, "8"); return; }
            used_.insert({ client, std::string(protocol::field(m, Tag::ClOrdID)) });
            Order order{ numbered(), std::string(protocol::field(m, Tag::ClOrdID)), 0, client, account_, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 0, {}, 0, {}, {}, {}, 0, 0 };
            auto ipc = to_fix_new_order(m, order);
            order.remaining = ipc.quantity.get().value();
            order.time_in_force = static_cast<char>(ipc.time_in_force.get().value());
            order.display = static_cast<char>(ipc.display.get().value());
            order.quantity = order.remaining;
            order.requested = order.reference;
            order.entered_exec_inst = std::string(protocol::field(m, Tag::ExecInst));
            order.entered_min_qty = std::string(protocol::field(m, Tag::MinQty));
            order.entered_max_floor = std::string(protocol::field(m, Tag::MaxFloor));
            order.entered_ord_type = std::string(protocol::field(m, Tag::OrdType));
            order.entered_order_capacity = std::string(protocol::field(m, Tag::OrderCapacity));
            order.entered_side = std::string(protocol::field(m, Tag::Side));
            order.entered_symbol = std::string(protocol::field(m, Tag::Symbol));
            order.entered_account = std::string(protocol::field(m, Tag::Account));
            order.entered_expire_time = std::string(protocol::field(m, Tag::ExpireTime));
            order.entered_anti_internalization_qualifiers_aiqid = std::string(protocol::field(m, Tag::AntiInternalizationQualifiersAIQID));
            order.entered_time_in_force = std::string(protocol::field(m, Tag::TimeInForce));
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
            if ((!protocol::field(m, Tag::ExecInst).empty() && protocol::field(m, Tag::ExecInst) != replaced.entered_exec_inst) || protocol::field(m, Tag::OrdType) != replaced.entered_ord_type || (!protocol::field(m, Tag::OrderCapacity).empty() && protocol::field(m, Tag::OrderCapacity) != replaced.entered_order_capacity) || protocol::field(m, Tag::Side) != replaced.entered_side || protocol::field(m, Tag::Symbol) != replaced.entered_symbol || (!protocol::field(m, Tag::TimeInForce).empty() && protocol::field(m, Tag::TimeInForce) != replaced.entered_time_in_force)) {   // names what it must match otherwise than the order was entered with
                respond(client, "9", [&](protocol::Writer& out) { to_fix_replace_unmatched(out, m, replaced, now); }, now);
                return;
            }
            used_.insert({ client, std::string(protocol::field(m, Tag::ClOrdID)) });
            std::string carried(m);   // the replace, with what it leaves out as the order was entered with it
            if (protocol::field(m, Tag::ExecInst).empty() && !replaced.entered_exec_inst.empty()) carried.append(std::to_string(static_cast<std::uint32_t>(Tag::ExecInst))).append("=").append(replaced.entered_exec_inst).append("\x01");
            if (protocol::field(m, Tag::OrderCapacity).empty() && !replaced.entered_order_capacity.empty()) carried.append(std::to_string(static_cast<std::uint32_t>(Tag::OrderCapacity))).append("=").append(replaced.entered_order_capacity).append("\x01");
            if (protocol::field(m, Tag::TimeInForce).empty() && !replaced.entered_time_in_force.empty()) carried.append(std::to_string(static_cast<std::uint32_t>(Tag::TimeInForce))).append("=").append(replaced.entered_time_in_force).append("\x01");
            if (protocol::field(m, Tag::MinQty).empty() && !replaced.entered_min_qty.empty()) carried.append(std::to_string(static_cast<std::uint32_t>(Tag::MinQty))).append("=").append(replaced.entered_min_qty).append("\x01");
            Order order = replaced;
            if (!protocol::field(m, Tag::ExecInst).empty()) order.entered_exec_inst = std::string(protocol::field(m, Tag::ExecInst));
            if (!protocol::field(m, Tag::OrderCapacity).empty()) order.entered_order_capacity = std::string(protocol::field(m, Tag::OrderCapacity));
            if (!protocol::field(m, Tag::TimeInForce).empty()) order.entered_time_in_force = std::string(protocol::field(m, Tag::TimeInForce));
            if (!protocol::field(m, Tag::MinQty).empty()) order.entered_min_qty = std::string(protocol::field(m, Tag::MinQty));
            order.entered_account = std::string(protocol::field(m, Tag::Account));
            order.id = numbered();
            order.reference = std::string(protocol::field(m, Tag::ClOrdID));
            order.requested = order.reference;
            order.replaced_id = replaced.id;
            order.replaced_reference = replaced.reference;
            order.asked = carried;
            auto ipc = to_fix_replace_order(carried, order);
            order.quantity = ipc.quantity.get().value() + (replaced.quantity - replaced.remaining);   // the whole, its executions included
            order.remaining = ipc.quantity.get().value();
            order.time_in_force = static_cast<char>(ipc.time_in_force.get().value());
            order.display = static_cast<char>(ipc.display.get().value());
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

    Emit emit_;
    Clock clock_;
    protocol::AcceptorConfig config_{};
    std::map<std::size_t, std::unique_ptr<Client>> clients_;
    std::size_t next_client_ = 0;   // session numbers, never reused
    std::map<std::size_t, Connection> connections_;
    std::size_t next_connection_ = 0;   // connection numbers, never reused
    std::map<std::string, std::size_t, std::less<>> comp_ids_;   // each comp id's session, by its number
    std::map<std::string, std::string, std::less<>> credentials_;
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
    std::set<std::pair<std::size_t, std::string>> used_;   // every reference a session has given an order, which none may give again
};

}  // namespace sim::orderentry
