#pragma once

#include <ostream>
#include <string_view>

#include "NewOrderMessage.hpp"
#include "OrderModificationMessage.hpp"
#include "OrderCancellationMessage.hpp"
#include "TradeMessage.hpp"
#include "NewSpreadOrderMessage.hpp"
#include "SpreadOrderModificationMessage.hpp"
#include "SpreadOrderCancellationMessage.hpp"
#include "SpreadTradeMessage.hpp"
#include "TradeCancelMessage.hpp"
#include "HeartbeatMessage.hpp"

namespace nse::nsefo::mtbt::binary::v7_1 {

namespace binary_mtbt = ::nse::nsefo::mtbt::binary::v7_1;

inline std::ostream& operator<<(std::ostream& os, const stream_header& value) {
    os << "message_length=" << value.message_length.get().value()
       << ",stream_id=" << value.stream_id.get().value()
       << ",sequence_number=" << value.sequence_number.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const message& value) {
    os << "message_type=\"" << binary_mtbt::message_type::to_string(value.message_type.get().value()) << '"'
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const new_order_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",order_type=\"" << binary_mtbt::order_type::to_string(msg.fields.order_type.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_modification_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",order_type=\"" << binary_mtbt::order_type::to_string(msg.fields.order_type.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const order_cancellation_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",order_type=\"" << binary_mtbt::order_type::to_string(msg.fields.order_type.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trade_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",buy_order_id=" << msg.fields.buy_order_id.get().value()
       << ",sell_order_id=" << msg.fields.sell_order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",trade_price=" << msg.fields.trade_price.get().value()
       << ",trade_quantity=" << msg.fields.trade_quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const new_spread_order_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",order_type=\"" << binary_mtbt::order_type::to_string(msg.fields.order_type.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_order_modification_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",order_type=\"" << binary_mtbt::order_type::to_string(msg.fields.order_type.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_order_cancellation_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",order_id=" << msg.fields.order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",order_type=\"" << binary_mtbt::order_type::to_string(msg.fields.order_type.get().value()) << '"'
       << ",price=" << msg.fields.price.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const spread_trade_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",buy_order_id=" << msg.fields.buy_order_id.get().value()
       << ",sell_order_id=" << msg.fields.sell_order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",trade_price=" << msg.fields.trade_price.get().value()
       << ",quantity=" << msg.fields.quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const trade_cancel_message& msg) {
    os << "timestamp=" << msg.fields.timestamp.get().value()
       << ",buy_order_id=" << msg.fields.buy_order_id.get().value()
       << ",sell_order_id=" << msg.fields.sell_order_id.get().value()
       << ",token=" << msg.fields.token.get().value()
       << ",trade_price=" << msg.fields.trade_price.get().value()
       << ",trade_quantity=" << msg.fields.trade_quantity.get().value()
       ;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const heartbeat_message& msg) {
    os << "last_sequence_no=" << msg.fields.last_sequence_no.get().value()
       ;
    return os;
}

}
