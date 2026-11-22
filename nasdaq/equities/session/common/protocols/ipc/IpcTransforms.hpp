#pragma once

#include "../marketdata/totalview/messages/AddOrderNoMpidAttributionMessage.hpp"
#include "../marketdata/totalview/messages/AddOrderWithMpidAttributionMessage.hpp"
#include "../marketdata/totalview/messages/OrderExecutedMessage.hpp"
#include "../marketdata/totalview/messages/OrderExecutedWithPriceMessage.hpp"
#include "../marketdata/totalview/messages/OrderCancelMessage.hpp"
#include "../marketdata/totalview/messages/NonCrossTradeMessage.hpp"
#include "../marketdata/totalview/messages/OrderDeleteMessage.hpp"
#include "../marketdata/totalview/messages/OrderReplaceMessage.hpp"
#include "../marketdata/totalview/messages/SystemEventMessage.hpp"
#include "../marketdata/totalview/messages/IpoQuotingPeriodUpdate.hpp"
#include "../marketdata/totalview/messages/StockTradingActionMessage.hpp"
#include "../marketdata/totalview/messages/OperationalHaltMessage.hpp"
#include "../marketdata/totalview/messages/LuldAuctionCollarMessage.hpp"
#include "../marketdata/totalview/messages/BrokenTradeMessage.hpp"
#include "../marketdata/totalview/messages/MwcbStatusLevelMessage.hpp"
#include "../marketdata/totalview/messages/MwcbDeclineLevelMessage.hpp"
#include "../marketdata/totalview/messages/StockDirectoryMessage.hpp"
#include "../marketdata/totalview/messages/RegShoShortSalePriceTestRestrictedIndicatorMessage.hpp"
#include "../marketdata/totalview/messages/MarketParticipantPositionMessage.hpp"
#include "../marketdata/totalview/messages/CrossTradeMessage.hpp"
#include "../marketdata/totalview/messages/NetOrderImbalanceIndicatorMessage.hpp"
#include "structs/OrderRestedMessage.hpp"
#include "structs/TradeMessage.hpp"
#include "structs/OrderCanceledMessage.hpp"
#include "structs/OrderReplacedMessage.hpp"
#include "structs/OrderReducedMessage.hpp"
#include "structs/OrderEliminatedMessage.hpp"
#include "structs/PhaseAnnouncedMessage.hpp"
#include "structs/TradeBrokenMessage.hpp"
#include "structs/CrossCompletedMessage.hpp"
#include "structs/ImbalanceIndicatedMessage.hpp"
#include <type_traits>
#include <utility>
#include <chrono>
#include <cstdint>
#include <string_view>
#include <cstdint>

namespace sim {

namespace ipc_ns = nasdaq::nsmequities::matchingengine::ipc::v2026;

// Each instrument's IpoPrice, as its listing states it.
inline std::int64_t listing_ipoprice_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{0};
    if (instrument == "ZWZZT") return std::int64_t{0};
    if (instrument == "ZXZZT") return std::int64_t{10000};
    return {};
}

// Each instrument's LuldReference, as its listing states it.
inline std::int64_t listing_luldreference_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{10000};
    if (instrument == "ZWZZT") return std::int64_t{10000};
    if (instrument == "ZXZZT") return std::int64_t{10000};
    return {};
}

// Each instrument's LuldUpper, as its listing states it.
inline std::int64_t listing_luldupper_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{10500};
    if (instrument == "ZWZZT") return std::int64_t{10500};
    if (instrument == "ZXZZT") return std::int64_t{10500};
    return {};
}

// Each instrument's LuldLower, as its listing states it.
inline std::int64_t listing_luldlower_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{9500};
    if (instrument == "ZWZZT") return std::int64_t{9500};
    if (instrument == "ZXZZT") return std::int64_t{9500};
    return {};
}

// Each instrument's MwcbLevel1, as its listing states it.
inline std::int64_t listing_mwcblevel1_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{465000000000};
    if (instrument == "ZWZZT") return std::int64_t{465000000000};
    if (instrument == "ZXZZT") return std::int64_t{465000000000};
    return {};
}

