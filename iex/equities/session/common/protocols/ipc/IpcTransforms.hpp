#pragma once

#include "../marketdata/deep/messages/TradeReportMessage.hpp"
#include "../marketdata/deep/messages/PriceLevelBuyUpdateMessage.hpp"
#include "../marketdata/deep/messages/PriceLevelSellUpdateMessage.hpp"
#include "../marketdata/tops/messages/QuoteUpdateMessage.hpp"
#include "../marketdata/deepplus/messages/AddOrderMessage.hpp"
#include "../marketdata/deepplus/messages/TradeMessage.hpp"
#include "../marketdata/tops/messages/SystemEventMessage.hpp"
#include "../marketdata/deep/messages/SystemEventMessage.hpp"
#include "../marketdata/deepplus/messages/SystemEventMessage.hpp"
#include "../marketdata/tops/messages/TradingStatusMessage.hpp"
#include "../marketdata/deep/messages/TradingStatusMessage.hpp"
#include "../marketdata/deepplus/messages/TradingStatusMessage.hpp"
#include "../marketdata/tops/messages/AuctionInformationMessage.hpp"
#include "../marketdata/deep/messages/AuctionInformationMessage.hpp"
#include "../marketdata/tops/messages/OfficialPriceMessage.hpp"
#include "../marketdata/deep/messages/OfficialPriceMessage.hpp"
#include "../marketdata/deep/messages/SecurityEventMessage.hpp"
#include "../marketdata/deepplus/messages/SecurityEventMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/PhaseAnnouncedMessage.hpp"
#include "structs/ImbalanceIndicatedMessage.hpp"
#include "structs/CrossCompletedMessage.hpp"
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

// phase_announced_message -> tops:system_event_message  (IexEquities.Ipc.Transforms.xml: tops_system_event)
inline iex::iexequities::tops::iextp::v1_66::system_event_message to_tops_system_event(const ipc_ns::phase_announced_message& m) {
    iex::iexequities::tops::iextp::v1_66::system_event_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.system_event.set(static_cast<value_of<decltype(out.fields.system_event)>>([](char value) { switch (value) { case 'Q': return 'R'; default: return value; } }(static_cast<char>(m.announcement.get().value()))));
    return out;
}

// phase_announced_message -> deep:system_event_message  (IexEquities.Ipc.Transforms.xml: deep_system_event)
inline iex::iexequities::deep::iextp::v1_08::system_event_message to_deep_system_event(const ipc_ns::phase_announced_message& m) {
    iex::iexequities::deep::iextp::v1_08::system_event_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.system_event.set(static_cast<value_of<decltype(out.fields.system_event)>>([](char value) { switch (value) { case 'Q': return 'R'; default: return value; } }(static_cast<char>(m.announcement.get().value()))));
    return out;
}

// phase_announced_message -> deepplus:system_event_message  (IexEquities.Ipc.Transforms.xml: deepplus_system_event)
inline iex::iexequities::deepplus::iextp::v1_05::system_event_message to_deepplus_system_event(const ipc_ns::phase_announced_message& m) {
    iex::iexequities::deepplus::iextp::v1_05::system_event_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.system_event.set(static_cast<value_of<decltype(out.fields.system_event)>>([](char value) { switch (value) { case 'Q': return 'R'; default: return value; } }(static_cast<char>(m.announcement.get().value()))));
    return out;
}

