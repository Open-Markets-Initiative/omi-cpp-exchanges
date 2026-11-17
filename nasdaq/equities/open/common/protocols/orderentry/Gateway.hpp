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
#include "messages/OrderAcceptedMessage.hpp"
#include "messages/ReplacedMessage.hpp"
#include "messages/OrderModifiedMessage.hpp"
#include "messages/OrderExecutedMessage.hpp"
#include "messages/OrderRestatedMessage.hpp"
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
#include "../ipc/structs/TradeMessage.hpp"
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
// and the client that owns it.
struct Order {
    std::uint64_t id = 0;
    std::uint64_t reference = 0;
    std::uint64_t remaining = 0;
    std::size_t client = 0;
    protocol::enter_order_message::fields_type entered{};   // what the client entered it with, which a response restates
    protocol::enter_order_message request{};   // the request that admitted it, answered once the engine has it
    std::string options{};   // the options it was entered with, as they arrived, less those a replace changed: what a response echoes
    std::string firm{};   // the firm its client was admitted under, which the engine is told
    char side = 0;   // the side the engine was told it is on, as the Ipc says it
    char display = 0;   // whether the engine was told to show it, as the Ipc codes it
    std::int64_t price = 0;   // the price the engine took it at, as its acceptance says: the venue may reprice it on entry
    std::uint64_t executed = 0;   // the shares it has executed, which an intended size counts
    char pending = 0;   // the code of the request last acting on it, which tells its answers apart
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
    out.side.set(static_cast<value_of<decltype(out.side)>>([](char value) { switch (value) { case 'T': return 'S'; case 'E': return 'S'; default: return value; } }(static_cast<char>(m.fields.side.get().value()))));
    out.quantity.set(m.fields.quantity.get().value());
    out.symbol.set(m.fields.symbol.get().value());
    out.price.set(static_cast<value_of<decltype(out.price)>>(m.fields.price.get().value()));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.expire_time.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 15, 4).value_or(0));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 6, 1).value_or(static_cast<std::uint64_t>('L'))))));
    out.peg_offset.set(static_cast<std::int32_t>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 7, 4).value_or(0)));
    out.min_qty.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 3, 4).value_or(0));
    out.post_only.set(static_cast<value_of<decltype(out.post_only)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 12, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.trade_now.set(static_cast<value_of<decltype(out.trade_now)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 16, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.max_floor.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 5, 4).value_or(0));
    out.short_sale.set(static_cast<value_of<decltype(out.short_sale)>>(static_cast<char>(m.fields.side.get().value())));
    out.display.set(static_cast<value_of<decltype(out.display)>>([](char value) { switch (value) { case 'Z': return 'Y'; default: return value; } }(static_cast<char>(m.fields.display.get().value()))));
    { const std::optional<std::uint64_t> held = option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 2, 4); std::string text; if (held) { for (int at = 3; at >= 0; --at) text.push_back(static_cast<char>((*held >> (8 * at)) & 0xff)); } if (text.find_first_not_of(' ') == std::string::npos) text = order.firm.substr(0, 4); out.attribution.set(std::string_view{text}); }
    out.cross_type.set(static_cast<value_of<decltype(out.cross_type)>>(static_cast<char>(m.fields.cross_type.get().value())));
    out.self_match_strategy.set(static_cast<value_of<decltype(out.self_match_strategy)>>([](char value) { switch (value) { case 'Y': return 'D'; case 'D': return 'D'; case 'O': return 'O'; case 'W': return 'W'; case 'R': return 'R'; case 'y': return 'D'; case 'd': return 'D'; case 'o': return 'O'; case 'w': return 'W'; case 'r': return 'R'; case 'Z': return 'D'; case 'E': return 'D'; case 'P': return 'O'; case 'X': return 'W'; case 'S': return 'R'; case '0': return 'D'; case '1': return 'D'; case '2': return 'O'; case '4': return 'W'; case '5': return 'R'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.self_match_level.set(static_cast<value_of<decltype(out.self_match_level)>>([](char value) { switch (value) { case 'Y': return 'F'; case 'D': return 'F'; case 'O': return 'F'; case 'W': return 'F'; case 'R': return 'F'; case 'y': return 'O'; case 'd': return 'O'; case 'o': return 'O'; case 'w': return 'O'; case 'r': return 'O'; case 'Z': return 'A'; case 'E': return 'A'; case 'P': return 'A'; case 'X': return 'A'; case 'S': return 'A'; case '0': return 'M'; case '1': return 'M'; case '2': return 'M'; case '4': return 'M'; case '5': return 'M'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.aiq_strategy.set(static_cast<value_of<decltype(out.aiq_strategy)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>([&]() -> std::uint64_t { std::uint64_t key = 14695981039346656037ull; const auto mix = [&key](std::string_view text) { for (const char c : text) { key ^= static_cast<unsigned char>(c); key *= 1099511628211ull; } key ^= 0x1f; key *= 1099511628211ull; }; mix(std::to_string(order.client)); { const std::optional<std::uint64_t> held = option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 2, 4); mix(held ? std::to_string(*held) : std::string{}); } { const std::optional<std::uint64_t> held = option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 30, 2); mix(held ? std::to_string(*held) : std::string{}); } return key == 0 ? 1 : key; }()));
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
    out.quantity.set(liable(static_cast<std::uint64_t>(m.fields.quantity.get().value()), order));
    out.price.set(static_cast<value_of<decltype(out.price)>>(m.fields.price.get().value()));
    out.time_in_force.set(static_cast<value_of<decltype(out.time_in_force)>>(static_cast<char>(m.fields.time_in_force.get().value())));
    out.peg.set(static_cast<value_of<decltype(out.peg)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 6, 1).value_or(static_cast<std::uint64_t>('L'))))));
    out.peg_offset.set(static_cast<std::int32_t>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 7, 4).value_or(0)));
    out.min_qty.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 3, 4).value_or(0));
    out.post_only.set(static_cast<value_of<decltype(out.post_only)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 12, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.trade_now.set(static_cast<value_of<decltype(out.trade_now)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 16, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.max_floor.set(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 5, 4).value_or(0));
    { const std::optional<std::uint64_t> held = option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 2, 4); std::string text; if (held) { for (int at = 3; at >= 0; --at) text.push_back(static_cast<char>((*held >> (8 * at)) & 0xff)); } out.attribution.set(std::string_view{text}); }
    out.self_match_strategy.set(static_cast<value_of<decltype(out.self_match_strategy)>>([](char value) { switch (value) { case 'Y': return 'D'; case 'D': return 'D'; case 'O': return 'O'; case 'W': return 'W'; case 'R': return 'R'; case 'y': return 'D'; case 'd': return 'D'; case 'o': return 'O'; case 'w': return 'W'; case 'r': return 'R'; case 'Z': return 'D'; case 'E': return 'D'; case 'P': return 'O'; case 'X': return 'W'; case 'S': return 'R'; case '0': return 'D'; case '1': return 'D'; case '2': return 'O'; case '4': return 'W'; case '5': return 'R'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.self_match_level.set(static_cast<value_of<decltype(out.self_match_level)>>([](char value) { switch (value) { case 'Y': return 'F'; case 'D': return 'F'; case 'O': return 'F'; case 'W': return 'F'; case 'R': return 'F'; case 'y': return 'O'; case 'd': return 'O'; case 'o': return 'O'; case 'w': return 'O'; case 'r': return 'O'; case 'Z': return 'A'; case 'E': return 'A'; case 'P': return 'A'; case 'X': return 'A'; case 'S': return 'A'; case '0': return 'M'; case '1': return 'M'; case '2': return 'M'; case '4': return 'M'; case '5': return 'M'; case '*': return 'N'; default: return value; } }(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N')))))));
    out.aiq_strategy.set(static_cast<value_of<decltype(out.aiq_strategy)>>(static_cast<char>(static_cast<char>(option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 29, 1).value_or(static_cast<std::uint64_t>('N'))))));
    out.self_match_group.set(static_cast<value_of<decltype(out.self_match_group)>>([&]() -> std::uint64_t { std::uint64_t key = 14695981039346656037ull; const auto mix = [&key](std::string_view text) { for (const char c : text) { key ^= static_cast<unsigned char>(c); key *= 1099511628211ull; } key ^= 0x1f; key *= 1099511628211ull; }; mix(std::to_string(order.client)); { const std::optional<std::uint64_t> held = option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 2, 4); mix(held ? std::to_string(*held) : std::string{}); } { const std::optional<std::uint64_t> held = option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 30, 2); mix(held ? std::to_string(*held) : std::string{}); } return key == 0 ? 1 : key; }()));
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

// trade_message -> order_executed_message  (NsmEquities.Ipc.Transforms.xml: ouch_executed_resting)
inline protocol::order_executed_message to_ouch_executed_resting(const ipc_ns::trade_message& m, const Order& order) {
    (void)m;
    (void)order;
    protocol::order_executed_message out{};
    out.fields.user_ref_num.set(static_cast<value_of<decltype(out.fields.user_ref_num)>>(order.reference));
    out.fields.timestamp.set(nanoseconds_since_midnight(m.timestamp.get().value(), new_york()));
    out.fields.quantity.set(m.quantity.get().value());
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'A'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
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
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'e'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
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
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'J'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
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
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'k'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
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
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'R'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
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
    out.fields.price.set(static_cast<value_of<decltype(out.fields.price)>>(m.price.get().value()));
    out.fields.liquidity_flag.set(static_cast<value_of<decltype(out.fields.liquidity_flag)>>([](char value) { switch (value) { case 'N': return 'm'; default: return value; } }(static_cast<char>(m.auction.get().value()))));
    out.fields.match_number.set(m.execution_id.get().value());
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
    explicit Gateway(Emit emit, std::uint64_t first = 1, std::uint64_t step = 1) : emit_(emit), next_id_(first), step_(step) {}

    // A login the gateway accepts; with none registered, any login is accepted.
    void allow(std::string username, std::string password) { credentials_[std::move(username)] = std::move(password); }

    // A new client connection, writing through `send`; returns the session's own number.
    // 
    // Numbered once and never again: an order carries the number of the session that entered
    // it, and so does the reference it was entered under. A number handed out twice would
    // make one client's reference name another client's order.
    std::size_t connect(Send send) {
        const auto client = next_client_++;
        clients_.emplace(client, std::make_unique<Client>(*this, client, std::move(send)));
        return client;
    }

    // A client that has gone: its session is over, and what it held is let go. Whatever it
    // left resting stays in the book - a venue does not cancel an order because a line
    // dropped - and answers about those orders find nobody, which is what has happened.
    void disconnect(std::size_t client) { clients_.erase(client); }

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

    // The engine's trade_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::trade_message& m) {
        if (static_cast<char>(m.resting_display.get().value()) != 'N') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end() && static_cast<char>((option(reinterpret_cast<const std::byte*>(found->second.options.data()), reinterpret_cast<const std::byte*>(found->second.options.data()) + found->second.options.size(), 4, 1)).value_or(0)) != 'R') respond(found->second.client, to_ouch_executed_resting(m, found->second));
        if (static_cast<char>(m.resting_display.get().value()) != 'N') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end() && static_cast<char>((option(reinterpret_cast<const std::byte*>(found->second.options.data()), reinterpret_cast<const std::byte*>(found->second.options.data()) + found->second.options.size(), 4, 1)).value_or(0)) == 'R') respond(found->second.client, to_ouch_executed_resting_retail(m, found->second));
        if (static_cast<char>(m.resting_display.get().value()) == 'N' && static_cast<char>(m.resting_midpoint.get().value()) != 'Y') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_executed_hidden(m, found->second));
        if (static_cast<char>(m.resting_midpoint.get().value()) == 'Y') if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_executed_midpoint(m, found->second));
        if (static_cast<char>(m.resting_midpoint.get().value()) != 'Y') if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_executed_aggressor(m, found->second));
        if (static_cast<char>(m.resting_midpoint.get().value()) == 'Y') if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_executed_aggressor_midpoint(m, found->second));
        if (auto found = orders_.find(m.resting_order_id.get().value()); found != orders_.end()) found->second.executed += m.quantity.get().value();
        fill(m.resting_order_id.get().value(), m.quantity.get().value());
        if (auto found = orders_.find(m.aggressor_order_id.get().value()); found != orders_.end()) found->second.executed += m.quantity.get().value();
        fill(m.aggressor_order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_rested_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_rested_message& m) {
        if (static_cast<long long>(m.refreshed_from.get().value()) != 0) if (auto found = orders_.find(m.refreshed_from.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_restated(m, found->second), stated(1, 8, static_cast<std::uint64_t>(m.order_id.get().value())) + stated(22, 4, static_cast<std::uint64_t>(m.quantity.get().value())) + echoed(found->second.options, { 28 }, { 28 }));
        if (auto found = orders_.find(m.refreshed_from.get().value()); found != orders_.end()) { auto held = orders_.extract(found); held.key() = m.order_id.get().value(); held.mapped().id = held.key(); references_[{ held.mapped().client, held.mapped().reference }] = held.key(); orders_.insert(std::move(held)); }
    }

    // The engine's cancel_pending_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::cancel_pending_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_cancel_pending(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
    }

    // The engine's reduce_refused_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::reduce_refused_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_cancel_reject(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
    }

    // The engine's order_canceled_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_canceled_message& m) {
        if (static_cast<char>(m.prevented.get().value()) == 'Y') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_aiq_canceled(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.prevented.get().value()) == 'N') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('M')) respond(found->second.client, to_ouch_canceled(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        close(m.order_id.get().value());
    }

    // The engine's order_reduced_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_reduced_message& m) {
        if (static_cast<char>(m.prevented.get().value()) == 'Y') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_aiq_reduced(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.prevented.get().value()) != 'Y') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending == static_cast<char>('X')) respond(found->second.client, to_ouch_reduced(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_eliminated_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_eliminated_message& m) {
        if (static_cast<char>(m.time_in_force.get().value()) == '3') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_eliminated(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<long long>(m.price.get().value()) == 2147483647 && static_cast<char>(m.time_in_force.get().value()) != '3') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_eliminated_market(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.time_in_force.get().value()) == '0' && static_cast<long long>(m.price.get().value()) != 2147483647) if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_expired(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.time_in_force.get().value()) == '5' && static_cast<long long>(m.price.get().value()) != 2147483647) if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_expired_extended(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.time_in_force.get().value()) == '6' && static_cast<long long>(m.price.get().value()) != 2147483647) if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_expired_timed(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        fill(m.order_id.get().value(), m.quantity.get().value());
    }

    // The engine's order_refused_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_refused_message& m) {
        if (static_cast<char>(m.refusal_reason.get().value()) == 'E') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(found->second.client, to_ouch_order_accepted_closed(found->second.request, found->second), echoed(found->second.options, { 2, 3, 4, 5, 6, 12, 15, 16, 24, 29, 30, 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'E') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(found->second.client, to_ouch_refused(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending == static_cast<char>('U')) respond(found->second.client, to_ouch_replace_rejected(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'O') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(found->second.client, to_ouch_rejected(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'P') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(found->second.client, to_ouch_rejected_peg(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'T') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(found->second.client, to_ouch_rejected_type(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        if (static_cast<char>(m.refusal_reason.get().value()) == 'C') if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end() && found->second.pending != static_cast<char>('U')) respond(found->second.client, to_ouch_rejected_cross(m, found->second), echoed(found->second.options, { 28 }, { 28 }));
        close(m.order_id.get().value());
    }

    // The engine's order_accepted_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_accepted_message& m) {
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) found->second.price = m.price.get().value();
        if (auto found = orders_.find(m.order_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_order_accepted(found->second.request, found->second), echoed(found->second.options, { 2, 3, 4, 5, 6, 12, 15, 16, 24, 29, 30, 28 }, { 28 }));
    }

    // The engine's order_replaced_message: answered to the client owning each order it names.
    void on_event(const ipc_ns::order_replaced_message& m) {
        if (auto found = orders_.find(m.replacement_id.get().value()); found != orders_.end()) respond(found->second.client, to_ouch_replaced(found->second.replace, found->second), echoed(found->second.options, { 2, 3, 5, 6, 12, 15, 16, 29, 30, 28 }, { 28 }));
        if (auto found = orders_.find(m.replacement_id.get().value()); found != orders_.end()) found->second.remaining = m.open_quantity.get().value();
        if (m.order_id.get().value() != m.replacement_id.get().value()) close(m.order_id.get().value());
    }

private:
    struct Client {
        Gateway& gateway;
        std::size_t index;
        Send send;
        std::string firm{};   // the firm it was admitted under, none until it is
        soupbin::Session<Send, Client> session;

        Client(Gateway& owner, std::size_t at, Send writer) : gateway(owner), index(at), send(std::move(writer)), session("NSMOUCH001", send, *this) {}

        std::optional<soupbin::status> login(auto username, auto password, auto requested, auto) {
            const auto refused = gateway.login(username, password);
            if (!refused) firm = std::string(username);   // no firm is registered for an account: its username stands in for one
            return refused;
        }
        void message(std::string_view packet) { gateway.message(index, packet); }
        void logout() { lost = true; }   // the session is over: logged out, or its login refused
        void silent() { lost = true; }   // heard nothing for longer than the session allows
        bool lost = false;
        bool blocked = false;   // does it take no new orders, having asked not to?
        std::uint64_t last_reference{};   // the last reference a request admitted or replaced an order under
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
        const auto* bytes = reinterpret_cast<const std::byte*>(packet.data());
        auto selector = protocol::unsequenced_data_packet::parse(bytes)->fields.unsequenced_message_type.get().value();
        switch (selector) {
            case static_cast<decltype(selector)>('O'): {
                if (packet.size() < fixed_size<protocol::enter_order_message>()) return;
                const auto& m = *protocol::enter_order_message::parse(bytes);
                if (auto& last = clients_.at(client)->last_reference; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                if (clients_.at(client)->blocked) { respond(client, to_ouch_rejected_disabled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!((static_cast<long long>(m.fields.quantity.get().value()) >= 1 && static_cast<long long>(m.fields.quantity.get().value()) <= 999999))) { respond(client, to_ouch_rejected_quantity(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!((static_cast<long long>(m.fields.price.get().value()) >= 0 && static_cast<long long>(m.fields.price.get().value()) <= 1999999900) || static_cast<long long>(m.fields.price.get().value()) == 2000000000 || static_cast<long long>(m.fields.price.get().value()) == 2147483647)) { respond(client, to_ouch_rejected_price(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(true && static_cast<long long>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 3, 4)).value_or(0)) % 100 == 0)) { respond(client, to_ouch_rejected_min_qty(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(true && static_cast<long long>((option(m.tail_begin(), m.tail_begin() + std::min<std::size_t>(m.fields.appendage_length.get().value(), static_cast<std::size_t>(m.tail_end() - m.tail_begin())), 5, 4)).value_or(0)) % 100 == 0)) { respond(client, to_ouch_rejected_max_floor(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(static_cast<char>(m.fields.side.get().value()) == 'B' || static_cast<char>(m.fields.side.get().value()) == 'S' || static_cast<char>(m.fields.side.get().value()) == 'T' || static_cast<char>(m.fields.side.get().value()) == 'E')) { respond(client, to_ouch_rejected_side(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!(static_cast<char>(m.fields.display.get().value()) == 'Y' || static_cast<char>(m.fields.display.get().value()) == 'N' || static_cast<char>(m.fields.display.get().value()) == 'A')) { respond(client, to_ouch_rejected_display(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                if (!([](std::string_view named) { while (!named.empty() && named.back() == ' ') named.remove_suffix(1); return named == "ZVZZT"; }(m.fields.symbol.get().value()))) { respond(client, to_ouch_rejected_symbol(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 })); return; }
                Order order{ numbered(), static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client };
                order.firm = clients_.at(client)->firm;
                auto ipc = to_ouch_enter_order(m, order);
                order.remaining = ipc.quantity.get().value();
                order.side = static_cast<char>(ipc.side.get().value());   // the side the engine was told
                order.display = static_cast<char>(ipc.display.get().value());   // whether the engine was told to show it
                order.entered = m.fields;
                order.options.assign(appendage(m, packet));
                std::memcpy(&order.request, packet.data(), std::min(packet.size(), sizeof order.request));   // as much of it as arrived
                orders_[order.id] = order;
                references_[{ client, order.reference }] = order.id;
                detail_ipc::emit_framed(emit_, 'N', ipc);
                return;
            }
            case static_cast<decltype(selector)>('X'): {
                if (packet.size() < fixed_size<protocol::cancel_order_message>() - 2) return;   // its last 2 bytes may be left out
                const auto& m = *protocol::cancel_order_message::parse(bytes);
                if (m.fields.quantity.get().value() == 0) {
                    const auto key = std::make_pair(client, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()));
                    const auto found = references_.find(key);
                    if (found == references_.end()) return;
                    Order& order = orders_.at(found->second);
                    order.pending = static_cast<char>('X');
                    auto ipc = to_ouch_cancel_order(m, order);
                    detail_ipc::emit_framed(emit_, 'C', ipc);
                    return;
                }
                if (m.fields.quantity.get().value() != 0) {
                    const auto key = std::make_pair(client, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()));
                    const auto found = references_.find(key);
                    if (found == references_.end()) return;
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
                const auto key = std::make_pair(client, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()));
                const auto found = references_.find(key);
                if (found == references_.end()) return;
                Order& order = orders_.at(found->second);
                order.pending = static_cast<char>('M');
                auto ipc = to_ouch_modify_order(m, order);
                respond(client, to_ouch_modified(m, order), echoed(order.options, { 28 }, { 28 }));
                detail_ipc::emit_framed(emit_, 'D', ipc);
                return;
            }
            case static_cast<decltype(selector)>('U'): {
                if (packet.size() < fixed_size<protocol::replace_order_message>()) return;
                const auto& m = *protocol::replace_order_message::parse(bytes);
                const auto key = std::make_pair(client, static_cast<std::uint64_t>(m.fields.orig_user_ref_num.get().value()));
                const auto found = references_.find(key);
                if (found == references_.end()) return;
                if (auto& last = clients_.at(client)->last_reference; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission, or a number already used
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                const Order& replaced = orders_.at(found->second);
                Order order = replaced;
                order.id = numbered();
                order.reference = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                order.replaced_id = replaced.id;
                order.options = merged(replaced.options, appendage(m, packet), { 25, 26 });
                order.replaced_reference = replaced.reference;
                std::memcpy(&order.replace, packet.data(), std::min(packet.size(), sizeof order.replace));   // as much of it as arrived
                order.pending = static_cast<char>('U');
                auto ipc = to_ouch_replace_order(m, order);
                order.remaining = ipc.quantity.get().value();
                orders_[order.id] = order;
                references_[{ client, order.reference }] = order.id;
                detail_ipc::emit_framed(emit_, 'M', ipc);
                return;
            }
            case static_cast<decltype(selector)>('C'): {
                if (packet.size() < fixed_size<protocol::mass_cancel_request_message>()) return;
                const auto& m = *protocol::mass_cancel_request_message::parse(bytes);
                if (auto& last = clients_.at(client)->last_reference; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                respond(client, to_ouch_mass_canceled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 27, 24, 28 }, { 28 }));
                std::vector<std::uint64_t> taken;   // found first: an order the engine answers at once may close as it is handed over
                for (const auto& [id, order] : orders_) if (order.client == client && (std::string_view(m.fields.symbol.get().value()).find_first_not_of(' ') == std::string_view::npos || std::string_view(m.fields.symbol.get().value()) == std::string_view(order.entered.symbol.get().value())) && (!option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 27, 1) || static_cast<char>(*option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 27, 1)) == static_cast<char>(order.entered.side.get().value())) && (!option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 24, 2) || *option(m.tail_begin(), m.tail_begin() + appendage(m, packet).size(), 24, 2) == option(reinterpret_cast<const std::byte*>(order.options.data()), reinterpret_cast<const std::byte*>(order.options.data()) + order.options.size(), 24, 2).value_or(0))) taken.push_back(id);
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
                if (auto& last = clients_.at(client)->last_reference; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                respond(client, to_ouch_disabled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 }));
                clients_.at(client)->blocked = true;   // it takes no new orders
                return;
            }
            case static_cast<decltype(selector)>('E'): {
                if (packet.size() < fixed_size<protocol::enable_order_entry_request_message>()) return;
                const auto& m = *protocol::enable_order_entry_request_message::parse(bytes);
                (void)m;
                if (auto& last = clients_.at(client)->last_reference; static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()) <= last) return;   // a retransmission
                else last = static_cast<std::uint64_t>(m.fields.user_ref_num.get().value());
                respond(client, to_ouch_enabled(m, Order{ 0, static_cast<std::uint64_t>(m.fields.user_ref_num.get().value()), 0, client }), echoed(appendage(m, packet), { 28 }, { 28 }));
                clients_.at(client)->blocked = false;   // it takes new orders again
                return;
            }
            case static_cast<decltype(selector)>('Q'): {
                if (packet.size() < fixed_size<protocol::account_query_message>() - 2) return;   // its last 2 bytes may be left out
                const auto& m = *protocol::account_query_message::parse(bytes);
                (void)m;
                respond(client, to_ouch_account_query(m, Order{ 0, clients_.at(client)->last_reference + 1, 0, client }), echoed((packet.size() >= fixed_size<protocol::account_query_message>() ? appendage(m, packet) : std::string_view{}), { 28 }, { 28 }));
                return;
            }
            default:
                return;   // a message the order entry does not map
        }
    }

    // A response to a client, as the body of the message its session publishes.
    template <typename Response>
    void respond(std::size_t client, const Response& response) {
        const auto* bytes = reinterpret_cast<const char*>(&response) + sizeof(protocol::server_packet_header);
        // A client that has gone is not written to: the order it left may still trade, and what
        // the venue would have told it goes nowhere.
        const auto held = clients_.find(client);
        if (held == clients_.end()) { return; }

        held->second->session.sequenced(bytes, response.header.packet_length.get().value() + 2 - sizeof(protocol::server_packet_header));
    }

    // A response with options in its appendage: written after its fixed part, as much as its
    // message has room for, and counted by its appendage's length and by its packet's.
    template <typename Response>
    void respond(std::size_t client, Response response, std::string_view options) {
        const std::size_t size = std::min<std::size_t>(options.size(), Response::tail_capacity);
        std::memcpy(response.tail, options.data(), size);
        response.fields.appendage_length.set(static_cast<std::remove_cvref_t<decltype(response.fields.appendage_length.get().value())>>(size));
        response.header.packet_length.set(static_cast<std::remove_cvref_t<decltype(response.header.packet_length.get().value())>>(response.header.packet_length.get().value() + size));
        respond(client, response);
    }

    // Shares executed on an order; a filled order is no longer open.
    void fill(std::uint64_t id, std::uint64_t shares) {
        auto found = orders_.find(id);
        if (found == orders_.end()) return;
        found->second.remaining -= std::min(shares, found->second.remaining);
        if (found->second.remaining == 0) close(id);
    }

    void close(std::uint64_t id) {
        auto found = orders_.find(id);
        if (found == orders_.end()) return;
        references_.erase({ found->second.client, found->second.reference });
        orders_.erase(found);
    }

    // The number of the next order admitted, and the gateway's next after it.
    std::uint64_t numbered() {
        const std::uint64_t id = next_id_;
        next_id_ += step_;
        return id;
    }

    Emit emit_;
    std::map<std::size_t, std::unique_ptr<Client>> clients_;
    std::size_t next_client_ = 0;   // session numbers, never reused
    std::map<std::string, std::string, std::less<>> credentials_;
    std::map<std::uint64_t, Order> orders_;
    std::map<std::pair<std::size_t, std::uint64_t>, std::uint64_t> references_;
    std::uint64_t next_id_;   // the number the next order admitted is given
    std::uint64_t step_;      // how many gateways number in turn
};

}
