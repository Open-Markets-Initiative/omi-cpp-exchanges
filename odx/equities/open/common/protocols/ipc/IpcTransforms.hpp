#pragma once

#include "../marketdata/itch/messages/OrderAddedMessage.hpp"
#include "../marketdata/itch/messages/OrderExecutedMessage.hpp"
#include "../marketdata/itch/messages/OrderDeletedMessage.hpp"
#include "../marketdata/itch/messages/OrderReplacedMessage.hpp"
#include "../marketdata/itch/messages/SystemEventMessage.hpp"
#include "../marketdata/itch/messages/PriceTickSizeMessage.hpp"
#include "../marketdata/itch/messages/OrderbookDirectoryMessage.hpp"
#include "../marketdata/itch/messages/TradingStateMessage.hpp"
#include "../marketdata/itch/messages/SecondsMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderCanceledMessage.hpp"
#include "structs/OrderReplacedMessage.hpp"
#include "structs/PhaseAnnouncedMessage.hpp"
#include <type_traits>
#include <utility>
#include <chrono>
#include <cstdint>
#include <string_view>
#include <cstdint>
#include <span>

namespace sim {

namespace ipc_ns = odx::odxequities::matchingengine::ipc::v2026;

// The group each instrument is listed in, as the venue declares it.
inline std::string_view group_of(std::string_view instrument) {
    if (instrument == "9999") return "DAY";
    return {};
}

// Each instrument's Isin, as its listing states it.
inline std::string_view listing_isin(std::string_view instrument) {
    if (instrument == "9999") return "JP3999900008";
    return {};
}

// Each instrument's RoundLot, as its listing states it.
inline std::int64_t listing_roundlot_number(std::string_view instrument) {
    if (instrument == "9999") return std::int64_t{100};
    return {};
}

// Each instrument's TickTable, as its listing states it.
inline std::int64_t listing_ticktable_number(std::string_view instrument) {
    if (instrument == "9999") return std::int64_t{1};
    return {};
}

// Each instrument's Decimals, as its listing states it.
inline std::int64_t listing_decimals_number(std::string_view instrument) {
    if (instrument == "9999") return std::int64_t{1};
    return {};
}

// Each instrument's UpperLimit, as its listing states it.
inline std::int64_t listing_upperlimit_number(std::string_view instrument) {
    if (instrument == "9999") return std::int64_t{13000};
    return {};
}

// Each instrument's LowerLimit, as its listing states it.
inline std::int64_t listing_lowerlimit_number(std::string_view instrument) {
    if (instrument == "9999") return std::int64_t{7000};
    return {};
}

// One tick of a tick table: the table, its size, and the price (or quantity) it starts at.
struct Tick { std::int64_t table; std::int64_t size; std::int64_t start; };

inline constexpr Tick tick_table_1[] = { { 1, 1, 0 }, { 1, 5, 30000 }, { 1, 10, 50000 }, { 1, 10, 100000 }, { 1, 50, 300000 }, { 1, 100, 500000 }, { 1, 100, 1000000 }, { 1, 500, 3000000 }, { 1, 1000, 5000000 }, { 1, 1000, 10000000 } };

// The ticks of the table each instrument's TickTable names, as the venue declares them.
inline std::span<const Tick> ticks_of_ticktable(std::string_view instrument) {
    if (instrument == "9999") return tick_table_1;
    return {};
}

// Every table an instrument's TickTable names, once each, in the order the instruments first name them.
inline constexpr Tick every_ticktable[] = { { 1, 1, 0 }, { 1, 5, 30000 }, { 1, 10, 50000 }, { 1, 10, 100000 }, { 1, 50, 300000 }, { 1, 100, 500000 }, { 1, 100, 1000000 }, { 1, 500, 3000000 }, { 1, 1000, 5000000 }, { 1, 1000, 10000000 } };

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// Nanoseconds since the Unix epoch as nanoseconds since midnight in a time zone.
inline std::uint64_t nanoseconds_since_midnight(std::uint64_t unix_nanoseconds, const std::chrono::time_zone* zone) {
    const auto local = zone->to_local(std::chrono::sys_time<std::chrono::nanoseconds>{ std::chrono::nanoseconds{ unix_nanoseconds } });
    return static_cast<std::uint64_t>((local - std::chrono::floor<std::chrono::days>(local)).count());
}

// The Asia/Tokyo time zone a timestamp is counted in.
inline const std::chrono::time_zone* tokyo() {
    static const auto* zone = std::chrono::locate_zone("Asia/Tokyo");
    return zone;
}

// Itch states each message's time as the nanoseconds into a second, and the second
// itself in a Seconds Message, sent as each second starts: this keeps the one last sent.
struct ItchSeconds {
    std::int64_t last = -1;

