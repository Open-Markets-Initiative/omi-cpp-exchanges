#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <chrono>
#include <deque>
#include "messages/EnterOrderMessage.hpp"
#include "messages/CancelOrderMessage.hpp"
#include "messages/ModifyOrderMessage.hpp"
#include "messages/ReplaceOrderMessage.hpp"
#include "messages/MassCancelRequestMessage.hpp"
#include "messages/MassCancelResponseMessage.hpp"
#include "messages/DisableOrderEntryResponseMessage.hpp"
#include "messages/EnableOrderEntryResponseMessage.hpp"
#include "messages/RejectedOrderMessage.hpp"
#include "messages/AccountQueryResponseMessage.hpp"
#include "messages/SystemEventMessage.hpp"
#include "messages/OrderAcceptedMessage.hpp"
#include "messages/ReplacedMessage.hpp"
#include "messages/OrderModifiedMessage.hpp"
#include "messages/OrderExecutedMessage.hpp"
#include "messages/BrokenTradeMessage.hpp"
#include "messages/OrderRestatedMessage.hpp"
#include "messages/OrderPriorityUpdateMessage.hpp"
#include "messages/CancelPendingMessage.hpp"
#include "messages/CancelRejectMessage.hpp"
#include "messages/AiqCanceledMessage.hpp"
#include "messages/CanceledMessage.hpp"
#include "messages/DisableOrderEntryRequestMessage.hpp"
#include "messages/EnableOrderEntryRequestMessage.hpp"
#include "messages/AccountQueryMessage.hpp"
#include "structs/ServerPacketHeader.hpp"
#include "structs/ClientPacketHeader.hpp"
#include "messages/UnsequencedDataPacket.hpp"
#include "SoupBinSession.hpp"
#include "../ipc/IpcBus.hpp"
#include "../ipc/structs/NewOrderMessage.hpp"
#include "../ipc/structs/CancelOrderMessage.hpp"
#include "../ipc/structs/ReduceOrderMessage.hpp"
#include "../ipc/structs/ReplaceOrderMessage.hpp"
#include "../ipc/structs/PhaseAnnouncedMessage.hpp"
#include "../ipc/structs/TradeMessage.hpp"
#include "../ipc/structs/TradeBrokenMessage.hpp"
#include "../ipc/structs/OrderRestedMessage.hpp"
#include "../ipc/structs/CancelPendingMessage.hpp"
#include "../ipc/structs/ReduceRefusedMessage.hpp"
#include "../ipc/structs/OrderCanceledMessage.hpp"
#include "../ipc/structs/OrderReducedMessage.hpp"
#include "../ipc/structs/OrderEliminatedMessage.hpp"
#include "../ipc/structs/OrderRefusedMessage.hpp"
#include "../ipc/structs/OrderAcceptedMessage.hpp"
#include "../ipc/structs/OrderReplacedMessage.hpp"

namespace sim::orderentry {

namespace protocol = ::nasdaq::nsmequities::orders::ouch::v5_0;
namespace ipc_ns = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// One option of a message's appendage of options - each a length byte, counting what follows it, then
// the option's tag and its value - in the bytes the message's appendage length counts: the value of
// the option of this tag and size, big endian, or none where the message carries no such option.
inline std::optional<std::uint64_t> option(const std::byte* begin, const std::byte* end, std::uint8_t tag, std::size_t size) {
    for (const std::byte* at = begin; at + 2 <= end; ) {
        const std::size_t length = std::to_integer<std::uint8_t>(at[0]);
        if (length == 0 || at + 1 + length > end) return std::nullopt;
        if (std::to_integer<std::uint8_t>(at[1]) == tag && length - 1 == size) {
            std::uint64_t value = 0;
            for (std::size_t i = 0; i < size; ++i) value = (value << 8) | std::to_integer<std::uint8_t>(at[2 + i]);
            return value;
        }
        at += 1 + length;
    }
    return std::nullopt;
}

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

// A message's appendage of options as it arrived: the bytes its length counts, no further than
// the packet went.
template <typename Message>
std::string_view appendage(const Message& m, std::string_view packet) {
    const char* begin = reinterpret_cast<const char*>(m.tail);
    const char* end = packet.data() + packet.size();
    if (begin >= end) return {};
    return { begin, std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(end - begin)) };
}

// The options of an appendage a response echoes: each element - a length counting its tag and
// its value, the tag, the value - whose tag is one of those named, as it arrived. One a zero
// leaves unstated is left out where its value is zero.
inline std::string echoed(std::string_view options, std::initializer_list<std::uint8_t> tags, std::initializer_list<std::uint8_t> unset) {
    std::string out;
    for (std::size_t at = 0; at + 2 <= options.size(); ) {
        const std::size_t length = static_cast<std::uint8_t>(options[at]);
        if (length == 0 || at + 1 + length > options.size()) break;
        const auto tag = static_cast<std::uint8_t>(options[at + 1]);
        const bool zero = options.substr(at + 2, length - 1).find_first_not_of('\0') == std::string_view::npos;
        if (std::find(tags.begin(), tags.end(), tag) != tags.end() && !(zero && std::find(unset.begin(), unset.end(), tag) != unset.end()))
            out.append(options.substr(at, 1 + length));
        at += 1 + length;
    }
    return out;
}

// An option a response states of its own: its length, counting its tag and its value, the tag,
// and the value as a number of that many bytes, the first the most significant.
inline std::string stated(std::uint8_t tag, std::size_t size, std::uint64_t value) {
    std::string out;
    out.push_back(static_cast<char>(1 + size));
    out.push_back(static_cast<char>(tag));
    for (std::size_t at = size; at > 0; --at) out.push_back(static_cast<char>((value >> (8 * (at - 1))) & 0xff));
    return out;
}

// A replacement's options: those its replace gives, and every one of the order replaced that the
// replace leaves out - but for those a replace must give again.
inline std::string merged(std::string_view kept, std::string_view given, std::initializer_list<std::uint8_t> forgotten) {
    std::string out(given);
    for (std::size_t at = 0; at + 2 <= kept.size(); ) {
        const std::size_t length = static_cast<std::uint8_t>(kept[at]);
        if (length == 0 || at + 1 + length > kept.size()) break;
        const auto tag = static_cast<std::uint8_t>(kept[at + 1]);
        bool stated = std::find(forgotten.begin(), forgotten.end(), tag) != forgotten.end();
        for (std::size_t in = 0; !stated && in + 2 <= given.size(); ) {
            const std::size_t size = static_cast<std::uint8_t>(given[in]);
            if (size == 0 || in + 1 + size > given.size()) break;
            stated = static_cast<std::uint8_t>(given[in + 1]) == tag;
            in += 1 + size;
        }
        if (!stated) out.append(kept.substr(at, 1 + length));
        at += 1 + length;
    }
    return out;
}

// The bytes a message's fixed part takes: all of it, less the tail a variable-length one reserves.
template <typename Message>
constexpr std::size_t fixed_size() {
    if constexpr (requires { Message::tail_capacity; }) return sizeof(Message) - Message::tail_capacity;
    else return sizeof(Message);
}

// An order the gateway admitted: its id, the client's reference to it, the shares still open,
// and the Account that owns it - its port's, whichever connection entered it.
struct Order {
    std::uint64_t id = 0;
    std::uint64_t reference = 0;
    std::uint64_t remaining = 0;
    std::size_t account = 0;
    std::uint64_t channel = 0;   // the order flow channel it was entered on, within which its reference names it
    bool traded = false;   // has it traded, so that once closed it is kept for a broken trade's answer
    protocol::enter_order_message::fields_type entered{};   // what the client entered it with, which a response restates
    protocol::enter_order_message request{};   // the request that admitted it, answered once the engine has it
    std::string options{};   // the options it was entered with, as they arrived, less those a replace changed: what a response echoes
    std::string firm{};   // the firm its client was admitted under, which the engine is told
    char side = 0;   // the side the engine was told it is on, as the Ipc says it
    char display = 0;   // whether the engine was told to show it, as the Ipc codes it
    std::int64_t price = 0;   // the price the engine took it at, as its acceptance says: the venue may reprice it on entry
    std::uint64_t executed = 0;   // the shares it has executed, which an intended size counts
    char pending = 0;   // the code of the request last acting on it, which tells its answers apart
    std::uint32_t told = 0;   // the responses told at most once of it, a bit each, those told set
    std::string client{};   // the username of the client that entered it, whose risk controls count it
    long double valued = 0;   // the price its open shares are valued at by its client's exposure: the Ipc's, or a market order's at its security's declared price
    std::uint64_t replaced_id = 0;   // the order a replacement replaces, its number; 0 for an order entered
    std::uint64_t replaced_reference{};   // and the reference it was known by
    protocol::replace_order_message replace{};   // the replace that gave it, answered once the engine has it
};

// The size an intended order size leaves open. OUCH 5.0's is "the maximum number of shares
// that can potentially be executed in total", so it counts what the order has executed; what
// it leaves open is that less those, and never more than the order has open now. It is an
// approximation, to be revisited: it counts the executions the gateway has heard of, so one the
// engine makes after the request and before the request reaches it is not counted. Being exact
// needs the engine to count what each order has executed and work this out itself.
inline std::uint64_t intended(std::uint64_t size, const Order& order) {
    const std::uint64_t open = size > order.executed ? size - order.executed : 0;
    return open < order.remaining ? open : order.remaining;
}

// The size a replace leaves open. OUCH 5.0's is the "Total number of shares liable, inclusive
// of previous executions" on the order chain, which its replacement carries on; a replace may
// raise it, so what it leaves open is that less those alone.
inline std::uint64_t liable(std::uint64_t size, const Order& order) {
    return size > order.executed ? size - order.executed : 0;
}

