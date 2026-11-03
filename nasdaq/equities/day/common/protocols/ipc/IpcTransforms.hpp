#pragma once

#include "../marketdata/totalview/messages/AddOrderNoMpidAttributionMessage.hpp"
#include "../marketdata/totalview/messages/OrderExecutedMessage.hpp"
#include "../marketdata/totalview/messages/OrderExecutedWithPriceMessage.hpp"
#include "../marketdata/totalview/messages/OrderDeleteMessage.hpp"
#include "../marketdata/totalview/messages/SystemEventMessage.hpp"
#include "../marketdata/totalview/messages/StockTradingActionMessage.hpp"
#include "../marketdata/totalview/messages/CrossTradeMessage.hpp"
#include "../marketdata/totalview/messages/NetOrderImbalanceIndicatorMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderCanceledMessage.hpp"
#include "structs/PhaseAnnouncedMessage.hpp"
#include "structs/CrossCompletedMessage.hpp"
#include "structs/ImbalanceIndicatedMessage.hpp"
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

// trade_message -> totalview:order_executed_with_price_message  (NsmEquities.Ipc.Transforms.xml: totalview_crossed_resting)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_with_price_message to_totalview_crossed_resting(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_with_price_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.printable.set(static_cast<value_of<decltype(out.fields.printable)>>('N'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.resting_order_id.get().value());
    out.fields.executed_shares.set(m.quantity.get().value());
    out.fields.match_number.set(m.execution_id.get().value());
    out.fields.execution_price.set(static_cast<value_of<decltype(out.fields.execution_price)>>(m.price.get().value()));
    return out;
}

// trade_message -> totalview:order_executed_with_price_message  (NsmEquities.Ipc.Transforms.xml: totalview_crossed_aggressor)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_with_price_message to_totalview_crossed_aggressor(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_with_price_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.printable.set(static_cast<value_of<decltype(out.fields.printable)>>('N'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.aggressor_order_id.get().value());
    out.fields.executed_shares.set(m.quantity.get().value());
    out.fields.match_number.set(m.execution_id.get().value());
    out.fields.execution_price.set(static_cast<value_of<decltype(out.fields.execution_price)>>(m.price.get().value()));
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

// phase_announced_message -> totalview:system_event_message  (NsmEquities.Ipc.Transforms.xml: totalview_system_event)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::system_event_message to_totalview_system_event(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::system_event_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.event_code.set(static_cast<value_of<decltype(out.fields.event_code)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// phase_announced_message -> totalview:stock_trading_action_message  (NsmEquities.Ipc.Transforms.xml: totalview_stock_trading_action)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message to_totalview_stock_trading_action(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.trading_state.set(static_cast<value_of<decltype(out.fields.trading_state)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// cross_completed_message -> totalview:cross_trade_message  (NsmEquities.Ipc.Transforms.xml: totalview_cross_trade)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::cross_trade_message to_totalview_cross_trade(const ipc_ns::cross_completed_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::cross_trade_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.cross_shares.set(static_cast<value_of<decltype(out.fields.cross_shares)>>(m.quantity.get().value()));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.cross_price.set(static_cast<value_of<decltype(out.fields.cross_price)>>(m.price.get().value()));
    out.fields.match_number.set(m.execution_id.get().value());
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>([](char value) { switch (value) { case 'I': return 'H'; default: return value; } }(static_cast<char>(m.cross.get().value()))));
    return out;
}

// imbalance_indicated_message -> totalview:net_order_imbalance_indicator_message  (NsmEquities.Ipc.Transforms.xml: totalview_noii)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::net_order_imbalance_indicator_message to_totalview_noii(const ipc_ns::imbalance_indicated_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::net_order_imbalance_indicator_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.price_variation_indicator.set(static_cast<value_of<decltype(out.fields.price_variation_indicator)>>('L'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.paired_shares.set(m.paired_quantity.get().value());
    out.fields.imbalance_shares.set(m.imbalance_quantity.get().value());
    out.fields.imbalance_direction.set(static_cast<value_of<decltype(out.fields.imbalance_direction)>>(static_cast<char>(m.imbalance_direction.get().value())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.far_price.set(static_cast<value_of<decltype(out.fields.far_price)>>(m.price.get().value()));
    out.fields.near_price.set(static_cast<value_of<decltype(out.fields.near_price)>>(m.price.get().value()));
    out.fields.current_reference_price.set(static_cast<value_of<decltype(out.fields.current_reference_price)>>(m.price.get().value()));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>([](char value) { switch (value) { case 'I': return 'H'; default: return value; } }(static_cast<char>(m.cross.get().value()))));
    return out;
}

// imbalance_indicated_message -> totalview:net_order_imbalance_indicator_message  (NsmEquities.Ipc.Transforms.xml: totalview_noii_insufficient)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::net_order_imbalance_indicator_message to_totalview_noii_insufficient(const ipc_ns::imbalance_indicated_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::net_order_imbalance_indicator_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(1));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.price_variation_indicator.set(static_cast<value_of<decltype(out.fields.price_variation_indicator)>>(' '));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.paired_shares.set(m.paired_quantity.get().value());
    out.fields.imbalance_shares.set(m.imbalance_quantity.get().value());
    out.fields.imbalance_direction.set(static_cast<value_of<decltype(out.fields.imbalance_direction)>>(static_cast<char>(m.imbalance_direction.get().value())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.far_price.set(static_cast<value_of<decltype(out.fields.far_price)>>(m.price.get().value()));
    out.fields.near_price.set(static_cast<value_of<decltype(out.fields.near_price)>>(m.price.get().value()));
    out.fields.current_reference_price.set(static_cast<value_of<decltype(out.fields.current_reference_price)>>(m.price.get().value()));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>([](char value) { switch (value) { case 'I': return 'H'; default: return value; } }(static_cast<char>(m.cross.get().value()))));
    return out;
}

}
