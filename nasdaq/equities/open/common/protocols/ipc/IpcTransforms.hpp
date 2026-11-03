#pragma once

#include "../marketdata/totalview/messages/AddOrderNoMpidAttributionMessage.hpp"
#include "../marketdata/totalview/messages/OrderExecutedMessage.hpp"
#include "../marketdata/totalview/messages/OrderDeleteMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderCanceledMessage.hpp"
#include <type_traits>
#include <utility>
#include <chrono>
#include <cstdint>

namespace sim {

namespace ipc_ns = nasdaq::nsmequities::matchingengine::ipc::v2026;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// Nanoseconds since the Unix epoch as nanoseconds since midnight in a time zone.
inline std::uint64_t nanoseconds_since_midnight(std::uint64_t unix_nanoseconds, const std::chrono::time_zone* zone) {
    const auto local = zone->to_local(std::chrono::sys_time<std::chrono::nanoseconds>{ std::chrono::nanoseconds{ unix_nanoseconds } });
    return static_cast<std::uint64_t>((local - std::chrono::floor<std::chrono::days>(local)).count());
}

// The America/New_York time zone a timestamp is counted in.
inline const std::chrono::time_zone* new_york() {
    static const auto* zone = std::chrono::locate_zone("America/New_York");
    return zone;
}

// order_rested_message -> totalview:add_order_no_mpid_attribution_message  (NsmEquities.Ipc.Transforms.xml: totalview_add_order)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::add_order_no_mpid_attribution_message to_totalview_add_order(const ipc_ns::order_rested_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::add_order_no_mpid_attribution_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    out.fields.buy_sell_indicator.set(static_cast<value_of<decltype(out.fields.buy_sell_indicator)>>(static_cast<char>(m.side.get().value())));
    out.fields.shares.set(m.quantity.get().value());
    out.fields.stock.set(m.symbol.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    return out;
}

// trade_message -> totalview:order_executed_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_executed)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_message to_totalview_order_executed(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.resting_order_id.get().value());
    out.fields.executed_shares.set(m.quantity.get().value());
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// order_canceled_message -> totalview:order_delete_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_delete)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message to_totalview_order_delete(const ipc_ns::order_canceled_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    return out;
}

}