// enter_order_message -> new_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_enter_order)
inline ipc_ns::new_order_message to_ouch_enter_order(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::new_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.late_limit.set(static_cast<value_of<decltype(out.late_limit)>>('R'));
    out.extended_close.set(static_cast<value_of<decltype(out.extended_close)>>('N'));
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case 'T': return 'S'; case 'E': return 'S'; default: return value; } }(static_cast<char>(m.fields.side.get().value()))));
    out.quantity.set(m.fields.quantity.get().value());
    out.symbol.set(m.fields.symbol.get().value());
    out.price.set(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000 ? std::nullopt : std::make_optional<value_of<decltype(out.price)>>(static_cast<value_of<decltype(out.price)>>(m.fields.price.get().value())));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.expire_time.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 15, 4).value_or(0));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 6, 1).value_or(static_cast<std::uint64_t>('L'))))));
    out.peg_offset.set(static_cast<std::int32_t>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 7, 4).value_or(0)));
    out.min_qty.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 3, 4).value_or(0));
    out.post_only.set(static_cast<value_of<decltype(out.post_only)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 12, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.imbalance_only.set(static_cast<value_of<decltype(out.imbalance_only)>>([](char value) { switch (value) { case 'I': return 'Y'; case 'O': return 'N'; case 'T': return 'N'; case 'Q': return 'N'; case 'B': return 'N'; case 'D': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 17, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.extended_life_book.set(static_cast<value_of<decltype(out.extended_life_book)>>([](char value) { switch (value) { case 'I': return 'N'; case 'O': return 'N'; case 'T': return 'N'; case 'Q': return 'N'; case 'B': return 'B'; case 'D': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 17, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.trade_now.set(static_cast<value_of<decltype(out.trade_now)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 16, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.max_floor.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 5, 4).value_or(0));
    out.random_reserve.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 13, 4).value_or(0));
    out.discretion_price.set(static_cast<value_of<decltype(out.discretion_price)>>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 9, 8).value_or(0)));
    out.discretion_type.set(static_cast<value_of<decltype(out.discretion_type)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 10, 1).value_or(static_cast<std::uint64_t>('L'))))));
    out.short_sale.set(static_cast<value_of<decltype(out.short_sale)>>(static_cast<char>(m.fields.side.get().value())));
    out.display.set(static_cast<value_of<decltype(out.display)>>([](char value) { switch (value) { case 'Z': return 'Y'; default: return value; } }(static_cast<char>(m.fields.display.get().value()))));
    { const std::optional<std::uint64_t> held = option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 2, 4); std::string text; if (held) { for (int at = 3; at >= 0; --at) text.push_back(static_cast<char>((*held >> (8 * at)) & 0xff)); } if (text.find_first_not_of(' ') == std::string::npos) text = order.firm.substr(0, 4); out.attribution.set(std::string_view{text}); }
    out.cross_type.set(static_cast<value_of<decltype(out.cross_type)>>(static_cast<char>(m.fields.cross_type.get().value())));
    out.self_match_strategy.set(static_cast<value_of<decltype(out.self_match_strategy)>>([](char value) { switch (value) { case 'Y': return 'D'; case 'D': return 'D'; case 'O': return 'O'; case 'W': return 'W'; case 'R': return 'R'; case 'y': return 'D'; case 'd': return 'D'; case 'o': return 'O'; case 'w': return 'W'; case 'r': return 'R'; case 'Z': return 'D'; case 'E': return 'D'; case 'P': return 'O'; case 'X': return 'W'; case 'S': return 'R'; case '0': return 'D'; case '1': return 'D'; case '2': return 'O'; case '4': return 'W'; case '5': return 'R'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.self_match_level.set(static_cast<value_of<decltype(out.self_match_level)>>([](char value) { switch (value) { case 'Y': return 'F'; case 'D': return 'F'; case 'O': return 'F'; case 'W': return 'F'; case 'R': return 'F'; case 'y': return 'O'; case 'd': return 'O'; case 'o': return 'O'; case 'w': return 'O'; case 'r': return 'O'; case 'Z': return 'A'; case 'E': return 'A'; case 'P': return 'A'; case 'X': return 'A'; case 'S': return 'A'; case '0': return 'M'; case '1': return 'M'; case '2': return 'M'; case '4': return 'M'; case '5': return 'M'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.aiq_strategy.set(static_cast<value_of<decltype(out.aiq_strategy)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>([&]() -> std::uint64_t { std::uint64_t key = 14695981039346656037ull; const auto mix = [&key](std::string_view text) { for (const char c : text) { key ^= static_cast<unsigned char>(c); key *= 1099511628211ull; } key ^= 0x1f; key *= 1099511628211ull; }; mix(std::to_string(order.account)); { const std::optional<std::uint64_t> held = option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 2, 4); mix(held ? std::to_string(*held) : std::string{}); } { const std::optional<std::uint64_t> held = option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 30, 2); mix(held ? std::to_string(*held) : std::string{}); } return key == 0 ? 1 : key; }()));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// cancel_order_message -> cancel_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_cancel_order)
inline ipc_ns::cancel_order_message to_ouch_cancel_order(const protocol::cancel_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.symbol.set(order.entered.symbol.get().value());
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// cancel_order_message -> reduce_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_reduce_order)
inline ipc_ns::reduce_order_message to_ouch_reduce_order(const protocol::cancel_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::reduce_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.symbol.set(order.entered.symbol.get().value());
    out.quantity.set(intended(static_cast<std::uint64_t>(m.fields.quantity.get().value()), order));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// modify_order_message -> reduce_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_modify_order)
inline ipc_ns::reduce_order_message to_ouch_modify_order(const protocol::modify_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::reduce_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.symbol.set(order.entered.symbol.get().value());
    out.quantity.set(intended(static_cast<std::uint64_t>(m.fields.quantity.get().value()), order));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// replace_order_message -> replace_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_replace_order)
inline ipc_ns::replace_order_message to_ouch_replace_order(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::replace_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.replaced_id));
    out.replacement_id.set(static_cast<value_of<decltype(out.replacement_id)>>(order.id));
    out.symbol.set(order.entered.symbol.get().value());
    out.side.set(static_cast<value_of<decltype(out.side)>>(order.side));
    out.short_sale.set(static_cast<value_of<decltype(out.short_sale)>>(static_cast<char>(order.entered.side.get().value())));
    out.display.set(static_cast<value_of<decltype(out.display)>>(order.display));
    out.cross_type.set(static_cast<value_of<decltype(out.cross_type)>>(static_cast<char>(order.entered.cross_type.get().value())));
    out.extended_close.set(static_cast<value_of<decltype(out.extended_close)>>('N'));
    out.quantity.set(liable(static_cast<std::uint64_t>(m.fields.quantity.get().value()), order));
    out.price.set(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000 ? std::nullopt : std::make_optional<value_of<decltype(out.price)>>(static_cast<value_of<decltype(out.price)>>(m.fields.price.get().value())));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 6, 1).value_or(static_cast<std::uint64_t>('L'))))));
    out.peg_offset.set(static_cast<std::int32_t>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 7, 4).value_or(0)));
    out.min_qty.set(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 3, 4).value_or(0));
    out.post_only.set(static_cast<value_of<decltype(out.post_only)>>(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 12, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.trade_now.set(static_cast<value_of<decltype(out.trade_now)>>(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 16, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.max_floor.set(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 5, 4).value_or(0));
    out.random_reserve.set(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 13, 4).value_or(0));
    out.discretion_price.set(static_cast<value_of<decltype(out.discretion_price)>>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 9, 8).value_or(0)));
    out.discretion_type.set(static_cast<value_of<decltype(out.discretion_type)>>(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 10, 1).value_or(static_cast<std::uint64_t>('L'))))));
    { const std::optional<std::uint64_t> held = option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 2, 4); std::string text; if (held) { for (int at = 3; at >= 0; --at) text.push_back(static_cast<char>((*held >> (8 * at)) & 0xff)); } out.attribution.set(std::string_view{text}); }
    out.imbalance_only.set(static_cast<value_of<decltype(out.imbalance_only)>>([](char value) { switch (value) { case 'I': return 'Y'; case 'O': return 'N'; case 'T': return 'N'; case 'Q': return 'N'; case 'B': return 'N'; case 'D': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 17, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.extended_life_book.set(static_cast<value_of<decltype(out.extended_life_book)>>([](char value) { switch (value) { case 'I': return 'N'; case 'O': return 'N'; case 'T': return 'N'; case 'Q': return 'N'; case 'B': return 'B'; case 'D': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 17, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.self_match_strategy.set(static_cast<value_of<decltype(out.self_match_strategy)>>([](char value) { switch (value) { case 'Y': return 'D'; case 'D': return 'D'; case 'O': return 'O'; case 'W': return 'W'; case 'R': return 'R'; case 'y': return 'D'; case 'd': return 'D'; case 'o': return 'O'; case 'w': return 'W'; case 'r': return 'R'; case 'Z': return 'D'; case 'E': return 'D'; case 'P': return 'O'; case 'X': return 'W'; case 'S': return 'R'; case '0': return 'D'; case '1': return 'D'; case '2': return 'O'; case '4': return 'W'; case '5': return 'R'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.self_match_level.set(static_cast<value_of<decltype(out.self_match_level)>>([](char value) { switch (value) { case 'Y': return 'F'; case 'D': return 'F'; case 'O': return 'F'; case 'W': return 'F'; case 'R': return 'F'; case 'y': return 'O'; case 'd': return 'O'; case 'o': return 'O'; case 'w': return 'O'; case 'r': return 'O'; case 'Z': return 'A'; case 'E': return 'A'; case 'P': return 'A'; case 'X': return 'A'; case 'S': return 'A'; case '0': return 'M'; case '1': return 'M'; case '2': return 'M'; case '4': return 'M'; case '5': return 'M'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.aiq_strategy.set(static_cast<value_of<decltype(out.aiq_strategy)>>(static_cast<char>(static_cast<char>(option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 29, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>([&]() -> std::uint64_t { std::uint64_t key = 14695981039346656037ull; const auto mix = [&key](std::string_view text) { for (const char c : text) { key ^= static_cast<unsigned char>(c); key *= 1099511628211ull; } key ^= 0x1f; key *= 1099511628211ull; }; mix(std::to_string(order.account)); { const std::optional<std::uint64_t> held = option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 2, 4); mix(held ? std::to_string(*held) : std::string{}); } { const std::optional<std::uint64_t> held = option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 30, 2); mix(held ? std::to_string(*held) : std::string{}); } return key == 0 ? 1 : key; }()));
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// mass_cancel_request_message -> cancel_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_mass_cancel)
inline ipc_ns::cancel_order_message to_ouch_mass_cancel(const protocol::mass_cancel_request_message& m, const Order& order) {
    (void)m;
    (void)order;
    ipc_ns::cancel_order_message out{};
    out.order_id.set(static_cast<value_of<decltype(out.order_id)>>(order.id));
    out.symbol.set(order.entered.symbol.get().value());
    out.timestamp.set(static_cast<value_of<decltype(out.timestamp)>>(send_time_nanos()));
    return out;
}

// mass_cancel_request_message -> mass_cancel_response_message  (NsmEquities.Ipc.Transforms.xml: ouch_mass_canceled)
inline protocol::mass_cancel_response_message to_ouch_mass_canceled(const protocol::mass_cancel_request_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::mass_cancel_response_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.firm.set(m.fields.firm.get().value());
    out.fields.symbol.set(m.fields.symbol.get().value());
    return out;
}

// disable_order_entry_request_message -> disable_order_entry_response_message  (NsmEquities.Ipc.Transforms.xml: ouch_disabled)
inline protocol::disable_order_entry_response_message to_ouch_disabled(const protocol::disable_order_entry_request_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::disable_order_entry_response_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.firm.set(m.fields.firm.get().value());
    return out;
}

// enable_order_entry_request_message -> enable_order_entry_response_message  (NsmEquities.Ipc.Transforms.xml: ouch_enabled)
inline protocol::enable_order_entry_response_message to_ouch_enabled(const protocol::enable_order_entry_request_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::enable_order_entry_response_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.firm.set(m.fields.firm.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_disabled)
inline protocol::rejected_order_message to_ouch_rejected_disabled(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(12));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// account_query_message -> account_query_response_message  (NsmEquities.Ipc.Transforms.xml: ouch_account_query)
inline protocol::account_query_response_message to_ouch_account_query(const protocol::account_query_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::account_query_response_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.next_user_ref_num.set(static_cast<value_of<decltype(out.fields.next_user_ref_num)>>(order.reference));
    return out;
}

// phase_announced_message -> system_event_message  (NsmEquities.Ipc.Transforms.xml: ouch_start_of_day)
inline protocol::system_event_message to_ouch_start_of_day(const ipc_ns::phase_announced_message& m) {
    (void)m;
    protocol::system_event_message out{};
    out.fields.event_code.set(static_cast<value_of<decltype(out.fields.event_code)>>('S'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// phase_announced_message -> system_event_message  (NsmEquities.Ipc.Transforms.xml: ouch_end_of_day)
inline protocol::system_event_message to_ouch_end_of_day(const ipc_ns::phase_announced_message& m) {
    (void)m;
    protocol::system_event_message out{};
    out.fields.event_code.set(static_cast<value_of<decltype(out.fields.event_code)>>('E'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// enter_order_message -> order_accepted_message  (NsmEquities.Ipc.Transforms.xml: ouch_order_accepted)
inline protocol::order_accepted_message to_ouch_order_accepted(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_accepted_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(order.price));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(order.id));
    out.fields.order_state.set(static_cast<value_of<decltype(out.fields.order_state)>>('L'));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(static_cast<char>(m.fields.side.get().value())));
    out.fields.quantity.set(m.fields.quantity.get().value());
    out.fields.symbol.set(m.fields.symbol.get().value());
    out.fields.time_in_force.set(static_cast<value_of<decltype(out.fields.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.fields.display.set(static_cast<value_of<decltype(out.fields.display)>>(static_cast<char>(m.fields.display.get().value())));
    out.fields.capacity.set(static_cast<value_of<decltype(out.fields.capacity)>>(static_cast<char>(m.fields.capacity.get().value())));
    out.fields.inter_market_sweep_eligibility.set(static_cast<value_of<decltype(out.fields.inter_market_sweep_eligibility)>>(static_cast<char>(m.fields.inter_market_sweep_eligibility.get().value())));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>(static_cast<char>(m.fields.cross_type.get().value())));
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> replaced_message  (NsmEquities.Ipc.Transforms.xml: ouch_replaced)
inline protocol::replaced_message to_ouch_replaced(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::replaced_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(static_cast<char>(order.entered.side.get().value())));
    out.fields.symbol.set(order.entered.symbol.get().value());
    out.fields.display.set(static_cast<value_of<decltype(out.fields.display)>>(static_cast<char>(order.entered.display.get().value())));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(order.id));
    out.fields.capacity.set(static_cast<value_of<decltype(out.fields.capacity)>>(static_cast<char>(order.entered.capacity.get().value())));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>(static_cast<char>(order.entered.cross_type.get().value())));
    out.fields.order_state.set(static_cast<value_of<decltype(out.fields.order_state)>>('L'));
    out.fields.orig_user_ref_num.set(m.fields.orig_user_ref_num.get().value());
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.quantity.set(liable(static_cast<std::uint64_t>(m.fields.quantity.get().value()), order));
    out.fields.price.set(m.fields.price.get().value());
    out.fields.time_in_force.set(static_cast<value_of<decltype(out.fields.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.fields.inter_market_sweep_eligibility.set(static_cast<value_of<decltype(out.fields.inter_market_sweep_eligibility)>>(static_cast<char>(m.fields.inter_market_sweep_eligibility.get().value())));
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// modify_order_message -> order_modified_message  (NsmEquities.Ipc.Transforms.xml: ouch_modified)
inline protocol::order_modified_message to_ouch_modified(const protocol::modify_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_modified_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(static_cast<char>(m.fields.side.get().value())));
    out.fields.quantity.set(intended(static_cast<std::uint64_t>(m.fields.quantity.get().value()), order));
    return out;
}

// enter_order_message -> order_accepted_message  (NsmEquities.Ipc.Transforms.xml: ouch_order_accepted_closed)
inline protocol::order_accepted_message to_ouch_order_accepted_closed(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_accepted_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(order.id));
    out.fields.order_state.set(static_cast<value_of<decltype(out.fields.order_state)>>('L'));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(static_cast<char>(m.fields.side.get().value())));
    out.fields.quantity.set(m.fields.quantity.get().value());
    out.fields.symbol.set(m.fields.symbol.get().value());
    out.fields.price.set(m.fields.price.get().value());
    out.fields.time_in_force.set(static_cast<value_of<decltype(out.fields.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.fields.display.set(static_cast<value_of<decltype(out.fields.display)>>(static_cast<char>(m.fields.display.get().value())));
    out.fields.capacity.set(static_cast<value_of<decltype(out.fields.capacity)>>(static_cast<char>(m.fields.capacity.get().value())));
    out.fields.inter_market_sweep_eligibility.set(static_cast<value_of<decltype(out.fields.inter_market_sweep_eligibility)>>(static_cast<char>(m.fields.inter_market_sweep_eligibility.get().value())));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>(static_cast<char>(m.fields.cross_type.get().value())));
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> order_accepted_message  (NsmEquities.Ipc.Transforms.xml: ouch_order_accepted_slid)
inline protocol::order_accepted_message to_ouch_order_accepted_slid(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_accepted_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.order_reference_number.set(static_cast<value_of<decltype(out.fields.order_reference_number)>>(order.id));
    out.fields.order_state.set(static_cast<value_of<decltype(out.fields.order_state)>>('L'));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(static_cast<char>(m.fields.side.get().value())));
    out.fields.quantity.set(m.fields.quantity.get().value());
    out.fields.symbol.set(m.fields.symbol.get().value());
    out.fields.price.set(m.fields.price.get().value());
    out.fields.time_in_force.set(static_cast<value_of<decltype(out.fields.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.fields.display.set(static_cast<value_of<decltype(out.fields.display)>>(static_cast<char>(m.fields.display.get().value())));
    out.fields.capacity.set(static_cast<value_of<decltype(out.fields.capacity)>>(static_cast<char>(m.fields.capacity.get().value())));
    out.fields.inter_market_sweep_eligibility.set(static_cast<value_of<decltype(out.fields.inter_market_sweep_eligibility)>>(static_cast<char>(m.fields.inter_market_sweep_eligibility.get().value())));
    out.fields.cross_type.set(static_cast<value_of<decltype(out.fields.cross_type)>>(static_cast<char>(m.fields.cross_type.get().value())));
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_resting)
inline protocol::order_executed_message to_ouch_executed_resting(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value_or(2147483647)));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'A'; case 'A': return 'i'; case 'E': return 'n'; case 'I': return 'H'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_resting_retail)
inline protocol::order_executed_message to_ouch_executed_resting_retail(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value_or(2147483647)));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'e'; case 'A': return 'i'; case 'E': return 'n'; case 'I': return 'H'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_hidden)
inline protocol::order_executed_message to_ouch_executed_hidden(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value_or(2147483647)));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'J'; case 'A': return 'i'; case 'E': return 'n'; case 'I': return 'H'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_midpoint)
inline protocol::order_executed_message to_ouch_executed_midpoint(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value_or(2147483647)));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'k'; case 'A': return 'i'; case 'E': return 'n'; case 'I': return 'H'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_aggressor)
inline protocol::order_executed_message to_ouch_executed_aggressor(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value_or(2147483647)));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'R'; case 'A': return 'i'; case 'E': return 'n'; case 'M': return 'O'; case 'L': return 'C'; case 'I': return 'H'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_aggressor_midpoint)
inline protocol::order_executed_message to_ouch_executed_aggressor_midpoint(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value_or(2147483647)));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'm'; case 'A': return 'i'; case 'E': return 'n'; case 'I': return 'H'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
    out.fields.match_number.set(m.execution_id.get().value());
    return out;
}

// trade_broken_message -> broken_trade_message  (NsmEquities.Ipc.Transforms.xml: ouch_broken_resting)
inline protocol::broken_trade_message to_ouch_broken_resting(const ipc_ns::trade_broken_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::broken_trade_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.match_number.set(m.execution_id.get().value());
    out.fields.broken_trade_reason.set(static_cast<value_of<decltype(out.fields.broken_trade_reason)>>(static_cast<char>(m.break_reason.get().value())));
    return out;
}

// trade_broken_message -> broken_trade_message  (NsmEquities.Ipc.Transforms.xml: ouch_broken_aggressor)
inline protocol::broken_trade_message to_ouch_broken_aggressor(const ipc_ns::trade_broken_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::broken_trade_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.match_number.set(m.execution_id.get().value());
    out.fields.broken_trade_reason.set(static_cast<value_of<decltype(out.fields.broken_trade_reason)>>(static_cast<char>(m.break_reason.get().value())));
    return out;
}

// order_rested_message -> order_restated_message  (NsmEquities.Ipc.Transforms.xml: ouch_restated)
inline protocol::order_restated_message to_ouch_restated(const ipc_ns::order_rested_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_restated_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.order_restated_reason.set(static_cast<value_of<decltype(out.fields.order_restated_reason)>>('R'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_rested_message -> order_priority_update_message  (NsmEquities.Ipc.Transforms.xml: ouch_priority_update)
inline protocol::order_priority_update_message to_ouch_priority_update(const ipc_ns::order_rested_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_priority_update_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value_or(2147483647)));
    out.fields.display.set(static_cast<value_of<decltype(out.fields.display)>>(static_cast<char>(m.display.get().value())));
    out.fields.order_reference_number.set(m.order_id.get().value());
    return out;
}

// cancel_pending_message -> cancel_pending_message  (NsmEquities.Ipc.Transforms.xml: ouch_cancel_pending)
inline protocol::cancel_pending_message to_ouch_cancel_pending(const ipc_ns::cancel_pending_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::cancel_pending_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// reduce_refused_message -> cancel_reject_message  (NsmEquities.Ipc.Transforms.xml: ouch_cancel_reject)
inline protocol::cancel_reject_message to_ouch_cancel_reject(const ipc_ns::reduce_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::cancel_reject_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_canceled_message -> aiq_canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_aiq_canceled)
inline protocol::aiq_canceled_message to_ouch_aiq_canceled(const ipc_ns::order_canceled_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::aiq_canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.decrement_shares.set(static_cast<value_of<decltype(out.fields.decrement_shares)>>(order.remaining));
    out.fields.order_cancel_reason.set(static_cast<value_of<decltype(out.fields.order_cancel_reason)>>('Q'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity_prevented_from_trading.set(m.prevented_quantity.get().value());
    out.fields.execution_price.set(static_cast<value_of<decltype(out.fields.execution_price)>>(m.prevented_price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>(static_cast<char>(m.prevented_liquidity.get().value())));
    out.fields.aiq_strategy.set(static_cast<value_of<decltype(out.fields.aiq_strategy)>>(static_cast<char>(m.prevented_strategy.get().value())));
    return out;
}

// order_reduced_message -> aiq_canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_aiq_reduced)
inline protocol::aiq_canceled_message to_ouch_aiq_reduced(const ipc_ns::order_reduced_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::aiq_canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.order_cancel_reason.set(static_cast<value_of<decltype(out.fields.order_cancel_reason)>>('Q'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.decrement_shares.set(m.quantity.get().value());
    out.fields.quantity_prevented_from_trading.set(m.prevented_quantity.get().value());
    out.fields.execution_price.set(static_cast<value_of<decltype(out.fields.execution_price)>>(m.prevented_price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>(static_cast<char>(m.prevented_liquidity.get().value())));
    out.fields.aiq_strategy.set(static_cast<value_of<decltype(out.fields.aiq_strategy)>>(static_cast<char>(m.prevented_strategy.get().value())));
    return out;
}

// order_canceled_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_canceled)
inline protocol::canceled_message to_ouch_canceled(const ipc_ns::order_canceled_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(order.remaining));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('U'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_canceled_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_canceled_protected)
inline protocol::canceled_message to_ouch_canceled_protected(const ipc_ns::order_canceled_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(order.remaining));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('X'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_reduced_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_reduced)
inline protocol::canceled_message to_ouch_reduced(const ipc_ns::order_reduced_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('U'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    return out;
}

// order_eliminated_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_eliminated_halted)
inline protocol::canceled_message to_ouch_eliminated_halted(const ipc_ns::order_eliminated_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('H'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    return out;
}

// order_eliminated_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_eliminated)
inline protocol::canceled_message to_ouch_eliminated(const ipc_ns::order_eliminated_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('I'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    return out;
}

// order_eliminated_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_eliminated_market)
inline protocol::canceled_message to_ouch_eliminated_market(const ipc_ns::order_eliminated_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('Z'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    return out;
}

// order_eliminated_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_expired)
inline protocol::canceled_message to_ouch_expired(const ipc_ns::order_eliminated_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('T'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    return out;
}

// order_eliminated_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_expired_extended)
inline protocol::canceled_message to_ouch_expired_extended(const ipc_ns::order_eliminated_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('T'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    return out;
}

// order_eliminated_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_expired_timed)
inline protocol::canceled_message to_ouch_expired_timed(const ipc_ns::order_eliminated_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('T'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    return out;
}

// order_refused_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_refused)
inline protocol::canceled_message to_ouch_refused(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(order.remaining));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('E'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_slid)
inline protocol::canceled_message to_ouch_slid(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(order.remaining));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('G'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_quantity)
inline protocol::rejected_order_message to_ouch_rejected_quantity(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(19));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_price)
inline protocol::rejected_order_message to_ouch_rejected_price(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(29));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_min_qty)
inline protocol::rejected_order_message to_ouch_rejected_min_qty(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(13));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_max_floor)
inline protocol::rejected_order_message to_ouch_rejected_max_floor(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(4));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_side)
inline protocol::rejected_order_message to_ouch_rejected_side(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(9));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_aiq)
inline protocol::rejected_order_message to_ouch_rejected_aiq(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(64));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_display)
inline protocol::rejected_order_message to_ouch_rejected_display(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(3));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_symbol)
inline protocol::rejected_order_message to_ouch_rejected_symbol(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(23));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_share_size)
inline protocol::rejected_order_message to_ouch_risk_share_size(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(49));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_share_size_replace)
inline protocol::rejected_order_message to_ouch_risk_share_size_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(49));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_notional)
inline protocol::rejected_order_message to_ouch_risk_notional(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(48));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_notional_unpriced)
inline protocol::rejected_order_message to_ouch_risk_notional_unpriced(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(48));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_notional_replace)
inline protocol::rejected_order_message to_ouch_risk_notional_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(48));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_notional_unpriced_replace)
inline protocol::rejected_order_message to_ouch_risk_notional_unpriced_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(48));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_restricted)
inline protocol::rejected_order_message to_ouch_risk_restricted(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(34));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_hard_to_borrow)
inline protocol::rejected_order_message to_ouch_risk_hard_to_borrow(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(35));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_pre_market)
inline protocol::rejected_order_message to_ouch_risk_pre_market(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(45));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_post_market)
inline protocol::rejected_order_message to_ouch_risk_post_market(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(46));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_after_hours)
inline protocol::rejected_order_message to_ouch_risk_after_hours(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(46));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_pre_market_replace)
inline protocol::rejected_order_message to_ouch_risk_pre_market_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(45));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_post_market_replace)
inline protocol::rejected_order_message to_ouch_risk_post_market_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(46));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_after_hours_replace)
inline protocol::rejected_order_message to_ouch_risk_after_hours_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(46));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_short_sale)
inline protocol::rejected_order_message to_ouch_risk_short_sale(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(43));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_short_sale_exempt)
inline protocol::rejected_order_message to_ouch_risk_short_sale_exempt(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(47));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_intermarket_sweep)
inline protocol::rejected_order_message to_ouch_risk_intermarket_sweep(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(36));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_intermarket_sweep_replace)
inline protocol::rejected_order_message to_ouch_risk_intermarket_sweep_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(36));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_ipo_market_buy)
inline protocol::rejected_order_message to_ouch_risk_ipo_market_buy(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(51));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_market_impact_buy)
inline protocol::rejected_order_message to_ouch_risk_market_impact_buy(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(33));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_market_impact_sell)
inline protocol::rejected_order_message to_ouch_risk_market_impact_sell(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(33));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_market_impact_buy_replace)
inline protocol::rejected_order_message to_ouch_risk_market_impact_buy_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(33));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_market_impact_sell_replace)
inline protocol::rejected_order_message to_ouch_risk_market_impact_sell_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(33));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_average_daily_volume)
inline protocol::rejected_order_message to_ouch_risk_average_daily_volume(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(37));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_average_daily_volume_replace)
inline protocol::rejected_order_message to_ouch_risk_average_daily_volume_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(37));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_port_rate)
inline protocol::rejected_order_message to_ouch_risk_port_rate(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(41));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_port_rate_replace)
inline protocol::rejected_order_message to_ouch_risk_port_rate_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(41));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_symbol_rate)
inline protocol::rejected_order_message to_ouch_risk_symbol_rate(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(40));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_symbol_rate_replace)
inline protocol::rejected_order_message to_ouch_risk_symbol_rate_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(40));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_duplicate)
inline protocol::rejected_order_message to_ouch_risk_duplicate(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(42));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// enter_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_stopped)
inline protocol::rejected_order_message to_ouch_risk_stopped(const protocol::enter_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(32));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// replace_order_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_stopped_replace)
inline protocol::rejected_order_message to_ouch_risk_stopped_replace(const protocol::replace_order_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.timestamp.set(static_cast<value_of<decltype(out.fields.timestamp)>>(nanoseconds_since_midnight(static_cast<std::uint64_t>(send_time_nanos()), new_york())));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(32));
    out.fields.user_ref_num.set(m.fields.user_ref_num.get().value());
    out.fields.clordid.set(m.fields.clordid.get().value());
    return out;
}