// phase_announced_message -> tops:trading_status_message  (IexEquities.Ipc.Transforms.xml: tops_trading_status)
inline iex::iexequities::tops::iextp::v1_66::trading_status_message to_tops_trading_status(const ipc_ns::phase_announced_message& m) {
    iex::iexequities::tops::iextp::v1_66::trading_status_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.trading_status.set(static_cast<value_of<decltype(out.fields.trading_status)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// phase_announced_message -> deep:trading_status_message  (IexEquities.Ipc.Transforms.xml: deep_trading_status)
inline iex::iexequities::deep::iextp::v1_08::trading_status_message to_deep_trading_status(const ipc_ns::phase_announced_message& m) {
    iex::iexequities::deep::iextp::v1_08::trading_status_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.trading_status.set(static_cast<value_of<decltype(out.fields.trading_status)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// phase_announced_message -> deepplus:trading_status_message  (IexEquities.Ipc.Transforms.xml: deepplus_trading_status)
inline iex::iexequities::deepplus::iextp::v1_05::trading_status_message to_deepplus_trading_status(const ipc_ns::phase_announced_message& m) {
    iex::iexequities::deepplus::iextp::v1_05::trading_status_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.trading_status.set(static_cast<value_of<decltype(out.fields.trading_status)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// imbalance_indicated_message -> tops:auction_information_message  (IexEquities.Ipc.Transforms.xml: tops_auction_information)
inline iex::iexequities::tops::iextp::v1_66::auction_information_message to_tops_auction_information(const ipc_ns::imbalance_indicated_message& m) {
    iex::iexequities::tops::iextp::v1_66::auction_information_message out{};
    out.fields.extension_number.set(static_cast<value_of<decltype(out.fields.extension_number)>>(0));
    out.fields.scheduled_auction_time.set(static_cast<value_of<decltype(out.fields.scheduled_auction_time)>>(0));
    out.fields.collar_reference_price.set(static_cast<value_of<decltype(out.fields.collar_reference_price)>>(0));
    out.fields.lower_auction_collar.set(static_cast<value_of<decltype(out.fields.lower_auction_collar)>>(0));
    out.fields.upper_auction_collar.set(static_cast<value_of<decltype(out.fields.upper_auction_collar)>>(0));
    out.fields.auction_type.set(static_cast<value_of<decltype(out.fields.auction_type)>>([](char value) { switch (value) { case 'I': return 'H'; default: return value; } }(static_cast<char>(m.cross.get().value()))));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.paired_shares.set(static_cast<value_of<decltype(out.fields.paired_shares)>>(m.paired_quantity.get().value()));
    out.fields.reference_price.set(m.price.get().value());
    out.fields.indicative_clearing_price.set(m.price.get().value());
    out.fields.imbalance_shares.set(static_cast<value_of<decltype(out.fields.imbalance_shares)>>(m.imbalance_quantity.get().value()));
    out.fields.imbalance_side.set(static_cast<value_of<decltype(out.fields.imbalance_side)>>([](char value) { switch (value) { case 'O': return 'N'; default: return value; } }(static_cast<char>(m.imbalance_direction.get().value()))));
    out.fields.auction_book_clearing_price.set(m.price.get().value());
    return out;
}

// imbalance_indicated_message -> deep:auction_information_message  (IexEquities.Ipc.Transforms.xml: deep_auction_information)
inline iex::iexequities::deep::iextp::v1_08::auction_information_message to_deep_auction_information(const ipc_ns::imbalance_indicated_message& m) {
    iex::iexequities::deep::iextp::v1_08::auction_information_message out{};
    out.fields.extension_number.set(static_cast<value_of<decltype(out.fields.extension_number)>>('0'));
    out.fields.scheduled_auction_time.set(static_cast<value_of<decltype(out.fields.scheduled_auction_time)>>(0));
    out.fields.collar_reference_price.set(static_cast<value_of<decltype(out.fields.collar_reference_price)>>(0));
    out.fields.lower_auction_collar.set(static_cast<value_of<decltype(out.fields.lower_auction_collar)>>(0));
    out.fields.upper_auction_collar.set(static_cast<value_of<decltype(out.fields.upper_auction_collar)>>(0));
    out.fields.auction_type.set(static_cast<value_of<decltype(out.fields.auction_type)>>([](char value) { switch (value) { case 'I': return 'H'; default: return value; } }(static_cast<char>(m.cross.get().value()))));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.paired_shares.set(static_cast<value_of<decltype(out.fields.paired_shares)>>(m.paired_quantity.get().value()));
    out.fields.reference_price.set(m.price.get().value());
    out.fields.indicative_clearing_price.set(m.price.get().value());
    out.fields.imbalance_shares.set(static_cast<value_of<decltype(out.fields.imbalance_shares)>>(m.imbalance_quantity.get().value()));
    out.fields.imbalance_side.set(static_cast<value_of<decltype(out.fields.imbalance_side)>>([](char value) { switch (value) { case 'O': return 'N'; default: return value; } }(static_cast<char>(m.imbalance_direction.get().value()))));
    out.fields.auction_book_clearing_price.set(m.price.get().value());
    return out;
}

// cross_completed_message -> tops:official_price_message  (IexEquities.Ipc.Transforms.xml: tops_official_opening_price)
inline iex::iexequities::tops::iextp::v1_66::official_price_message to_tops_official_opening_price(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::tops::iextp::v1_66::official_price_message out{};
    out.fields.price_type.set(static_cast<value_of<decltype(out.fields.price_type)>>('Q'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.official_price.set(m.price.get().value());
    return out;
}

// cross_completed_message -> tops:official_price_message  (IexEquities.Ipc.Transforms.xml: tops_official_closing_price)
inline iex::iexequities::tops::iextp::v1_66::official_price_message to_tops_official_closing_price(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::tops::iextp::v1_66::official_price_message out{};
    out.fields.price_type.set(static_cast<value_of<decltype(out.fields.price_type)>>('M'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.official_price.set(m.price.get().value());
    return out;
}

// cross_completed_message -> deep:official_price_message  (IexEquities.Ipc.Transforms.xml: deep_official_opening_price)
inline iex::iexequities::deep::iextp::v1_08::official_price_message to_deep_official_opening_price(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::deep::iextp::v1_08::official_price_message out{};
    out.fields.price_type.set(static_cast<value_of<decltype(out.fields.price_type)>>('Q'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.official_price.set(m.price.get().value());
    return out;
}

// cross_completed_message -> deep:official_price_message  (IexEquities.Ipc.Transforms.xml: deep_official_closing_price)
inline iex::iexequities::deep::iextp::v1_08::official_price_message to_deep_official_closing_price(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::deep::iextp::v1_08::official_price_message out{};
    out.fields.price_type.set(static_cast<value_of<decltype(out.fields.price_type)>>('M'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    out.fields.official_price.set(m.price.get().value());
    return out;
}

// cross_completed_message -> deep:security_event_message  (IexEquities.Ipc.Transforms.xml: deep_opening_process_complete)
inline iex::iexequities::deep::iextp::v1_08::security_event_message to_deep_opening_process_complete(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::deep::iextp::v1_08::security_event_message out{};
    out.fields.security_event.set(static_cast<value_of<decltype(out.fields.security_event)>>('O'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    return out;
}

// cross_completed_message -> deep:security_event_message  (IexEquities.Ipc.Transforms.xml: deep_closing_process_complete)
inline iex::iexequities::deep::iextp::v1_08::security_event_message to_deep_closing_process_complete(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::deep::iextp::v1_08::security_event_message out{};
    out.fields.security_event.set(static_cast<value_of<decltype(out.fields.security_event)>>('C'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    return out;
}

// cross_completed_message -> deepplus:security_event_message  (IexEquities.Ipc.Transforms.xml: deepplus_opening_process_complete)
inline iex::iexequities::deepplus::iextp::v1_05::security_event_message to_deepplus_opening_process_complete(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::deepplus::iextp::v1_05::security_event_message out{};
    out.fields.security_event.set(static_cast<value_of<decltype(out.fields.security_event)>>('O'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    return out;
}

// cross_completed_message -> deepplus:security_event_message  (IexEquities.Ipc.Transforms.xml: deepplus_closing_process_complete)
inline iex::iexequities::deepplus::iextp::v1_05::security_event_message to_deepplus_closing_process_complete(const ipc_ns::cross_completed_message& m) {
    iex::iexequities::deepplus::iextp::v1_05::security_event_message out{};
    out.fields.security_event.set(static_cast<value_of<decltype(out.fields.security_event)>>('C'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(m.timestamp.get().value()));
    out.fields.symbol.set(m.symbol.get().value());
    return out;
}

}