// Each instrument's MwcbLevel2, as its listing states it.
inline std::int64_t listing_mwcblevel2_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{435000000000};
    if (instrument == "ZWZZT") return std::int64_t{435000000000};
    if (instrument == "ZXZZT") return std::int64_t{435000000000};
    return {};
}

// Each instrument's MwcbLevel3, as its listing states it.
inline std::int64_t listing_mwcblevel3_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{400000000000};
    if (instrument == "ZWZZT") return std::int64_t{400000000000};
    if (instrument == "ZXZZT") return std::int64_t{400000000000};
    return {};
}

// Each instrument's MarketCategory, as its listing states it.
inline std::string_view listing_marketcategory(std::string_view instrument) {
    if (instrument == "ZVZZT") return "G";
    if (instrument == "ZWZZT") return "G";
    if (instrument == "ZXZZT") return "G";
    return {};
}

// Each instrument's FinancialStatus, as its listing states it.
inline std::string_view listing_financialstatus(std::string_view instrument) {
    if (instrument == "ZVZZT") return "N";
    if (instrument == "ZWZZT") return "N";
    if (instrument == "ZXZZT") return "N";
    return {};
}

// Each instrument's RoundLot, as its listing states it.
inline std::int64_t listing_roundlot_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{100};
    if (instrument == "ZWZZT") return std::int64_t{100};
    if (instrument == "ZXZZT") return std::int64_t{100};
    return {};
}

// Each instrument's RoundLotsOnly, as its listing states it.
inline std::string_view listing_roundlotsonly(std::string_view instrument) {
    if (instrument == "ZVZZT") return "N";
    if (instrument == "ZWZZT") return "N";
    if (instrument == "ZXZZT") return "N";
    return {};
}

// Each instrument's IssueClassification, as its listing states it.
inline std::string_view listing_issueclassification(std::string_view instrument) {
    if (instrument == "ZVZZT") return "C";
    if (instrument == "ZWZZT") return "C";
    if (instrument == "ZXZZT") return "C";
    return {};
}

// Each instrument's IssueSubType, as its listing states it.
inline std::string_view listing_issuesubtype(std::string_view instrument) {
    if (instrument == "ZVZZT") return "Z";
    if (instrument == "ZWZZT") return "Z";
    if (instrument == "ZXZZT") return "Z";
    return {};
}

// Each instrument's Authenticity, as its listing states it.
inline std::string_view listing_authenticity(std::string_view instrument) {
    if (instrument == "ZVZZT") return "T";
    if (instrument == "ZWZZT") return "T";
    if (instrument == "ZXZZT") return "T";
    return {};
}

// Each instrument's ShortSaleThreshold, as its listing states it.
inline std::string_view listing_shortsalethreshold(std::string_view instrument) {
    if (instrument == "ZVZZT") return "N";
    if (instrument == "ZWZZT") return "N";
    if (instrument == "ZXZZT") return "N";
    return {};
}

// Each instrument's IpoFlag, as its listing states it.
inline std::string_view listing_ipoflag(std::string_view instrument) {
    if (instrument == "ZVZZT") return "N";
    if (instrument == "ZWZZT") return "N";
    if (instrument == "ZXZZT") return "Y";
    return {};
}

// Each instrument's LuldTier, as its listing states it.
inline std::string_view listing_luldtier(std::string_view instrument) {
    if (instrument == "ZVZZT") return "2";
    if (instrument == "ZWZZT") return "2";
    if (instrument == "ZXZZT") return "2";
    return {};
}

// Each instrument's EtpFlag, as its listing states it.
inline std::string_view listing_etpflag(std::string_view instrument) {
    if (instrument == "ZVZZT") return "N";
    if (instrument == "ZWZZT") return "N";
    if (instrument == "ZXZZT") return "N";
    return {};
}

// Each instrument's EtpLeverage, as its listing states it.
inline std::int64_t listing_etpleverage_number(std::string_view instrument) {
    if (instrument == "ZVZZT") return std::int64_t{0};
    if (instrument == "ZWZZT") return std::int64_t{0};
    if (instrument == "ZXZZT") return std::int64_t{0};
    return {};
}

// Each instrument's Inverse, as its listing states it.
inline std::string_view listing_inverse(std::string_view instrument) {
    if (instrument == "ZVZZT") return "N";
    if (instrument == "ZWZZT") return "N";
    if (instrument == "ZXZZT") return "N";
    return {};
}

