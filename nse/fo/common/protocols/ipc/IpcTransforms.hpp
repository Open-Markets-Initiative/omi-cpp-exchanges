#pragma once

#include "../marketdata/mtbt/messages/NewOrderMessage.hpp"
#include "../marketdata/mtbt/messages/OrderCancellationMessage.hpp"
#include "../marketdata/mtbt/messages/TradeMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/OrderCanceledMessage.hpp"
#include "structs/TradeMessage.hpp"
#include <type_traits>
#include <utility>
#include <chrono>
#include <cstdint>

namespace sim {

namespace ipc_ns = nse::nsefo::matchingengine::ipc::v1_0;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// Nanoseconds since the Unix epoch as nanoseconds since the Dos epoch, the first of January
// 1980, read on the wall clock of a time zone.
inline std::uint64_t nanoseconds_since_dos_epoch(std::uint64_t unix_nanoseconds, const std::chrono::time_zone* zone) {
    using namespace std::chrono;
    constexpr auto epoch = sys_days{ January / 1 / 1980 }.time_since_epoch();
    const auto instant = sys_time<nanoseconds>{ nanoseconds{ unix_nanoseconds } };
    const auto local = zone->to_local(instant);
    return static_cast<std::uint64_t>((local.time_since_epoch() - duration_cast<nanoseconds>(epoch)).count());
}

// Nanoseconds since the Unix epoch as nanoseconds since midnight in a time zone.
inline std::uint64_t nanoseconds_since_midnight(std::uint64_t unix_nanoseconds, const std::chrono::time_zone* zone) {
    const auto local = zone->to_local(std::chrono::sys_time<std::chrono::nanoseconds>{ std::chrono::nanoseconds{ unix_nanoseconds } });
    return static_cast<std::uint64_t>((local - std::chrono::floor<std::chrono::days>(local)).count());
}

// The Asia/Kolkata time zone a timestamp is counted in.
inline const std::chrono::time_zone* kolkata() {
    static const auto* zone = std::chrono::locate_zone("Asia/Kolkata");
    return zone;
}

// order_rested_message -> mtbt:new_order_message  (NseFo.Ipc.Transforms.xml: mtbt_new_order)
inline nse::nsefo::mtbt::binary::v7_1::new_order_message to_mtbt_new_order(const ipc_ns::order_rested_message& m) {
    nse::nsefo::mtbt::binary::v7_1::new_order_message out{};
    out.fields.token.set(static_cast<value_of<decltype(out.fields.token)>>(78753));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_dos_epoch(m.timestamp.get().value(), kolkata())));
    out.fields.order_id.set(static_cast<double>(m.order_id.get().value()));
    out.fields.order_type.set(static_cast<value_of<decltype(out.fields.order_type)>>(static_cast<char>(m.side.get().value())));
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(m.quantity.get().value()));
    return out;
}

// order_canceled_message -> mtbt:order_cancellation_message  (NseFo.Ipc.Transforms.xml: mtbt_order_cancellation)
inline nse::nsefo::mtbt::binary::v7_1::order_cancellation_message to_mtbt_order_cancellation(const ipc_ns::order_canceled_message& m) {
    nse::nsefo::mtbt::binary::v7_1::order_cancellation_message out{};
    out.fields.token.set(static_cast<value_of<decltype(out.fields.token)>>(78753));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_dos_epoch(m.timestamp.get().value(), kolkata())));
    out.fields.order_id.set(static_cast<double>(m.order_id.get().value()));
    out.fields.order_type.set(static_cast<value_of<decltype(out.fields.order_type)>>(static_cast<char>(m.side.get().value())));
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(m.quantity.get().value()));
    return out;
}

// trade_message -> mtbt:trade_message  (NseFo.Ipc.Transforms.xml: mtbt_trade)
inline nse::nsefo::mtbt::binary::v7_1::trade_message to_mtbt_trade(const ipc_ns::trade_message& m) {
    nse::nsefo::mtbt::binary::v7_1::trade_message out{};
    out.fields.token.set(static_cast<value_of<decltype(out.fields.token)>>(78753));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_dos_epoch(m.timestamp.get().value(), kolkata())));
    out.fields.buy_order_id.set(static_cast<double>(m.resting_order_id.get().value()));
    out.fields.sell_order_id.set(static_cast<double>(m.aggressor_order_id.get().value()));
    out.fields.trade_price.set(static_cast<value_of<decltype(out.fields.trade_price)>>(m.price.get().value()));
    out.fields.trade_quantity.set(static_cast<value_of<decltype(out.fields.trade_quantity)>>(m.quantity.get().value()));
    return out;
}

}
