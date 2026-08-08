#pragma once

#include "../marketdata/deep/messages/TradeReportMessage.hpp"
#include "../marketdata/deep/messages/PriceLevelBuyUpdateMessage.hpp"
#include "../marketdata/deep/messages/PriceLevelSellUpdateMessage.hpp"
#include "../marketdata/tops/messages/QuoteUpdateMessage.hpp"
#include "../marketdata/tops/messages/QuoteUpdateMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderRestedMessage.hpp"

namespace sim {

namespace ipc_ns = iex::iexequities::matchingengine::ipc::v1_0;

// trade_message -> deep:trade_report_message  (Ipc.Transforms.xml: deep_trade)
inline iex::iexequities::deep::iextp::v1_08::trade_report_message to_deep_trade(const ipc_ns::trade_message& m) {
    iex::iexequities::deep::iextp::v1_08::trade_report_message out{};
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> deep:price_level_buy_update_message  (Ipc.Transforms.xml: deep_price_level_buy)
inline iex::iexequities::deep::iextp::v1_08::price_level_buy_update_message to_deep_price_level_buy(const ipc_ns::order_rested_message& m) {
    iex::iexequities::deep::iextp::v1_08::price_level_buy_update_message out{};
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> deep:price_level_sell_update_message  (Ipc.Transforms.xml: deep_price_level_sell)
inline iex::iexequities::deep::iextp::v1_08::price_level_sell_update_message to_deep_price_level_sell(const ipc_ns::order_rested_message& m) {
    iex::iexequities::deep::iextp::v1_08::price_level_sell_update_message out{};
    out.fields.price.set(m.price.get().value());
    out.fields.size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> tops:quote_update_message  (Ipc.Transforms.xml: tops_quote_bid)
inline iex::iexequities::tops::iextp::v1_66::quote_update_message to_tops_quote_bid(const ipc_ns::order_rested_message& m) {
    iex::iexequities::tops::iextp::v1_66::quote_update_message out{};
    out.fields.bid_price.set(m.price.get().value());
    out.fields.bid_size.set(m.quantity.get().value());
    return out;
}

// order_rested_message -> tops:quote_update_message  (Ipc.Transforms.xml: tops_quote_ask)
inline iex::iexequities::tops::iextp::v1_66::quote_update_message to_tops_quote_ask(const ipc_ns::order_rested_message& m) {
    iex::iexequities::tops::iextp::v1_66::quote_update_message out{};
    out.fields.ask_price.set(m.price.get().value());
    out.fields.ask_size.set(m.quantity.get().value());
    return out;
}

}