// Each instrument's RegSho, as its listing states it.
inline std::string_view listing_regsho(std::string_view instrument) {
    if (instrument == "ZVZZT") return "0";
    if (instrument == "ZWZZT") return "0";
    if (instrument == "ZXZZT") return "0";
    return {};
}

// Each instrument's MarketMaker, as its listing states it.
inline std::string_view listing_marketmaker(std::string_view instrument) {
    if (instrument == "ZVZZT") return "SIMM";
    if (instrument == "ZWZZT") return "SIMM";
    if (instrument == "ZXZZT") return "SIMM";
    return {};
}

// Each instrument's PrimaryMarketMaker, as its listing states it.
inline std::string_view listing_primarymarketmaker(std::string_view instrument) {
    if (instrument == "ZVZZT") return "Y";
    if (instrument == "ZWZZT") return "Y";
    if (instrument == "ZXZZT") return "Y";
    return {};
}

// Each instrument's MarketMakerMode, as its listing states it.
inline std::string_view listing_marketmakermode(std::string_view instrument) {
    if (instrument == "ZVZZT") return "N";
    if (instrument == "ZWZZT") return "N";
    if (instrument == "ZXZZT") return "N";
    return {};
}

// Each instrument's MarketParticipantState, as its listing states it.
inline std::string_view listing_marketparticipantstate(std::string_view instrument) {
    if (instrument == "ZVZZT") return "A";
    if (instrument == "ZWZZT") return "A";
    if (instrument == "ZXZZT") return "A";
    return {};
}

// The number a feed identifies each instrument by, as the venue declares it.
inline std::int64_t locate_of(std::string_view instrument) {
    if (instrument == "ZVZZT") return 1;
    if (instrument == "ZWZZT") return 2;
    if (instrument == "ZXZZT") return 3;
    return {};
}

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
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    out.fields.buy_sell_indicator.set(static_cast<value_of<decltype(out.fields.buy_sell_indicator)>>(static_cast<char>(m.side.get().value())));
    out.fields.shares.set(m.quantity.get().value());
    out.fields.stock.set(m.symbol.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    return out;
}

// order_rested_message -> totalview:add_order_with_mpid_attribution_message  (NsmEquities.Ipc.Transforms.xml: totalview_add_order_attributed)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::add_order_with_mpid_attribution_message to_totalview_add_order_attributed(const ipc_ns::order_rested_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::add_order_with_mpid_attribution_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    out.fields.buy_sell_indicator.set(static_cast<value_of<decltype(out.fields.buy_sell_indicator)>>(static_cast<char>(m.side.get().value())));
    out.fields.shares.set(m.quantity.get().value());
    out.fields.stock.set(m.symbol.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.attribution.set(m.attribution.get().value());
    return out;
}

// trade_message -> totalview:order_executed_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_executed)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_message to_totalview_order_executed(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_executed_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
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
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
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
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.printable.set(static_cast<value_of<decltype(out.fields.printable)>>('N'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.aggressor_order_id.get().value());
    out.fields.executed_shares.set(m.quantity.get().value());
    out.fields.match_number.set(m.execution_id.get().value());
    out.fields.execution_price.set(static_cast<value_of<decltype(out.fields.execution_price)>>(m.price.get().value()));
    return out;
}

// trade_message -> totalview:order_cancel_message  (NsmEquities.Ipc.Transforms.xml: totalview_discretion_taken)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_cancel_message to_totalview_discretion_taken(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_cancel_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.aggressor_order_id.get().value());
    out.fields.canceled_shares.set(m.quantity.get().value());
    return out;
}