// order_canceled_message -> canceled_message  (NsmEquities.Ipc.Transforms.xml: ouch_risk_canceled)
inline protocol::canceled_message to_ouch_risk_canceled(const ipc_ns::order_canceled_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::canceled_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.quantity.set(static_cast<value_of<decltype(out.fields.quantity)>>(order.remaining));
    out.fields.cancel_order_reason.set(static_cast<value_of<decltype(out.fields.cancel_order_reason)>>('Z'));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_replace_rejected)
inline protocol::rejected_order_message to_ouch_replace_rejected(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(21));
    out.fields.clordid.set(order.replace.fields.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected)
inline protocol::rejected_order_message to_ouch_rejected(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(15));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_peg)
inline protocol::rejected_order_message to_ouch_rejected_peg(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(5));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_type)
inline protocol::rejected_order_message to_ouch_rejected_type(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(44));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_cross)
inline protocol::rejected_order_message to_ouch_rejected_cross(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(20));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_unreferenced)
inline protocol::rejected_order_message to_ouch_rejected_unreferenced(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(14));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_aggressive)
inline protocol::rejected_order_message to_ouch_rejected_aggressive(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(25));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_fat_finger)
inline protocol::rejected_order_message to_ouch_rejected_fat_finger(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(38));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// order_refused_message -> rejected_order_message  (NsmEquities.Ipc.Transforms.xml: ouch_rejected_limit_protection)
inline protocol::rejected_order_message to_ouch_rejected_limit_protection(const ipc_ns::order_refused_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::rejected_order_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.rejected_order_reason.set(static_cast<value_of<decltype(out.fields.rejected_order_reason)>>(6));
    out.fields.clordid.set(order.entered.clordid.get().value());
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    return out;
}

