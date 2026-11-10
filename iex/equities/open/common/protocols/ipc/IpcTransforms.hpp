#pragma once

#include "../marketdata/deep/messages/TradeReportMessage.hpp"
#include "../marketdata/deep/messages/PriceLevelBuyUpdateMessage.hpp"
#include "../marketdata/deep/messages/PriceLevelSellUpdateMessage.hpp"
#include "../marketdata/tops/messages/QuoteUpdateMessage.hpp"
#include "../marketdata/deepplus/messages/AddOrderMessage.hpp"
#include "../marketdata/deepplus/messages/TradeMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include <type_traits>
#include <utility>

namespace sim {

namespace ipc_ns = iex::iexequities::matchingengine::ipc::v1_0;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// trade_message -> deep:trade_report_message  (IexEquities.Ipc.Transforms.xml: deep_trade)
inline iex::iexequities::deep::iextp::v1_08::trade_report_message to_deep_trade(const ipc_ns::trade_message& m) {
    iex::iexequities::deep::iextp::v1_08::trade_report_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> deep:price_level_buy_update_message  (IexEquities.Ipc.Transforms.xml: deep_price_level_buy)
inline iex::iexequities::deep::iextp::v1_08::price_level_buy_update_message to_deep_price_level_buy(const ipc_ns::order_rested_message& m) {
    iex::iexequities::deep::iextp::v1_08::price_level_buy_update_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> deep:price_level_sell_update_message  (IexEquities.Ipc.Transforms.xml: deep_price_level_sell)
inline iex::iexequities::deep::iextp::v1_08::price_level_sell_update_message to_deep_price_level_sell(const ipc_ns::order_rested_message& m) {
    iex::iexequities::deep::iextp::v1_08::price_level_sell_update_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> tops:quote_update_message  (IexEquities.Ipc.Transforms.xml: tops_quote_bid)
inline iex::iexequities::tops::iextp::v1_66::quote_update_message to_tops_quote_bid(const ipc_ns::order_rested_message& m) {
    iex::iexequities::tops::iextp::v1_66::quote_update_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.bid_price.set(m.price.get().value());
    out.fields.bid_size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> tops:quote_update_message  (IexEquities.Ipc.Transforms.xml: tops_quote_ask)
inline iex::iexequities::tops::iextp::v1_66::quote_update_message to_tops_quote_ask(const ipc_ns::order_rested_message& m) {
    iex::iexequities::tops::iextp::v1_66::quote_update_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.ask_price.set(m.price.get().value());
    out.fields.ask_size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> deepplus:add_order_message  (IexEquities.Ipc.Transforms.xml: deepplus_add_order)
inline iex::iexequities::deepplus::iextp::v1_05::add_order_message to_deepplus_add_order(const ipc_ns::order_rested_message& m) {
    iex::iexequities::deepplus::iextp::v1_05::add_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.order_id.set(m.order_id.get().value());
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

// trade_message -> deepplus:trade_message  (IexEquities.Ipc.Transforms.xml: deepplus_trade)
inline iex::iexequities::deepplus::iextp::v1_05::trade_message to_deepplus_trade(const ipc_ns::trade_message& m) {
    iex::iexequities::deepplus::iextp::v1_05::trade_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

}