// trade_message -> totalview:non_cross_trade_message  (NsmEquities.Ipc.Transforms.xml: totalview_hidden_executed)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::non_cross_trade_message to_totalview_hidden_executed(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::non_cross_trade_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(0));
    out.fields.buy_sell_indicator.set(static_cast<value_of<decltype(out.fields.buy_sell_indicator)>>('B'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.shares.set(m.quantity.get().value());
    out.fields.stock.set(m.symbol.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> totalview:non_cross_trade_message  (NsmEquities.Ipc.Transforms.xml: totalview_extended_executed)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::non_cross_trade_message to_totalview_extended_executed(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::non_cross_trade_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(0));
    out.fields.buy_sell_indicator.set(static_cast<value_of<decltype(out.fields.buy_sell_indicator)>>('B'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.shares.set(m.quantity.get().value());
    out.fields.stock.set(m.symbol.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> totalview:non_cross_trade_message  (NsmEquities.Ipc.Transforms.xml: totalview_extended_life_executed)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::non_cross_trade_message to_totalview_extended_life_executed(const ipc_ns::trade_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::non_cross_trade_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(0));
    out.fields.buy_sell_indicator.set(static_cast<value_of<decltype(out.fields.buy_sell_indicator)>>('B'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.shares.set(m.quantity.get().value());
    out.fields.stock.set(m.symbol.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// order_canceled_message -> totalview:order_delete_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_delete)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message to_totalview_order_delete(const ipc_ns::order_canceled_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    return out;
}

// order_replaced_message -> totalview:order_replace_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_replace)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_replace_message to_totalview_order_replace(const ipc_ns::order_replaced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_replace_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.original_order_reference_number.set(m.order_id.get().value());
    out.fields.new_order_reference_number.set(m.replacement_id.get().value());
    out.fields.shares.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    return out;
}

// order_rested_message -> totalview:order_replace_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_repriced)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_replace_message to_totalview_order_repriced(const ipc_ns::order_rested_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_replace_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.original_order_reference_number.set(m.refreshed_from.get().value());
    out.fields.new_order_reference_number.set(m.order_id.get().value());
    out.fields.shares.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    return out;
}

// order_replaced_message -> totalview:order_delete_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_replaced_away)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message to_totalview_order_replaced_away(const ipc_ns::order_replaced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    return out;
}

// order_reduced_message -> totalview:order_cancel_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_cancel)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_cancel_message to_totalview_order_cancel(const ipc_ns::order_reduced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_cancel_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    out.fields.canceled_shares.set(m.quantity.get().value());
    return out;
}

// order_eliminated_message -> totalview:order_delete_message  (NsmEquities.Ipc.Transforms.xml: totalview_order_expired)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message to_totalview_order_expired(const ipc_ns::order_eliminated_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::order_delete_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
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

// phase_announced_message -> totalview:system_event_message  (NsmEquities.Ipc.Transforms.xml: totalview_end_of_messages)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::system_event_message to_totalview_end_of_messages(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::system_event_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.event_code.set(static_cast<value_of<decltype(out.fields.event_code)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// phase_announced_message -> totalview:ipo_quoting_period_update  (NsmEquities.Ipc.Transforms.xml: totalview_ipo_quoting_period_update)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::ipo_quoting_period_update to_totalview_ipo_quoting_period_update(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::ipo_quoting_period_update out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.ipo_quotation_release_qualifier.set(static_cast<value_of<decltype(out.fields.ipo_quotation_release_qualifier)>>('A'));
    out.fields.ipo_price.set(static_cast<std::remove_cvref_t<decltype(out.fields.ipo_price.get().value())>>(listing_ipoprice_number(m.symbol.get_trimmed().value())));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.ipo_quotation_release_time.set(static_cast<value_of<decltype(out.fields.ipo_quotation_release_time)>>((nanoseconds_since_midnight(m.release.get().value(), new_york()) / 1000000000)));
    return out;
}

// phase_announced_message -> totalview:ipo_quoting_period_update  (NsmEquities.Ipc.Transforms.xml: totalview_ipo_postponed)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::ipo_quoting_period_update to_totalview_ipo_postponed(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::ipo_quoting_period_update out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.ipo_quotation_release_time.set(static_cast<value_of<decltype(out.fields.ipo_quotation_release_time)>>(0));
    out.fields.ipo_quotation_release_qualifier.set(static_cast<value_of<decltype(out.fields.ipo_quotation_release_qualifier)>>('C'));
    out.fields.ipo_price.set(static_cast<value_of<decltype(out.fields.ipo_price)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    return out;
}

// phase_announced_message -> totalview:ipo_quoting_period_update  (NsmEquities.Ipc.Transforms.xml: totalview_ipo_postponed_release)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::ipo_quoting_period_update to_totalview_ipo_postponed_release(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::ipo_quoting_period_update out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.ipo_quotation_release_qualifier.set(static_cast<value_of<decltype(out.fields.ipo_quotation_release_qualifier)>>('A'));
    out.fields.ipo_price.set(static_cast<std::remove_cvref_t<decltype(out.fields.ipo_price.get().value())>>(listing_ipoprice_number(m.symbol.get_trimmed().value())));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.ipo_quotation_release_time.set(static_cast<value_of<decltype(out.fields.ipo_quotation_release_time)>>((nanoseconds_since_midnight(m.release.get().value(), new_york()) / 1000000000)));
    return out;
}

// phase_announced_message -> totalview:stock_trading_action_message  (NsmEquities.Ipc.Transforms.xml: totalview_stock_trading_action)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message to_totalview_stock_trading_action(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.reserved.set(std::span<const std::uint8_t>(std::array<std::uint8_t, 1>{ 0x20 }));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.trading_state.set(static_cast<value_of<decltype(out.fields.trading_state)>>(static_cast<char>(m.announcement.get().value())));
    out.fields.reason_code.set(static_cast<value_of<decltype(out.fields.reason_code)>>(m.reason.get().value()));
    return out;
}

// phase_announced_message -> totalview:operational_halt_message  (NsmEquities.Ipc.Transforms.xml: totalview_operational_halt)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::operational_halt_message to_totalview_operational_halt(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::operational_halt_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.market_code.set(static_cast<value_of<decltype(out.fields.market_code)>>('Q'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.operational_halt_action.set(static_cast<value_of<decltype(out.fields.operational_halt_action)>>(static_cast<char>(m.announcement.get().value())));
    return out;
}