// The order-entry gateway: SoupBin Tcp clients in, the Ipc to the engine out, and the engine's
// events back to the client each order belongs to. `Emit` is a callable(const char*, std::size_t)
// taking framed Ipc messages to the engine.
template <typename Emit>
class Gateway {
public:
    using Send = std::function<void(const char*, std::size_t)>;

    // One of the venue's gateways, a gateway for each trader. Each numbers the orders it admits
    // in an interleave of its own - the k-th of n gateways k, k + n, k + 2n ... - so no two of them
    // give one number, and none has to ask another what it gave. The simulator's own scheme:
    // Nasdaq's feeds show orders numbered by the engine as they arrive, not by any gateway.
    // A gateway on its own - a test's - numbers from one, as the one gateway does.
    // Its place in that interleave is its port's too, and so names the Account the port is bound to.
    explicit Gateway(Emit emit, std::uint64_t first = 1, std::uint64_t step = 1) : emit_(emit), account_(first), next_id_(first), step_(step) {}

    // A login the gateway accepts; with none registered, any login is accepted.
    void allow(std::string username, std::string password) { credentials_[std::move(username)] = std::move(password); }

    // A new client connection, writing through `send`; returns the session's own number.
    // 
    // Numbered once and never again: the loop knows a connection by it, and a number handed out
    // twice would close one client's connection for another's. Its orders are not the
    // connection's: every connection on the port is the port's one Account.
    std::size_t connect(Send send) {
        const auto client = next_client_++;
        clients_.emplace(client, std::make_unique<Client>(*this, client, std::move(send)));
        return client;
    }

    // A client that has gone: its session is over, and what it held is let go. Whatever it
    // left resting stays in the book - a venue does not cancel an order because a line
    // dropped - and stays the Account's: answers about it reach whichever connections are
    // logged in on the port, none while none is, and a client that logs in again finds it.
    // Unless the client it logged in as declares a control breached by a session ending other than
    // by its logout: what that breach cancels is canceled.
    void disconnect(std::size_t client) {
        if (const auto held = clients_.find(client); held != clients_.end() && held->second->risk != nullptr && !held->second->logged_out) disconnected(*held->second->risk);
        clients_.erase(client);
    }

    void on_bytes(std::size_t client, const char* data, std::size_t length) {
        if (const auto held = clients_.find(client); held != clients_.end()) {
            held->second->session.on_bytes(data, length);
        }
    }

    std::size_t open_orders() const { return orders_.size(); }

    // The time: every session fires what is due by it - its heartbeats, and the end of a client
    // gone silent. The loop calls it.
    void tick() { for (auto& held : clients_) held.second->session.tick(); }

    // Has a client's session ended - gone silent past what the session allows? Its connection is
    // then the loop's to close, or, for a link that stays, to open afresh.
    bool ended(std::size_t client) const {
        const auto held = clients_.find(client);
        return held == clients_.end() || held->second->lost;
    }