    // The Seconds Message that goes ahead of a message whose event was written at the
    // time `m` states, where that second is not the one last sent.
    template <typename Ipc>
    bool due(const Ipc& m, odx::odxequities::pts::itch::v2_2::seconds_message& out) {
        out = {};
        out.fields.seconds.set(static_cast<value_of<decltype(out.fields.seconds)>>((nanoseconds_since_midnight(m.timestamp.get().value(), tokyo()) / 1000000000)));
        const auto second = static_cast<std::int64_t>(out.fields.seconds.get().value());
        if (second == last) return false;
        last = second;
        return true;
    }
};

// order_rested_message -> itch:order_added_message  (OdxEquities.Ipc.Transforms.xml: itch_order_added)
inline odx::odxequities::pts::itch::v2_2::order_added_message to_itch_order_added(const ipc_ns::order_rested_message& m) {
    odx::odxequities::pts::itch::v2_2::order_added_message out{};
    out.fields.group.set(group_of(m.orderbook.get_trimmed().value()));
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    out.fields.order_number.set(m.order_id.get().value());
    out.fields.buy_sell_indicator.set(static_cast<value_of<decltype(out.fields.buy_sell_indicator)>>(static_cast<char>(m.side.get().value())));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.orderbook_id.set(m.orderbook.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    return out;
}

// trade_message -> itch:order_executed_message  (OdxEquities.Ipc.Transforms.xml: itch_order_executed)
inline odx::odxequities::pts::itch::v2_2::order_executed_message to_itch_order_executed(const ipc_ns::trade_message& m) {
    odx::odxequities::pts::itch::v2_2::order_executed_message out{};
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    out.fields.order_number.set(m.resting_order_id.get().value());
    out.fields.executed_quantity.set(m.quantity.get().value());
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// order_canceled_message -> itch:order_deleted_message  (OdxEquities.Ipc.Transforms.xml: itch_order_deleted)
inline odx::odxequities::pts::itch::v2_2::order_deleted_message to_itch_order_deleted(const ipc_ns::order_canceled_message& m) {
    odx::odxequities::pts::itch::v2_2::order_deleted_message out{};
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    out.fields.order_number.set(m.order_id.get().value());
    return out;
}

// order_replaced_message -> itch:order_replaced_message  (OdxEquities.Ipc.Transforms.xml: itch_order_replaced)
inline odx::odxequities::pts::itch::v2_2::order_replaced_message to_itch_order_replaced(const ipc_ns::order_replaced_message& m) {
    odx::odxequities::pts::itch::v2_2::order_replaced_message out{};
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    out.fields.original_order_number.set(m.order_id.get().value());
    out.fields.new_order_number.set(m.replacement_id.get().value());
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    return out;
}

// phase_announced_message -> itch:system_event_message  (OdxEquities.Ipc.Transforms.xml: itch_system_event)
inline odx::odxequities::pts::itch::v2_2::system_event_message to_itch_system_event(const ipc_ns::phase_announced_message& m) {
    odx::odxequities::pts::itch::v2_2::system_event_message out{};
    out.fields.group.set(static_cast<value_of<decltype(out.fields.group)>>(" "));
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    out.fields.system_event.set(static_cast<value_of<decltype(out.fields.system_event)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// phase_announced_message -> itch:price_tick_size_message  (OdxEquities.Ipc.Transforms.xml: itch_price_tick_size)
inline odx::odxequities::pts::itch::v2_2::price_tick_size_message to_itch_price_tick_size(const ipc_ns::phase_announced_message& m, const Tick& tick) {
    odx::odxequities::pts::itch::v2_2::price_tick_size_message out{};
    out.fields.price_tick_size_table_id.set(static_cast<std::remove_cvref_t<decltype(out.fields.price_tick_size_table_id.get().value())>>(tick.table));
    out.fields.price_tick_size.set(static_cast<std::remove_cvref_t<decltype(out.fields.price_tick_size.get().value())>>(tick.size));
    out.fields.price_start.set(static_cast<std::remove_cvref_t<decltype(out.fields.price_start.get().value())>>(tick.start));
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    return out;
}

// phase_announced_message -> itch:orderbook_directory_message  (OdxEquities.Ipc.Transforms.xml: itch_orderbook_directory)
inline odx::odxequities::pts::itch::v2_2::orderbook_directory_message to_itch_orderbook_directory(const ipc_ns::phase_announced_message& m) {
    odx::odxequities::pts::itch::v2_2::orderbook_directory_message out{};
    out.fields.orderbook_code.set(listing_isin(m.orderbook.get_trimmed().value()));
    out.fields.group.set(group_of(m.orderbook.get_trimmed().value()));
    out.fields.round_lot_size.set(static_cast<std::remove_cvref_t<decltype(out.fields.round_lot_size.get().value())>>(listing_roundlot_number(m.orderbook.get_trimmed().value())));
    out.fields.price_tick_size_table_id.set(static_cast<std::remove_cvref_t<decltype(out.fields.price_tick_size_table_id.get().value())>>(listing_ticktable_number(m.orderbook.get_trimmed().value())));
    out.fields.price_decimals.set(static_cast<std::remove_cvref_t<decltype(out.fields.price_decimals.get().value())>>(listing_decimals_number(m.orderbook.get_trimmed().value())));
    out.fields.upper_price_limit.set(static_cast<std::remove_cvref_t<decltype(out.fields.upper_price_limit.get().value())>>(listing_upperlimit_number(m.orderbook.get_trimmed().value())));
    out.fields.lower_price_limit.set(static_cast<std::remove_cvref_t<decltype(out.fields.lower_price_limit.get().value())>>(listing_lowerlimit_number(m.orderbook.get_trimmed().value())));
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    out.fields.orderbook_id.set(m.orderbook.get().value());
    return out;
}

// phase_announced_message -> itch:trading_state_message  (OdxEquities.Ipc.Transforms.xml: itch_trading_state)
inline odx::odxequities::pts::itch::v2_2::trading_state_message to_itch_trading_state(const ipc_ns::phase_announced_message& m) {
    odx::odxequities::pts::itch::v2_2::trading_state_message out{};
    out.fields.group.set(group_of(m.orderbook.get_trimmed().value()));
    out.fields.nanoseconds.set(static_cast<value_of<decltype(out.fields.nanoseconds)>>(((m.timestamp.get().value()) % 1000000000ULL)));
    out.fields.orderbook_id.set(m.orderbook.get().value());
    out.fields.trading_state.set(static_cast<value_of<decltype(out.fields.trading_state)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

}