// phase_announced_message -> totalview:stock_trading_action_message  (NsmEquities.Ipc.Transforms.xml: totalview_luld_pause)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message to_totalview_luld_pause(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.trading_state.set(static_cast<value_of<decltype(out.fields.trading_state)>>('P'));
    out.fields.reserved.set(std::span<const std::uint8_t>(std::array<std::uint8_t, 1>{ 0x20 }));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.reason_code.set(static_cast<value_of<decltype(out.fields.reason_code)>>(m.reason.get().value()));
    return out;
}

// phase_announced_message -> totalview:stock_trading_action_message  (NsmEquities.Ipc.Transforms.xml: totalview_quotation)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message to_totalview_quotation(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.trading_state.set(static_cast<value_of<decltype(out.fields.trading_state)>>('Q'));
    out.fields.reserved.set(std::span<const std::uint8_t>(std::array<std::uint8_t, 1>{ 0x20 }));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.reason_code.set(static_cast<value_of<decltype(out.fields.reason_code)>>(m.reason.get().value()));
    return out;
}

// phase_announced_message -> totalview:luld_auction_collar_message  (NsmEquities.Ipc.Transforms.xml: totalview_quotation_collar)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::luld_auction_collar_message to_totalview_quotation_collar(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::luld_auction_collar_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.auction_collar_reference_price.set(static_cast<value_of<decltype(out.fields.auction_collar_reference_price)>>(m.collar_reference.get().value()));
    out.fields.upper_auction_collar_price.set(static_cast<value_of<decltype(out.fields.upper_auction_collar_price)>>(m.collar_upper.get().value()));
    out.fields.lower_auction_collar_price.set(static_cast<value_of<decltype(out.fields.lower_auction_collar_price)>>(m.collar_lower.get().value()));
    out.fields.auction_collar_extension.set(m.collar_extension.get().value());
    return out;
}

// phase_announced_message -> totalview:luld_auction_collar_message  (NsmEquities.Ipc.Transforms.xml: totalview_luld_collar)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::luld_auction_collar_message to_totalview_luld_collar(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::luld_auction_collar_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.auction_collar_reference_price.set(static_cast<std::remove_cvref_t<decltype(out.fields.auction_collar_reference_price.get().value())>>(listing_luldreference_number(m.symbol.get_trimmed().value())));
    out.fields.upper_auction_collar_price.set(static_cast<std::remove_cvref_t<decltype(out.fields.upper_auction_collar_price.get().value())>>(listing_luldupper_number(m.symbol.get_trimmed().value())));
    out.fields.lower_auction_collar_price.set(static_cast<std::remove_cvref_t<decltype(out.fields.lower_auction_collar_price.get().value())>>(listing_luldlower_number(m.symbol.get_trimmed().value())));
    out.fields.auction_collar_extension.set(static_cast<value_of<decltype(out.fields.auction_collar_extension)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    return out;
}