    // The engine's phase_announced_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::phase_announced_message& m) {
        if (const auto heard = m.announcement.get().value(); static_cast<char>(heard) == 'O' || static_cast<char>(heard) == 'S' || static_cast<char>(heard) == 'Q' || static_cast<char>(heard) == 'M' || static_cast<char>(heard) == 'E' || static_cast<char>(heard) == 'C') heard_phase_announced_message_announcement_ = static_cast<std::int64_t>(heard);   // the latest it said of those the order entry keeps, which a client risk control reads
        if (static_cast<char>(m.announcement.get().value()) == 'S' && m.symbol.get_trimmed().value() == "ZVZZT") respond(to_ouch_start_of_day(m));
        if (static_cast<char>(m.announcement.get().value()) == 'C' && m.symbol.get_trimmed().value() == "ZXZZT") { respond(to_ouch_end_of_day(m)); for (const auto& held : clients_) held.second->session.end_of_session(); }
    }

    // The engine's trade_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::trade_message& m) {
        if (static_cast<char>(m.resting_display.get().value()) != 'N') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end() && static_cast<char>((option(reinterpret_cast<const std::byte*>(found->second.options.data()), reinterpret_cast<const std::byte*>(found->second.options.data()) + found->second.options.size(), 4, 1)).value_or(0)) != 'R') respond(to_ouch_executed_resting(m, found->second));
        if (static_cast<char>(m.resting_display.get().value()) != 'N') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end() && static_cast<char>((option(reinterpret_cast<const std::byte*>(found->second.options.data()), reinterpret_cast<const std::byte*>(found->second.options.data()) + found->second.options.size(), 4, 1)).value_or(0)) == 'R') respond(to_ouch_executed_resting_retail(m, found->second));
        if (static_cast<char>(m.resting_display.get().value()) == 'N' && static_cast<char>(m.resting_midpoint.get().value()) != 'Y') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) respond(to_ouch_executed_hidden(m, found->second));
        if (static_cast<char>(m.resting_midpoint.get().value()) == 'Y') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) respond(to_ouch_executed_midpoint(m, found->second));
        if (static_cast<char>(m.resting_midpoint.get().value()) != 'Y') if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) respond(to_ouch_executed_aggressor(m, found->second));
        if (static_cast<char>(m.resting_midpoint.get().value()) == 'Y') if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) respond(to_ouch_executed_aggressor_midpoint(m, found->second));
        if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) found->second.executed += m.quantity.get().value();
        { Risk* exposed_by = exposed(m.resting_order_id.get().value(), m.quantity.get().value(), static_cast<long double>(m.price.get().value_or(0))); fill(m.resting_order_id.get().value(), m.quantity.get().value()); if (exposed_by != nullptr) broken(*exposed_by, 0, 0); }
        if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) found->second.executed += m.quantity.get().value();
        { Risk* exposed_by = exposed(m.aggressor_order_id.get().value(), m.quantity.get().value(), static_cast<long double>(m.price.get().value_or(0))); fill(m.aggressor_order_id.get().value(), m.quantity.get().value()); if (exposed_by != nullptr) broken(*exposed_by, 0, 0); }
    }

    // The engine's trade_broken_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::trade_broken_message& m) {
        if (const Order* found = traded(m.resting_order_id.get().value()); found != nullptr) respond(to_ouch_broken_resting(m, *found), echoed(found->options, { 28 }, { 28 }));
        if (const Order* found = traded(m.aggressor_order_id.get().value()); found != nullptr) respond(to_ouch_broken_aggressor(m, *found), echoed(found->options, { 28 }, { 28 }));
    }

    // The engine's order_rested_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_rested_message& m) {
        if (static_cast<char>(m.repriced.get().value()) == 'N' && static_cast<long long>(m.refreshed_from.get().value()) != 0) if (auto found = orders_.find(m.refreshed_from.get().value()); found != orders_.end()) respond(to_ouch_restated(m, found->second), stated(1, 8, static_cast<std::uint64_t>(m.order_id.get().value())) + stated(22, 4, static_cast<std::uint64_t>(m.quantity.get().value())) + echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.repriced.get().value()) == 'Y') if (auto found = orders_.find(m.refreshed_from.get().value()); found != orders_.end()) respond(to_ouch_priority_update(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (auto found = orders_.find(m.refreshed_from.get().value()); found != orders_.end()) { auto held = orders_.extract(found); held.key() = m.order_id.get().value(); held.mapped().id = held.key(); references_[Named{ held.mapped().channel, held.mapped().reference }] = held.key(); orders_.insert(std::move(held)); }
    }

    // The engine's cancel_pending_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::cancel_pending_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_cancel_pending(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
    }

    // The engine's reduce_refused_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::reduce_refused_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && !(found->second.told & 1u)) { found->second.told |= 1u; respond(to_ouch_cancel_reject(m, found->second), echoed(found->second.options, { 28 }, { 28 })); }
    }

    // The engine's order_canceled_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_canceled_message& m) {
        if (static_cast<char>(m.prevented.get().value()) == 'Y') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_aiq_canceled(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.prevented.get().value()) == 'N' && static_cast<char>(m.elimination_reason.get().value()) != 'X') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('M') && found->second.pending != static_cast<char>('Z')) respond(to_ouch_canceled(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.elimination_reason.get().value()) == 'X') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_canceled_protected(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.prevented.get().value()) != 'Y' && static_cast<char>(m.elimination_reason.get().value()) != 'X') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending == static_cast<char>('Z')) respond(to_ouch_risk_canceled(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        close(m.order_id.get().value());
    }

    // The engine's order_reduced_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_reduced_message& m) {
        if (static_cast<char>(m.prevented.get().value()) == 'Y') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_aiq_reduced(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.prevented.get().value()) != 'Y') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending == static_cast<char>('X')) respond(to_ouch_reduced(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_eliminated_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_eliminated_message& m) {
        if (static_cast<char>(m.elimination_reason.get().value()) == 'H') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_eliminated_halted(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.time_in_force.get().value()) == '3' && static_cast<char>(m.elimination_reason.get().value()) != 'H') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_eliminated(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (!m.price.get().has_value() && static_cast<char>(m.time_in_force.get().value()) != '3' && static_cast<char>(m.elimination_reason.get().value()) != 'H') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_eliminated_market(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.time_in_force.get().value()) == '0' && m.price.get().has_value() && static_cast<char>(m.elimination_reason.get().value()) != 'H') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_expired(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.time_in_force.get().value()) == '5' && m.price.get().has_value() && static_cast<char>(m.elimination_reason.get().value()) != 'H') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_expired_extended(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.time_in_force.get().value()) == '6' && m.price.get().has_value() && static_cast<char>(m.elimination_reason.get().value()) != 'H') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_expired_timed(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_refused_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_refused_message& m) {
        if (static_cast<char>(m.refusal_reason.get().value()) == 'E') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_order_accepted_closed(found->second.request, found->second), echoed(found->second.options, { 2, 3, 4, 5, 13, 9, 10, 6, 12, 15, 16, 24, 29, 30, 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'G') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_order_accepted_slid(found->second.request, found->second), echoed(found->second.options, { 2, 3, 4, 5, 13, 9, 10, 6, 12, 15, 16, 24, 29, 30, 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'E') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_refused(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'G') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_slid(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending == static_cast<char>('U')) respond(to_ouch_replace_rejected(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'O') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'P') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected_peg(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'T') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected_type(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'C') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected_cross(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'N') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected_unreferenced(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'A') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected_aggressive(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'F') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected_fat_finger(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'L') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(to_ouch_rejected_limit_protection(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        close(m.order_id.get().value());
    }

    // The engine's order_accepted_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_accepted_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) found->second.price = m.price.get().value_or(2147483647);
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(to_ouch_order_accepted(found->second.request, found->second), echoed(found->second.options, { 2, 3, 4, 5, 13, 9, 10, 11, 6, 7, 12, 15, 16, 17, 24, 25, 29, 30, 28 }, { 28 }));
    }

    // The engine's order_replaced_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_replaced_message& m) {
        if (auto found = orders_.find(m.replacement_id.get().value()); found != orders_.end()) respond(to_ouch_replaced(found->second.replace, found->second), echoed(found->second.options, { 2, 3, 5, 6, 12, 15, 16, 17, 29, 30, 28 }, { 28 }));
        if (auto found = orders_.find(m.replacement_id.get().value()); found != orders_.end()) found->second.remaining = m.open_quantity.get().value();
        if (m.order_id.get().value() != m.replacement_id.get().value()) close(m.order_id.get().value());
    }

private:
    // The risk controls the venue declares of a client, each as its <Client> states it: a control it leaves out is
    // off, and a client stating none has no limit.
    struct Controls {
        bool share_size_control = false;   // ShareSizeControl
        long double share_size_control_shares = 0;   // its Shares
        bool maximum_single_order_notional = false;   // MaximumSingleOrderNotional
        long double maximum_single_order_notional_dollars = 0;   // its Dollars
        bool restricted_stock_list = false;   // RestrictedStockList
        std::vector<std::string_view> restricted_stock_list_symbol{};   // its Symbol list
        bool hard_to_borrow_list = false;   // HardToBorrowList
        std::vector<std::string_view> hard_to_borrow_list_symbol{};   // its Symbol list
        bool order_type_check_pre_market = false;   // OrderTypeCheck.PreMarket
        bool order_type_check_post_market = false;   // OrderTypeCheck.PostMarket
        bool order_type_check_short_sale = false;   // OrderTypeCheck.ShortSale
        bool order_type_check_short_sale_exempt = false;   // OrderTypeCheck.ShortSaleExempt
        bool order_type_check_intermarket_sweep = false;   // OrderTypeCheck.IntermarketSweep
        bool order_type_check_ipo_market_buy = false;   // OrderTypeCheck.IpoMarketBuy
        bool market_impact_check = false;   // MarketImpactCheck
        bool average_daily_volume_check = false;   // AverageDailyVolumeCheck
        long double average_daily_volume_check_minimum_shares = 0;   // its MinimumShares
        long double average_daily_volume_check_percent = 0;   // its Percent
        bool rate_thresholds_port = false;   // RateThresholds.Port
        long double rate_thresholds_port_messages = 0;   // its Messages
        long double rate_thresholds_port_milliseconds = 0;   // its Milliseconds
        bool rate_thresholds_symbol = false;   // RateThresholds.Symbol
        long double rate_thresholds_symbol_messages = 0;   // its Messages
        long double rate_thresholds_symbol_milliseconds = 0;   // its Milliseconds
        bool duplication_control = false;   // DuplicationControl
        long double duplication_control_seconds = 0;   // its Seconds
        bool kill_switch_gross_executed_exposure = false;   // KillSwitch.GrossExecutedExposure
        long double kill_switch_gross_executed_exposure_dollars = 0;   // its Dollars
        bool kill_switch_gross_notional_exposure = false;   // KillSwitch.GrossNotionalExposure
        long double kill_switch_gross_notional_exposure_dollars = 0;   // its Dollars
        bool gross_exposure_check = false;   // GrossExposureCheck
        std::vector<std::string_view> gross_exposure_check_measure{};   // its Measure list
        std::vector<std::string_view> gross_exposure_check_cancel_open_orders{};   // its CancelOpenOrders list
        long double gross_exposure_check_dollars = 0;   // its Dollars
        bool cancel_on_disconnect = false;   // CancelOnDisconnect
    };

    static Controls controlled(std::string_view username) {
        Controls controls{};
        if (username == "RISKST") {
            controls.share_size_control = true;
            controls.share_size_control_shares = 5000;
            controls.maximum_single_order_notional = true;
            controls.maximum_single_order_notional_dollars = 2000;
            controls.order_type_check_pre_market = true;
            controls.order_type_check_post_market = true;
            controls.order_type_check_short_sale = true;
            controls.order_type_check_short_sale_exempt = true;
            controls.order_type_check_intermarket_sweep = true;
            controls.order_type_check_ipo_market_buy = true;
            controls.hard_to_borrow_list = true;
            controls.hard_to_borrow_list_symbol = { "ZWZZT" };
            controls.market_impact_check = true;
            controls.average_daily_volume_check = true;
            controls.average_daily_volume_check_minimum_shares = 100000;
            controls.average_daily_volume_check_percent = 1;
        }
        if (username == "RSTRCT") {
            controls.restricted_stock_list = true;
            controls.restricted_stock_list_symbol = { "ZXZZT" };
        }
        if (username == "RATEDP") {
            controls.rate_thresholds_port = true;
            controls.rate_thresholds_port_messages = 10;
            controls.rate_thresholds_port_milliseconds = 1000;
            controls.rate_thresholds_symbol = true;
            controls.rate_thresholds_symbol_messages = 4;
            controls.rate_thresholds_symbol_milliseconds = 1000;
            controls.duplication_control = true;
            controls.duplication_control_seconds = 2;
        }
        if (username == "KILLSW") {
            controls.kill_switch_gross_executed_exposure = true;
            controls.kill_switch_gross_executed_exposure_dollars = 300;
            controls.kill_switch_gross_notional_exposure = true;
            controls.kill_switch_gross_notional_exposure_dollars = 2000;
            controls.cancel_on_disconnect = true;
        }
        if (username == "GROSSX") {
            controls.gross_exposure_check = true;
            controls.gross_exposure_check_measure = { "Open" };
            controls.gross_exposure_check_cancel_open_orders = { "Yes" };
            controls.gross_exposure_check_dollars = 1000;
        }
        if (username == "FIXRSK") {
            controls.share_size_control = true;
            controls.share_size_control_shares = 5000;
            controls.maximum_single_order_notional = true;
            controls.maximum_single_order_notional_dollars = 2000;
            controls.order_type_check_pre_market = true;
            controls.order_type_check_post_market = true;
            controls.order_type_check_short_sale = true;
            controls.order_type_check_short_sale_exempt = true;
            controls.restricted_stock_list = true;
            controls.restricted_stock_list_symbol = { "ZXZZT" };
            controls.market_impact_check = true;
            controls.duplication_control = true;
            controls.duplication_control_seconds = 2;
            controls.gross_exposure_check = true;
            controls.gross_exposure_check_measure = { "Open" };
            controls.gross_exposure_check_cancel_open_orders = { "Yes" };
            controls.gross_exposure_check_dollars = 1000;
            controls.cancel_on_disconnect = true;
        }
        return controls;
    }

    // What the gateway holds of a client for its risk controls, by the username it logs in as: kept the day,
    // across every connection it logs in on, as the port's Account is. Its orders are those it entered.
    struct Risk {
        std::string username{};   // the client's
        Controls controls{};   // its risk controls, as the venue declares them
        std::deque<std::int64_t> rate_thresholds_port{};   // RateThresholds.Port: when each of its messages within the window came, on the venue's clock
        std::map<std::string, std::deque<std::int64_t>, std::less<>> rate_thresholds_symbol{};   // RateThresholds.Symbol: the same, for the messages naming each instrument
        std::deque<std::pair<std::int64_t, std::string>> duplication_control{};   // DuplicationControl: when each order it entered within the window came, and what it is told apart by
        long double executed = 0;   // what its orders have executed: shares times the price, in the Ipc's price units, each side counted positive
        bool stopped = false;   // has a breach stopped it entering orders for the rest of the day?
    };

    struct Client {
        Gateway& gateway;
        std::size_t index;
        Send send;
        std::string firm{};   // the firm it was admitted under, none until it is
        bool withdraws = false;   // does its port have a post-only order canceled rather than repriced, as the venue declares the client
        std::uint32_t protection_percent = 0;   // how far through the book's best opposite price its port lets an order be priced, in hundredths of a percent of it, as the venue declares the client: 0 for none
        std::int64_t protection_amount = 0;   // and as an amount, at the Ipc's price decimals: 0 for none
        bool rejects_closing = false;   // does its port have a late order for the Closing Cross priced beyond its reference prices rejected rather than repriced, as the venue declares the client
        bool rejects_opening = false;   // does its port have a late order for the Opening Cross priced beyond its reference prices rejected rather than repriced, as the venue declares the client
        Controls controls{};   // the risk controls the venue declares of the client it logged in as: none until it does
        Risk* risk = nullptr;   // what the gateway holds of the client it logged in as, for its risk controls: none until it does
        bool logged_out = false;   // did its session end by its logout, or its login's refusal, rather than by a disruption?
        soupbin::Session<Send, Client> session;

        Client(Gateway& owner, std::size_t at, Send writer) : gateway(owner), index(at), send(std::move(writer)), session("NSMOUCH001", send, *this) {}

        std::optional<soupbin::status> login(auto username, auto password, auto requested, auto) {
            if (const std::string_view asked = requested; !asked.empty() && asked != "NSMOUCH001") return static_cast<soupbin::status>('S');   // not the session served
            withdraws = withdrawing(username);   // its port's choice for a post-only order the book would reprice
            protecting(username, protection_percent, protection_amount);   // its port's limit on how far through the book's best opposite price an order is priced
            controls = controlled(username);   // the risk controls the venue declares of it
            rejects_closing = rejecting_closing(username);   // its port's choice for a late order for the Closing Cross beyond its reference prices
            rejects_opening = rejecting_opening(username);   // its port's choice for a late order for the Opening Cross beyond its reference prices
            const auto refused = gateway.login(username, password);
            if (!refused) { risk = &gateway.risks_[std::string(username)]; risk->username = std::string(username); risk->controls = controls; }   // what the gateway holds of the client, kept the day across its connections
            if (!refused) firm = std::string(username);   // no firm is registered for an account: its username stands in for one
            return refused;
        }
        void message(std::string_view packet) { gateway.message(index, packet); }
        void logout() { lost = true; logged_out = true; }   // the session is over: logged out, or its login refused
        void silent() { lost = true; }   // heard nothing for longer than the session allows
        bool lost = false;
    };

    // A login is refused with the reason the exchange states, or the first the model gives.
    std::optional<soupbin::status> login(auto username, auto password) const {
        if (credentials_.empty()) return std::nullopt;
        // Credentials are configured as text, whatever the protocol presents them as.
        const auto named = [](auto value) {
            if constexpr (std::is_arithmetic_v<decltype(value)>) return std::to_string(value);
            else return std::string(value);
        };
        const auto found = credentials_.find(named(username));
        if (found != credentials_.end() && found->second == named(password)) return std::nullopt;
        return static_cast<soupbin::status>('A');
    }

    // One unsequenced data packet from a client, dispatched by its message's code.
    void message(std::size_t client, std::string_view packet) {
        if (packet.size() <= sizeof(protocol::client_packet_header)) return;
        if (clients_.at(client)->risk == nullptr) return;   // not logged in: nothing it sends is taken
        Risk& risk = *clients_.at(client)->risk;   // what the gateway holds of the client, for its risk controls
        const std::int64_t now = send_time_nanos();   // the venue's clock, which a client's windows count by
        if (risk.controls.rate_thresholds_port) tallied(risk.rate_thresholds_port, now, risk.controls.rate_thresholds_port_milliseconds * 1000000);   // RateThresholds.Port: every message it sends, within the window
        const auto* bytes = reinterpret_cast<const std::byte*>(packet.data());
        auto selector = protocol::unsequenced_data_packet::parse(bytes)->fields.unsequenced_message_type.get().value();
        switch (selector) {
            case static_cast<decltype(selector)>('O'): {
                if (packet.size() < fixed_size<protocol::enter_order_message>()) return;
                const auto& m = *protocol::enter_order_message::parse(bytes);
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(trimmed_symbol(m.fields.symbol.get().value()))], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
                if (auto& last = last_reference_[channel]; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                if (blocked_) { respond(to_ouch_rejected_disabled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (risk.stopped) { respond(to_ouch_risk_stopped(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }   // a breach stopped its client for the day
                if (!((static_cast<long long>(m.fields.quantity.get().value()) >= 1 && static_cast<long long>(m.fields.quantity.get().value()) <= 999999))) { respond(to_ouch_rejected_quantity(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!((static_cast<long long>(m.fields.price.get().value()) >= 0 && static_cast<long long>(m.fields.price.get().value()) <= 1999999900) || static_cast<long long>(m.fields.price.get().value()) == 2000000000 || static_cast<long long>(m.fields.price.get().value()) == 2147483647)) { respond(to_ouch_rejected_price(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(true && static_cast<long long>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 3, 4)).value_or(0)) % 100 == 0)) { respond(to_ouch_rejected_min_qty(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(true && static_cast<long long>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 5, 4)).value_or(0)) % 100 == 0)) { respond(to_ouch_rejected_max_floor(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(static_cast<char>(m.fields.side.get().value()) == 'B' || static_cast<char>(m.fields.side.get().value()) == 'S' || static_cast<char>(m.fields.side.get().value()) == 'T' || static_cast<char>(m.fields.side.get().value()) == 'E')) { respond(to_ouch_rejected_side(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!((!(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).has_value() || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == '*' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'N' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'Y' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'D' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'O' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'W' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'R' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'y' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'd' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'o' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'w' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'r' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'Z' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'E' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'P' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'X' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == 'S' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == '0' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == '1' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == '2' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == '4' || static_cast<char>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1)).value_or(0)) == '5'))) { respond(to_ouch_rejected_aiq(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(static_cast<char>(m.fields.display.get().value()) == 'Y' || static_cast<char>(m.fields.display.get().value()) == 'N' || static_cast<char>(m.fields.display.get().value()) == 'A')) { respond(to_ouch_rejected_display(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!([](std::string_view named) { while (!named.empty() && named.back() == ' ') named.remove_suffix(1); return named == "ZVZZT" || named == "ZWZZT" || named == "ZXZZT"; }(m.fields.symbol.get().value()))) { respond(to_ouch_rejected_symbol(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                const auto& controls = clients_.at(client)->controls;   // the risk controls the venue declares of the client
                if (controls.share_size_control && static_cast<long double>(m.fields.quantity.get().value()) > controls.share_size_control_shares) { respond(to_ouch_risk_share_size(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.maximum_single_order_notional && !(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.quantity.get().value()) * static_cast<long double>(m.fields.price.get().value()) > controls.maximum_single_order_notional_dollars * 10000) { respond(to_ouch_risk_notional(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.maximum_single_order_notional && (static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.quantity.get().value()) * listing_luld_reference(trimmed_symbol(m.fields.symbol.get().value())) > controls.maximum_single_order_notional_dollars * 10000) { respond(to_ouch_risk_notional_unpriced(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.restricted_stock_list && listed_in(controls.restricted_stock_list_symbol, trimmed_symbol(m.fields.symbol.get().value()))) { respond(to_ouch_risk_restricted(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.hard_to_borrow_list && (static_cast<char>(m.fields.side.get().value()) == 'T' || static_cast<char>(m.fields.side.get().value()) == 'E') && listed_in(controls.hard_to_borrow_list_symbol, trimmed_symbol(m.fields.symbol.get().value()))) { respond(to_ouch_risk_hard_to_borrow(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_pre_market && (heard_phase_announced_message_announcement_ == 'O' || heard_phase_announced_message_announcement_ == 'S') && static_cast<char>(m.fields.cross_type.get().value()) == 'N' && (static_cast<char>(m.fields.time_in_force.get().value()) == '3' || static_cast<char>(m.fields.time_in_force.get().value()) == '5' || static_cast<char>(m.fields.time_in_force.get().value()) == '6')) { respond(to_ouch_risk_pre_market(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_post_market && (heard_phase_announced_message_announcement_ == 'M' || heard_phase_announced_message_announcement_ == 'E') && static_cast<char>(m.fields.cross_type.get().value()) == 'N' && (static_cast<char>(m.fields.time_in_force.get().value()) == '3' || static_cast<char>(m.fields.time_in_force.get().value()) == '5' || static_cast<char>(m.fields.time_in_force.get().value()) == '6')) { respond(to_ouch_risk_post_market(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_post_market && static_cast<char>(m.fields.time_in_force.get().value()) == 'E') { respond(to_ouch_risk_after_hours(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_short_sale && static_cast<char>(m.fields.side.get().value()) == 'T') { respond(to_ouch_risk_short_sale(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_short_sale_exempt && static_cast<char>(m.fields.side.get().value()) == 'E') { respond(to_ouch_risk_short_sale_exempt(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_intermarket_sweep && static_cast<char>(m.fields.inter_market_sweep_eligibility.get().value()) == 'Y') { respond(to_ouch_risk_intermarket_sweep(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_ipo_market_buy && static_cast<char>(m.fields.side.get().value()) == 'B' && static_cast<char>(m.fields.cross_type.get().value()) == 'H' && (static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && listing_ipo_flag_text(trimmed_symbol(m.fields.symbol.get().value())) == "Y") { respond(to_ouch_risk_ipo_market_buy(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.market_impact_check && static_cast<char>(m.fields.side.get().value()) == 'B' && !(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.price.get().value()) > listing_luld_upper(trimmed_symbol(m.fields.symbol.get().value()))) { respond(to_ouch_risk_market_impact_buy(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.market_impact_check && (static_cast<char>(m.fields.side.get().value()) == 'S' || static_cast<char>(m.fields.side.get().value()) == 'T' || static_cast<char>(m.fields.side.get().value()) == 'E') && !(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.price.get().value()) < listing_luld_lower(trimmed_symbol(m.fields.symbol.get().value()))) { respond(to_ouch_risk_market_impact_sell(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.average_daily_volume_check && static_cast<long double>(m.fields.quantity.get().value()) > std::max(listing_average_daily_volume(trimmed_symbol(m.fields.symbol.get().value())), controls.average_daily_volume_check_minimum_shares) * controls.average_daily_volume_check_percent / 100) { respond(to_ouch_risk_average_daily_volume(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.rate_thresholds_port && static_cast<long double>(risk.rate_thresholds_port.size()) > controls.rate_thresholds_port_messages) { respond(to_ouch_risk_port_rate(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.rate_thresholds_symbol && static_cast<long double>(risk.rate_thresholds_symbol[std::string(trimmed_symbol(m.fields.symbol.get().value()))].size()) > controls.rate_thresholds_symbol_messages) { respond(to_ouch_risk_symbol_rate(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                const std::string key_ouch_risk_duplicate = std::string(trimmed_symbol(m.fields.symbol.get().value())) + '\x1f' + std::to_string(static_cast<long long>(m.fields.side.get().value())) + '\x1f' + std::to_string(static_cast<long long>(m.fields.quantity.get().value())) + '\x1f' + std::to_string(static_cast<long long>(m.fields.price.get().value())) + '\x1f' + std::to_string(static_cast<long long>(m.fields.time_in_force.get().value())) + '\x1f' + std::to_string(static_cast<long long>(m.fields.cross_type.get().value()));   // what it is told apart by, for a duplicate
                if (controls.duplication_control && duplicated(risk.duplication_control, now, controls.duplication_control_seconds * 1000000000, key_ouch_risk_duplicate)) { respond(to_ouch_risk_duplicate(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                Order order{ numbered(), static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ };
                order.channel = channel;
                order.firm = clients_.at(client)->firm;
                auto ipc = to_ouch_enter_order(m, order);
                if (clients_.at(client)->withdraws) withdrawn(ipc);   // its port has a post-only order canceled rather than repriced
                protect(ipc, clients_.at(client)->protection_percent, clients_.at(client)->protection_amount);   // its port's limit on how far through the book's best opposite price it is priced
                if (clients_.at(client)->rejects_closing) rejected(ipc, 'C');   // its port has a late order for the Closing Cross beyond its reference prices rejected
                if (clients_.at(client)->rejects_opening) rejected(ipc, 'O');   // its port has a late order for the Opening Cross beyond its reference prices rejected
                const long double priced = (ipc.price.get().has_value() ? static_cast<long double>(ipc.price.get().value()) : listing_luld_reference(trimmed_symbol(m.fields.symbol.get().value())));   // what its open shares are valued at by its client's exposure
                if (broken(risk, static_cast<long double>(ipc.quantity.get().value()) * priced, 0)) { respond(to_ouch_risk_stopped(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }   // admitted, it would breach a limit of its client's
                order.valued = priced;
                order.client = risk.username;   // the client that entered it
                order.remaining = ipc.quantity.get().value();
                order.side = static_cast<char>(ipc.side.get().value());   // the side the engine was told
                order.display = static_cast<char>(ipc.display.get().value());   // whether the engine was told to show it
                order.entered = m.fields;
                order.options.assign(appendage(m, packet));
                std::memcpy(&order.request, packet.data(), std::min(packet.size(), sizeof order.request));   // as much of it as arrived
                if (controls.duplication_control) risk.duplication_control.emplace_back(now, key_ouch_risk_duplicate);   // DuplicationControl: an order it entered
                orders_[order.id] = order;
                references_[Named{ order.channel, order.reference }] = order.id;
                detail_ipc::emit_framed(emit_, 'N', ipc);
                return;
            }
            case static_cast<decltype(selector)>('X'): {
                if (packet.size() < fixed_size<protocol::cancel_order_message>() - 2) return;   // its last 2 bytes may be left out
                const auto& m = *protocol::cancel_order_message::parse(bytes);
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                if (m.fields.quantity.get().value() == 0) {
                    const auto key = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                    const auto found = references_.find(Named{ channel, key });
                    if (found == references_.end()) return;
                    if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(trimmed_symbol(orders_.at(found->second).entered.symbol.get().value()))], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
                    Order& order = orders_.at(found->second);
                    order.pending = static_cast<char>('X');
                    auto ipc = to_ouch_cancel_order(m, order);
                    detail_ipc::emit_framed(emit_, 'C', ipc);
                    return;
                }
                if (m.fields.quantity.get().value() != 0) {
                    const auto key = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                    const auto found = references_.find(Named{ channel, key });
                    if (found == references_.end()) return;
                    if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(trimmed_symbol(orders_.at(found->second).entered.symbol.get().value()))], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
                    Order& order = orders_.at(found->second);
                    order.pending = static_cast<char>('X');
                    auto ipc = to_ouch_reduce_order(m, order);
                    detail_ipc::emit_framed(emit_, 'D', ipc);
                    return;
                }
                return;   // a message none of its requests takes
            }
            case static_cast<decltype(selector)>('M'): {
                if (packet.size() < fixed_size<protocol::modify_order_message>() - 2) return;   // its last 2 bytes may be left out
                const auto& m = *protocol::modify_order_message::parse(bytes);
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                const auto key = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                const auto found = references_.find(Named{ channel, key });
                if (found == references_.end()) return;
                if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(trimmed_symbol(orders_.at(found->second).entered.symbol.get().value()))], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
                Order& order = orders_.at(found->second);
                order.pending = static_cast<char>('M');
                auto ipc = to_ouch_modify_order(m, order);
                respond(to_ouch_modified(m, order), echoed(order.options, { 28 }, { 28 }));
                detail_ipc::emit_framed(emit_, 'D', ipc);
                return;
            }
            case static_cast<decltype(selector)>('U'): {
                if (packet.size() < fixed_size<protocol::replace_order_message>()) return;
                const auto& m = *protocol::replace_order_message::parse(bytes);
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                const auto key = static_cast<std::uint64_t>(m.fields.orig_user_ref_num.get().value());
                const auto found = references_.find(Named{ channel, key });
                if (found == references_.end()) return;
                if (auto& last = last_reference_[channel]; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission, or a number already used
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                const Order& replaced = orders_.at(found->second);
                if (risk.controls.rate_thresholds_symbol) tallied(risk.rate_thresholds_symbol[std::string(trimmed_symbol(replaced.entered.symbol.get().value()))], now, risk.controls.rate_thresholds_symbol_milliseconds * 1000000);   // RateThresholds.Symbol: the messages it sent naming the instrument, within the window
                if (risk.stopped) { respond(to_ouch_risk_stopped_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }   // a breach stopped its client for the day
                const auto& controls = clients_.at(client)->controls;   // the risk controls the venue declares of the client
                if (controls.share_size_control && static_cast<long double>(m.fields.quantity.get().value()) > controls.share_size_control_shares) { respond(to_ouch_risk_share_size_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.maximum_single_order_notional && !(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.quantity.get().value()) * static_cast<long double>(m.fields.price.get().value()) > controls.maximum_single_order_notional_dollars * 10000) { respond(to_ouch_risk_notional_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.maximum_single_order_notional && (static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.quantity.get().value()) * listing_luld_reference(trimmed_symbol(replaced.entered.symbol.get().value())) > controls.maximum_single_order_notional_dollars * 10000) { respond(to_ouch_risk_notional_unpriced_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_pre_market && (heard_phase_announced_message_announcement_ == 'O' || heard_phase_announced_message_announcement_ == 'S') && static_cast<char>(replaced.entered.cross_type.get().value()) == 'N' && (static_cast<char>(m.fields.time_in_force.get().value()) == '3' || static_cast<char>(m.fields.time_in_force.get().value()) == '5' || static_cast<char>(m.fields.time_in_force.get().value()) == '6')) { respond(to_ouch_risk_pre_market_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_post_market && (heard_phase_announced_message_announcement_ == 'M' || heard_phase_announced_message_announcement_ == 'E') && static_cast<char>(replaced.entered.cross_type.get().value()) == 'N' && (static_cast<char>(m.fields.time_in_force.get().value()) == '3' || static_cast<char>(m.fields.time_in_force.get().value()) == '5' || static_cast<char>(m.fields.time_in_force.get().value()) == '6')) { respond(to_ouch_risk_post_market_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_post_market && static_cast<char>(m.fields.time_in_force.get().value()) == 'E') { respond(to_ouch_risk_after_hours_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.order_type_check_intermarket_sweep && static_cast<char>(m.fields.inter_market_sweep_eligibility.get().value()) == 'Y') { respond(to_ouch_risk_intermarket_sweep_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.market_impact_check && static_cast<char>(replaced.entered.side.get().value()) == 'B' && !(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.price.get().value()) > listing_luld_upper(trimmed_symbol(replaced.entered.symbol.get().value()))) { respond(to_ouch_risk_market_impact_buy_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.market_impact_check && (static_cast<char>(replaced.entered.side.get().value()) == 'S' || static_cast<char>(replaced.entered.side.get().value()) == 'T' || static_cast<char>(replaced.entered.side.get().value()) == 'E') && !(static_cast<long long>(m.fields.price.get().value()) == 2147483647 || static_cast<long long>(m.fields.price.get().value()) == 2000000000) && static_cast<long double>(m.fields.price.get().value()) < listing_luld_lower(trimmed_symbol(replaced.entered.symbol.get().value()))) { respond(to_ouch_risk_market_impact_sell_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.average_daily_volume_check && static_cast<long double>(m.fields.quantity.get().value()) > std::max(listing_average_daily_volume(trimmed_symbol(replaced.entered.symbol.get().value())), controls.average_daily_volume_check_minimum_shares) * controls.average_daily_volume_check_percent / 100) { respond(to_ouch_risk_average_daily_volume_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.rate_thresholds_port && static_cast<long double>(risk.rate_thresholds_port.size()) > controls.rate_thresholds_port_messages) { respond(to_ouch_risk_port_rate_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (controls.rate_thresholds_symbol && static_cast<long double>(risk.rate_thresholds_symbol[std::string(trimmed_symbol(replaced.entered.symbol.get().value()))].size()) > controls.rate_thresholds_symbol_messages) { respond(to_ouch_risk_symbol_rate_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                Order order = replaced;
                order.id = numbered();
                order.reference = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                order.replaced_id = replaced.id;
                order.options = merged(replaced.options, appendage(m, packet), { 25, 26 });
                order.replaced_reference = replaced.reference;
                std::memcpy(&order.replace, packet.data(), std::min(packet.size(), sizeof order.replace));   // as much of it as arrived
                order.pending = static_cast<char>('U');
                auto ipc = to_ouch_replace_order(m, order);
                if (clients_.at(client)->withdraws) withdrawn(ipc);   // its port has a post-only order canceled rather than repriced
                protect(ipc, clients_.at(client)->protection_percent, clients_.at(client)->protection_amount);   // its port's limit on how far through the book's best opposite price it is priced
                const long double priced = (ipc.price.get().has_value() ? static_cast<long double>(ipc.price.get().value()) : listing_luld_reference(trimmed_symbol(replaced.entered.symbol.get().value())));   // what its open shares are valued at by its client's exposure
                if (broken(risk, static_cast<long double>(ipc.quantity.get().value()) * priced, static_cast<long double>(replaced.remaining) * replaced.valued)) { respond(to_ouch_risk_stopped_replace(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }   // admitted, it would breach a limit of its client's
                order.valued = priced;
                order.remaining = ipc.quantity.get().value();
                orders_[order.id] = order;
                references_[Named{ order.channel, order.reference }] = order.id;
                detail_ipc::emit_framed(emit_, 'M', ipc);
                return;
            }
            case static_cast<decltype(selector)>('C'): {
                if (packet.size() < fixed_size<protocol::mass_cancel_request_message>()) return;
                const auto& m = *protocol::mass_cancel_request_message::parse(bytes);
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                if (auto& last = last_reference_[channel]; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                respond(to_ouch_mass_canceled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 27, 24, 28 }, { 28 }));
                std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
                for (const auto& [id, order] : orders_) if ((std::string_view(m.fields.symbol.get().value()).find_first_not_of(' ') == std::string_view::npos || std::string_view(m.fields.symbol.get().value()) == std::string_view(order.entered.symbol.get().value())) && (!option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 27, 1) || static_cast<char>(*option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 27, 1)) == static_cast<char>(order.entered.side.get().value())) && (!option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 24, 2) || *option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 24, 2) == option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 24, 2).value_or(0))) taken.push_back(id);
                for (const auto id : taken) {
                    const auto held = orders_.find(id);
                    if (held == orders_.end()) continue;
                    held->second.pending = static_cast<char>('C');
                    auto ipc = to_ouch_mass_cancel(m, held->second);
                    detail_ipc::emit_framed(emit_, 'C', ipc);
                }
                return;
            }
            case static_cast<decltype(selector)>('D'): {
                if (packet.size() < fixed_size<protocol::disable_order_entry_request_message>()) return;
                const auto& m = *protocol::disable_order_entry_request_message::parse(bytes);
                (void)m;
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                if (auto& last = last_reference_[channel]; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                respond(to_ouch_disabled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 }));
                blocked_ = true;   // it takes no new orders
                return;
            }
            case static_cast<decltype(selector)>('E'): {
                if (packet.size() < fixed_size<protocol::enable_order_entry_request_message>()) return;
                const auto& m = *protocol::enable_order_entry_request_message::parse(bytes);
                (void)m;
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                if (auto& last = last_reference_[channel]; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                respond(to_ouch_enabled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, account_ }), echoed(appendage(m, packet), { 28 }, { 28 }));
                blocked_ = false;   // it takes new orders again
                return;
            }
            case static_cast<decltype(selector)>('Q'): {
                if (packet.size() < fixed_size<protocol::account_query_message>() - 2) return;   // its last 2 bytes may be left out
                const auto& m = *protocol::account_query_message::parse(bytes);
                (void)m;
                const std::uint64_t channel = option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 28, 1).value_or(0);   // the order flow channel it names, 0 where it names none
                respond(to_ouch_account_query(m, Order{ 0, last_reference_[channel] + 1, 0, account_ }), echoed((packet.size() >= fixed_size<protocol::account_query_message>() ? appendage(m, packet) : std::string_view{}), { 28 }, { 28 }));
                return;
            }
            default:
                return;   // a message the order entry does not map
        }
    }

    // A response to the port's Account, as the body of the message each session publishes.
    // Every connection logged in on the port is told it, mirroring the Account (OUCH 5.0, 1.3); one
    // that has gone is not, and what the venue tells the Account while none is logged in goes nowhere.
    template <typename Response>
    void respond(const Response& response) {
        const auto* bytes = reinterpret_cast<const char*>(&response) + sizeof(protocol::server_packet_header);
        for (const auto& held : clients_) held.second->session.sequenced(bytes, response.header.packet_length.get().value() + 2 - sizeof(protocol::server_packet_header));
    }

    // A response with options in its appendage: written after its fixed part, as much as its
    // message has room for, and counted by its appendage's length and by its packet's.
    template <typename Response>
    void respond(Response response, std::string_view options) {
        const std::size_t size = std::min<std::size_t>(options.size(), Response::tail_capacity);
        std::memcpy(response.tail, options.data(), size);
        response.fields.appendage_length.set(static_cast<std::remove_cvref_t<decltype(response.fields.appendage_length.get().value())>>(size));
        response.header.packet_length.set(static_cast<std::remove_cvref_t<decltype(response.header.packet_length.get().value())>>(response.header.packet_length.get().value() + size));
        respond(response);
    }

    // The clients whose port has a post-only order that would lock what the book shows canceled rather
    // than repriced, as the venue declares them: Nasdaq's Participant's choice (Equity 4, Rule 4702(b)(4)).
    static bool withdrawing(std::string_view username) {
        return username == "MAKER2";
    }

    // A post-only order of such a client, told the engine so.
    template <typename Ipc>
    static void withdrawn(Ipc& ipc) {
        if constexpr (requires { ipc.post_only; }) {
            if (static_cast<char>(ipc.post_only.get().value()) == 'P') ipc.post_only.set(static_cast<std::remove_cvref_t<decltype(ipc.post_only.get().value())>>('C'));
        }
    }

    // The clients whose port has a late order for the Closing Cross priced beyond its reference prices rejected
    // rather than repriced, as the venue declares them: Nasdaq's Participant's instruction (Equity 4, Rule
    // 4702(b)(9)(A), (12)(A)), whose default is to reprice.
    static bool rejecting_closing(std::string_view username) {
        return username == "MAKER2";
    }

    // The clients whose port has a late order for the Opening Cross priced beyond its reference prices rejected
    // rather than repriced, as the venue declares them: Nasdaq's Participant's instruction (Equity 4, Rule
    // 4702(b)(9)(A), (12)(A)), whose default is to reprice.
    static bool rejecting_opening(std::string_view username) {
        return username == "MAKER2";
    }

    // A new order of such a client for that cross, told the engine its port has it rejected.
    template <typename Ipc>
    static void rejected(Ipc& ipc, char cross) {
        if constexpr (requires { ipc.late_limit; }) {
            if (static_cast<char>(ipc.cross_type.get().value()) == cross) ipc.late_limit.set(static_cast<std::remove_cvref_t<decltype(ipc.late_limit.get().value())>>('J'));
        }
    }

    // How far through the book's best opposite price each client's port lets an order be priced, as the venue
    // declares it: Nasdaq's Fat Finger Protection, "percentage-based and dollar-based controls are available"
    // (Equity 6, Section 5(m)) - a percent of that price, in hundredths of a percent, and an amount at the Ipc's
    // price decimals, each 0 for none. The engine measures it, holding the book.
    static void protecting(std::string_view username, std::uint32_t& percent, std::int64_t& amount) {
        if (username == "FATFNG") { percent = 500; amount = 200; return; }
        percent = 0;
        amount = 0;
    }

    // An order or a replace of a client, carrying its port's limit to the engine.
    template <typename Ipc>
    static void protect(Ipc& ipc, std::uint32_t percent, std::int64_t amount) {
        ipc.protection_percent.set(static_cast<std::remove_cvref_t<decltype(ipc.protection_percent.get().value())>>(percent));
        ipc.protection_amount.set(static_cast<std::remove_cvref_t<decltype(ipc.protection_amount.get().value())>>(amount));
    }

    // A symbol as a request names it, without the spaces that fill it.
    static std::string_view trimmed_symbol(std::string_view symbol) {
        while (!symbol.empty() && symbol.back() == ' ') symbol.remove_suffix(1);
        return symbol;
    }

    // Is a value one of a list a client's risk control states?
    static bool listed_in(const std::vector<std::string_view>& list, std::string_view value) {
        return std::find(list.begin(), list.end(), value) != list.end();
    }

    // Each instrument's <Listing><LuldReference>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_luld_reference(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 10000; }
        if (symbol == "ZWZZT") { return 10000; }
        if (symbol == "ZXZZT") { return 10000; }
        return 0;
    }

    // Each instrument's <Listing><IpoFlag>, as the venue declares it, by the instrument a request names: none for one it does not list.
    static std::string_view listing_ipo_flag_text(std::string_view symbol) {
        if (symbol == "ZVZZT") { return "N"; }
        if (symbol == "ZWZZT") { return "N"; }
        if (symbol == "ZXZZT") { return "Y"; }
        return {};
    }

    // Each instrument's <Listing><LuldUpper>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_luld_upper(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 10500; }
        if (symbol == "ZWZZT") { return 10500; }
        if (symbol == "ZXZZT") { return 10500; }
        return 0;
    }

    // Each instrument's <Listing><LuldLower>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_luld_lower(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 9500; }
        if (symbol == "ZWZZT") { return 9500; }
        if (symbol == "ZXZZT") { return 9500; }
        return 0;
    }

    // Each instrument's <Listing><AverageDailyVolume>, as the venue declares it, by the instrument a request names: 0 for one it does not list.
    static long double listing_average_daily_volume(std::string_view symbol) {
        if (symbol == "ZVZZT") { return 200000; }
        if (symbol == "ZWZZT") { return 200000; }
        if (symbol == "ZXZZT") { return 0; }
        return 0;
    }

    // A client's message tallied in a rolling window: when each within it came, this one last, those older let
    // go. How many there are.
    static std::size_t tallied(std::deque<std::int64_t>& times, std::int64_t now, long double window) {
        times.push_back(now);
        while (!times.empty() && times.front() <= now - static_cast<std::int64_t>(window)) times.pop_front();
        return times.size();
    }

    // Is an order a duplicate of one its client entered within the window, told apart by the same? Those older
    // are let go.
    static bool duplicated(std::deque<std::pair<std::int64_t, std::string>>& entered, std::int64_t now, long double window, const std::string& key) {
        while (!entered.empty() && entered.front().first <= now - static_cast<std::int64_t>(window)) entered.pop_front();
        return std::any_of(entered.begin(), entered.end(), [&](const auto& each) { return each.second == key; });
    }

    // What a client's open orders leave open: each one's open shares at the price it is valued at.
    long double opened(const Risk& risk) const {
        long double open = 0;
        for (const auto& held : orders_) if (held.second.client == risk.username) open += static_cast<long double>(held.second.remaining) * held.second.valued;
        return open;
    }

    // An execution of an order a client entered, counted in what its orders have executed: the client, or none.
    Risk* exposed(std::uint64_t id, std::uint64_t shares, long double price) {
        const auto found = orders_.find(id);
        if (found == orders_.end() || found->second.client.empty()) return nullptr;
        const auto risk = risks_.find(found->second.client);
        if (risk == risks_.end()) return nullptr;
        risk->second.executed += static_cast<long double>(shares) * price;
        return &risk->second;
    }

    // Is a limit of a client's broken, were `entering` more left open and `leaving` less: what its orders have
    // executed, or leave open, past what it declares? Broken, the client is stopped for the rest of the day and
    // what that breach cancels is canceled. A client stopped stays so.
    bool broken(Risk& risk, long double entering, long double leaving) {
        if (risk.stopped) return true;
        const long double executed = risk.executed;
        const long double open = opened(risk) + entering - leaving;
        if (!risk.stopped && risk.controls.kill_switch_gross_executed_exposure && executed > risk.controls.kill_switch_gross_executed_exposure_dollars * 10000) { risk.stopped = true; canceled_ouch_kill_switch_executed(risk); }   // KillSwitch.GrossExecutedExposure
        if (!risk.stopped && risk.controls.kill_switch_gross_notional_exposure && (executed + open) > risk.controls.kill_switch_gross_notional_exposure_dollars * 10000) { risk.stopped = true; canceled_ouch_kill_switch_notional(risk); }   // KillSwitch.GrossNotionalExposure
        if (!risk.stopped && risk.controls.gross_exposure_check && listed_in(risk.controls.gross_exposure_check_measure, "Open") && open > risk.controls.gross_exposure_check_dollars * 10000) { risk.stopped = true; canceled_ouch_gross_exposure_open(risk); }   // GrossExposureCheck
        if (!risk.stopped && risk.controls.gross_exposure_check && listed_in(risk.controls.gross_exposure_check_measure, "Executed") && executed > risk.controls.gross_exposure_check_dollars * 10000) { risk.stopped = true; canceled_ouch_gross_exposure_executed(risk); }   // GrossExposureCheck
        if (!risk.stopped && risk.controls.gross_exposure_check && listed_in(risk.controls.gross_exposure_check_measure, "Notional") && (executed + open) > risk.controls.gross_exposure_check_dollars * 10000) { risk.stopped = true; canceled_ouch_gross_exposure_notional(risk); }   // GrossExposureCheck
        return risk.stopped;
    }

    // The client's open orders ouch_kill_switch_executed cancels (KillSwitch.GrossExecutedExposure), each as ouch_mass_cancel cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_ouch_kill_switch_executed(const Risk& risk) {
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.client == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            auto ipc = to_ouch_mass_cancel(protocol::mass_cancel_request_message{}, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders ouch_kill_switch_notional cancels (KillSwitch.GrossNotionalExposure), each as ouch_mass_cancel cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_ouch_kill_switch_notional(const Risk& risk) {
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.client == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            auto ipc = to_ouch_mass_cancel(protocol::mass_cancel_request_message{}, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders ouch_gross_exposure_open cancels (GrossExposureCheck), each as ouch_mass_cancel cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_ouch_gross_exposure_open(const Risk& risk) {
        if (!(listed_in(risk.controls.gross_exposure_check_cancel_open_orders, "Yes"))) return;   // the client's choice
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.client == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            auto ipc = to_ouch_mass_cancel(protocol::mass_cancel_request_message{}, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders ouch_gross_exposure_executed cancels (GrossExposureCheck), each as ouch_mass_cancel cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_ouch_gross_exposure_executed(const Risk& risk) {
        if (!(listed_in(risk.controls.gross_exposure_check_cancel_open_orders, "Yes"))) return;   // the client's choice
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.client == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            auto ipc = to_ouch_mass_cancel(protocol::mass_cancel_request_message{}, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders ouch_gross_exposure_notional cancels (GrossExposureCheck), each as ouch_mass_cancel cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_ouch_gross_exposure_notional(const Risk& risk) {
        if (!(listed_in(risk.controls.gross_exposure_check_cancel_open_orders, "Yes"))) return;   // the client's choice
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.client == risk.username) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            auto ipc = to_ouch_mass_cancel(protocol::mass_cancel_request_message{}, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // The client's open orders ouch_cancel_on_disconnect cancels (CancelOnDisconnect), each as ouch_mass_cancel cancels an order, marked 'Z'
    // as the request last acting on it, which tells the answer to its cancel apart.
    void canceled_ouch_cancel_on_disconnect(const Risk& risk) {
        std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
        for (const auto& held : orders_) if (held.second.client == risk.username && !(static_cast<char>(held.second.entered.cross_type.get().value()) == 'O' || static_cast<char>(held.second.entered.cross_type.get().value()) == 'C')) taken.push_back(held.first);
        for (const auto id : taken) {
            const auto held = orders_.find(id);
            if (held == orders_.end()) continue;
            held->second.pending = static_cast<char>('Z');
            auto ipc = to_ouch_mass_cancel(protocol::mass_cancel_request_message{}, held->second);
            detail_ipc::emit_framed(emit_, 'C', ipc);
        }
    }

    // A client's session ended other than by its logout: what each breach of its made so does.
    void disconnected(const Risk& risk) {
        if (risk.controls.cancel_on_disconnect) canceled_ouch_cancel_on_disconnect(risk);   // CancelOnDisconnect
    }

    // Shares executed on an order; a filled order is no longer open.
    void fill(std::uint64_t id, std::uint64_t shares) {
        auto found = orders_.find(id);
        if (found == orders_.end()) return;
        found->second.remaining -= std::min(shares, found->second.remaining);
        found->second.traded = true;
        if (found->second.remaining == 0) close(id);
    }

    void close(std::uint64_t id) {
        auto found = orders_.find(id);
        if (found == orders_.end()) return;
        references_.erase(Named{ found->second.channel, found->second.reference });
        if (found->second.traded) { traded_.insert(orders_.extract(found)); return; }   // kept the day, for a broken trade's answer
        orders_.erase(found);
    }

    // An order the gateway holds open, or closed after it traded; none where it holds neither.
    const Order* traded(std::uint64_t id) const {
        if (const auto open = orders_.find(id); open != orders_.end()) return &open->second;
        if (const auto kept = traded_.find(id); kept != traded_.end()) return &kept->second;
        return nullptr;
    }

    // The number of the next order admitted, and the gateway's next after it.
    std::uint64_t numbered() {
        const std::uint64_t id = next_id_;
        next_id_ += step_;
        return id;
    }

    Emit emit_;
    std::int64_t heard_phase_announced_message_announcement_ = 0;   // the latest announcement of the engine's phase_announced_message, which a client risk control reads: 0 before it hears one
    std::map<std::size_t, std::unique_ptr<Client>> clients_;
    std::map<std::string, Risk, std::less<>> risks_;   // what the gateway holds of each client for its risk controls, by username: kept the day
    std::size_t next_client_ = 0;   // session numbers, never reused
    std::map<std::string, std::string, std::less<>> credentials_;
    std::size_t account_;   // the Account its port is bound to: the gateway's place among the venue's gateways
    bool blocked_ = false;   // does the Account take no new orders, having asked not to?
    std::map<std::uint64_t, std::uint64_t> last_reference_;   // the last reference a request of the Account admitted or replaced an order under, on each order flow channel
    std::map<std::uint64_t, Order> orders_;
    std::map<std::uint64_t, Order> traded_;   // orders that traded and have closed, kept the day: a broken trade is told their owners
    using Named = std::pair<std::uint64_t, std::uint64_t>;   // what names an order: the order flow channel it was entered on, and its reference there
    std::map<Named, std::uint64_t> references_;   // each open order, by the channel and the reference the Account entered it under
    std::uint64_t next_id_;   // the number the next order admitted is given
    std::uint64_t step_;      // how many gateways number in turn
};

}