// trade_broken_message -> totalview:broken_trade_message  (NsmEquities.Ipc.Transforms.xml: totalview_broken_trade)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::broken_trade_message to_totalview_broken_trade(const ipc_ns::trade_broken_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::broken_trade_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// phase_announced_message -> totalview:luld_auction_collar_message  (NsmEquities.Ipc.Transforms.xml: totalview_luld_collar_extended)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::luld_auction_collar_message to_totalview_luld_collar_extended(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::luld_auction_collar_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.auction_collar_reference_price.set(static_cast<value_of<decltype(out.fields.auction_collar_reference_price)>>(m.collar_reference.get().value()));
    out.fields.upper_auction_collar_price.set(static_cast<value_of<decltype(out.fields.upper_auction_collar_price)>>(m.collar_upper.get().value()));
    out.fields.lower_auction_collar_price.set(static_cast<value_of<decltype(out.fields.lower_auction_collar_price)>>(m.collar_lower.get().value()));
    out.fields.auction_collar_extension.set(m.collar_extension.get().value());
    return out;
}

// phase_announced_message -> totalview:mwcb_status_level_message  (NsmEquities.Ipc.Transforms.xml: totalview_mwcb_status_1)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_status_level_message to_totalview_mwcb_status_1(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_status_level_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.breached_level.set(static_cast<value_of<decltype(out.fields.breached_level)>>('1'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    return out;
}

// phase_announced_message -> totalview:mwcb_status_level_message  (NsmEquities.Ipc.Transforms.xml: totalview_mwcb_status_2)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_status_level_message to_totalview_mwcb_status_2(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_status_level_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.breached_level.set(static_cast<value_of<decltype(out.fields.breached_level)>>('2'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    return out;
}

// phase_announced_message -> totalview:mwcb_status_level_message  (NsmEquities.Ipc.Transforms.xml: totalview_mwcb_status_3)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_status_level_message to_totalview_mwcb_status_3(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_status_level_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.breached_level.set(static_cast<value_of<decltype(out.fields.breached_level)>>('3'));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    return out;
}

// phase_announced_message -> totalview:mwcb_decline_level_message  (NsmEquities.Ipc.Transforms.xml: totalview_mwcb_decline_level)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_decline_level_message to_totalview_mwcb_decline_level(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::mwcb_decline_level_message out{};
    out.fields.stock_locate.set(static_cast<value_of<decltype(out.fields.stock_locate)>>(0));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.level_1.set(static_cast<std::remove_cvref_t<decltype(out.fields.level_1.get().value())>>(listing_mwcblevel1_number(m.symbol.get_trimmed().value())));
    out.fields.level_2.set(static_cast<std::remove_cvref_t<decltype(out.fields.level_2.get().value())>>(listing_mwcblevel2_number(m.symbol.get_trimmed().value())));
    out.fields.level_3.set(static_cast<std::remove_cvref_t<decltype(out.fields.level_3.get().value())>>(listing_mwcblevel3_number(m.symbol.get_trimmed().value())));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    return out;
}

// phase_announced_message -> totalview:stock_directory_message  (NsmEquities.Ipc.Transforms.xml: totalview_stock_directory)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_directory_message to_totalview_stock_directory(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_directory_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.market_category.set(static_cast<std::remove_cvref_t<decltype(out.fields.market_category.get().value())>>(listing_marketcategory(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_marketcategory(m.symbol.get_trimmed().value()).front()));
    out.fields.financial_status_indicator.set(static_cast<std::remove_cvref_t<decltype(out.fields.financial_status_indicator.get().value())>>(listing_financialstatus(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_financialstatus(m.symbol.get_trimmed().value()).front()));
    out.fields.round_lot_size.set(static_cast<std::remove_cvref_t<decltype(out.fields.round_lot_size.get().value())>>(listing_roundlot_number(m.symbol.get_trimmed().value())));
    out.fields.round_lots_only.set(static_cast<std::remove_cvref_t<decltype(out.fields.round_lots_only.get().value())>>(listing_roundlotsonly(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_roundlotsonly(m.symbol.get_trimmed().value()).front()));
    out.fields.issue_classification.set(static_cast<std::remove_cvref_t<decltype(out.fields.issue_classification.get().value())>>(listing_issueclassification(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_issueclassification(m.symbol.get_trimmed().value()).front()));
    out.fields.issue_sub_type.set(listing_issuesubtype(m.symbol.get_trimmed().value()));
    out.fields.authenticity.set(static_cast<std::remove_cvref_t<decltype(out.fields.authenticity.get().value())>>(listing_authenticity(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_authenticity(m.symbol.get_trimmed().value()).front()));
    out.fields.short_sale_threshold_indicator.set(static_cast<std::remove_cvref_t<decltype(out.fields.short_sale_threshold_indicator.get().value())>>(listing_shortsalethreshold(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_shortsalethreshold(m.symbol.get_trimmed().value()).front()));
    out.fields.ipo_flag.set(static_cast<std::remove_cvref_t<decltype(out.fields.ipo_flag.get().value())>>(listing_ipoflag(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_ipoflag(m.symbol.get_trimmed().value()).front()));
    out.fields.luld_reference_price_tier.set(static_cast<std::remove_cvref_t<decltype(out.fields.luld_reference_price_tier.get().value())>>(listing_luldtier(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_luldtier(m.symbol.get_trimmed().value()).front()));
    out.fields.etp_flag.set(static_cast<std::remove_cvref_t<decltype(out.fields.etp_flag.get().value())>>(listing_etpflag(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_etpflag(m.symbol.get_trimmed().value()).front()));
    out.fields.etp_leverage_factor.set(static_cast<std::remove_cvref_t<decltype(out.fields.etp_leverage_factor.get().value())>>(listing_etpleverage_number(m.symbol.get_trimmed().value())));
    out.fields.inverse_indicator.set(static_cast<std::remove_cvref_t<decltype(out.fields.inverse_indicator.get().value())>>(listing_inverse(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_inverse(m.symbol.get_trimmed().value()).front()));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    return out;
}

// phase_announced_message -> totalview:stock_trading_action_message  (NsmEquities.Ipc.Transforms.xml: totalview_trading_action_spin)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message to_totalview_trading_action_spin(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::stock_trading_action_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.trading_state.set(static_cast<value_of<decltype(out.fields.trading_state)>>('T'));
    out.fields.reserved.set(std::span<const std::uint8_t>(std::array<std::uint8_t, 1>{ 0x20 }));
    out.fields.reason_code.set(static_cast<value_of<decltype(out.fields.reason_code)>>(" "));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    return out;
}

// phase_announced_message -> totalview:reg_sho_short_sale_price_test_restricted_indicator_message  (NsmEquities.Ipc.Transforms.xml: totalview_reg_sho_spin)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::reg_sho_short_sale_price_test_restricted_indicator_message to_totalview_reg_sho_spin(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::reg_sho_short_sale_price_test_restricted_indicator_message out{};
    out.fields.locate_code.set(static_cast<std::remove_cvref_t<decltype(out.fields.locate_code.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.reg_sho_action.set(static_cast<std::remove_cvref_t<decltype(out.fields.reg_sho_action.get().value())>>(listing_regsho(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_regsho(m.symbol.get_trimmed().value()).front()));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    return out;
}

// phase_announced_message -> totalview:market_participant_position_message  (NsmEquities.Ipc.Transforms.xml: totalview_market_participant_spin)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::market_participant_position_message to_totalview_market_participant_spin(const ipc_ns::phase_announced_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::market_participant_position_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.mpid.set(listing_marketmaker(m.symbol.get_trimmed().value()));
    out.fields.primary_market_maker.set(static_cast<std::remove_cvref_t<decltype(out.fields.primary_market_maker.get().value())>>(listing_primarymarketmaker(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_primarymarketmaker(m.symbol.get_trimmed().value()).front()));
    out.fields.market_maker_mode.set(static_cast<std::remove_cvref_t<decltype(out.fields.market_maker_mode.get().value())>>(listing_marketmakermode(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_marketmakermode(m.symbol.get_trimmed().value()).front()));
    out.fields.market_participant_state.set(static_cast<std::remove_cvref_t<decltype(out.fields.market_participant_state.get().value())>>(listing_marketparticipantstate(m.symbol.get_trimmed().value()).empty() ? ' ' : listing_marketparticipantstate(m.symbol.get_trimmed().value()).front()));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.stock.set(m.symbol.get().value());
    return out;
}

// cross_completed_message -> totalview:cross_trade_message  (NsmEquities.Ipc.Transforms.xml: totalview_cross_trade)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::cross_trade_message to_totalview_cross_trade(const ipc_ns::cross_completed_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::cross_trade_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.cross_shares.set(static_cast<value_of<decltype(out.fields.cross_shares)>>(m.quantity.get().value()));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.cross_price.set(static_cast<value_of<decltype(out.fields.cross_price)>>(m.price.get().value()));
    out.fields.match_number.set(m.execution_id.get().value());
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>(static_cast<char>(m.cross.get().value())));
    return out;
}

// cross_completed_message -> totalview:cross_trade_message  (NsmEquities.Ipc.Transforms.xml: totalview_halt_cross_trade)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::cross_trade_message to_totalview_halt_cross_trade(const ipc_ns::cross_completed_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::cross_trade_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
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
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.paired_shares.set(m.paired_quantity.get().value());
    out.fields.imbalance_shares.set(m.imbalance_quantity.get().value());
    out.fields.imbalance_direction.set(static_cast<value_of<decltype(out.fields.imbalance_direction)>>(static_cast<char>(m.imbalance_direction.get().value())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.far_price.set(static_cast<value_of<decltype(out.fields.far_price)>>(m.far_price.get().value()));
    out.fields.near_price.set(static_cast<value_of<decltype(out.fields.near_price)>>(m.near_price.get().value()));
    out.fields.current_reference_price.set(static_cast<value_of<decltype(out.fields.current_reference_price)>>(m.reference_price.get().value()));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>([](char value) { switch (value) { case 'I': return 'H'; default: return value; } }(static_cast<char>(m.cross.get().value()))));
    out.fields.price_variation_indicator.set(static_cast<value_of<decltype(out.fields.price_variation_indicator)>>(static_cast<char>(m.price_variation.get().value())));
    return out;
}

// imbalance_indicated_message -> totalview:net_order_imbalance_indicator_message  (NsmEquities.Ipc.Transforms.xml: totalview_noii_insufficient)
inline nasdaq::nsmequities::totalview::itch::v5_0_2026::net_order_imbalance_indicator_message to_totalview_noii_insufficient(const ipc_ns::imbalance_indicated_message& m) {
    nasdaq::nsmequities::totalview::itch::v5_0_2026::net_order_imbalance_indicator_message out{};
    out.fields.stock_locate.set(static_cast<std::remove_cvref_t<decltype(out.fields.stock_locate.get().value())>>(locate_of(m.symbol.get_trimmed().value())));
    out.fields.tracking_number.set(static_cast<value_of<decltype(out.fields.tracking_number)>>(0));
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(m.timestamp.get().value(), new_york())));
    out.fields.paired_shares.set(m.paired_quantity.get().value());
    out.fields.imbalance_shares.set(m.imbalance_quantity.get().value());
    out.fields.imbalance_direction.set(static_cast<value_of<decltype(out.fields.imbalance_direction)>>(static_cast<char>(m.imbalance_direction.get().value())));
    out.fields.stock.set(m.symbol.get().value());
    out.fields.far_price.set(static_cast<value_of<decltype(out.fields.far_price)>>(m.far_price.get().value()));
    out.fields.near_price.set(static_cast<value_of<decltype(out.fields.near_price)>>(m.near_price.get().value()));
    out.fields.current_reference_price.set(static_cast<value_of<decltype(out.fields.current_reference_price)>>(m.reference_price.get().value()));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>([](char value) { switch (value) { case 'I': return 'H'; default: return value; } }(static_cast<char>(m.cross.get().value()))));
    out.fields.price_variation_indicator.set(static_cast<value_of<decltype(out.fields.price_variation_indicator)>>(static_cast<char>(m.price_variation.get().value())));
    return out;
}

}
