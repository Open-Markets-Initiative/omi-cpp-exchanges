#pragma once

// Generated Nasdaq Stock Market Session priority check: a captured day of the venue's TotalView feed, every
// order it showed held in the engine's book, and each execution it reported set beside the order the
// engine would have traded first.
//
// Each message of Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 the book model Nasdaq.NsmEquities.Equities binds to an operation is read
// through the feed's own structs and applied to the engine of the security it names. An execution -
// a message the feed's model marks Trade: Execution - is checked before its shares are taken: the
// engine's first order on the executed order's side, the first at the best price, is the one an
// arriving order would have met. What the feed never shows (hidden and reserve size, midpoint pegs,
// on-open and on-close orders) trades without an order on it, so an execution of a shown order behind
// such interest is no fault of the engine's; the report counts each kind of disagreement for that.

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../book/Types.hpp"
#include "../matching/MatchingEngine.hpp"
#include "../protocols/marketdata/totalview/structs/PacketHeader.hpp"
#include "../protocols/marketdata/totalview/structs/MessageHeader.hpp"
#include "../protocols/marketdata/totalview/messages/AddOrderNoMpidAttributionMessage.hpp"
#include "../protocols/marketdata/totalview/messages/AddOrderWithMpidAttributionMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderExecutedMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderExecutedWithPriceMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderCancelMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderDeleteMessage.hpp"
#include "../protocols/marketdata/totalview/messages/OrderReplaceMessage.hpp"

namespace sim::state_check {

namespace feed = ::nasdaq::nsmequities::totalview::itch::v5_0_2026;

// What the engine is told of a replayed book: nothing, since nothing in it trades.
struct Unheard {
    template <typename... Told>
    void operator()(const Told&...) const {}
};

using Engine = matching::MatchingEngine<Unheard, Unheard>;

// Each kind of execution, and how the venue's choice of order stood against the engine's.
struct Tally {
    std::uint64_t executions = 0;   // the venue's executions of a resting order
    std::uint64_t first      = 0;   // of the order the engine would have traded first
    std::uint64_t behind     = 0;   // of an order at the best price, behind others there
    std::uint64_t ahead      = 0;   // the orders that stood ahead of those, all told
    std::uint64_t worse      = 0;   // of an order at a worse price than the best on its side
    std::uint64_t absent     = 0;   // of an order the engine's book does not hold
};

// One execution where the venue took another order than the engine would have.
struct Difference {
    std::size_t   kind;
    book::Key key;
    std::uint64_t taken;      // the order the venue executed
    std::uint64_t first;      // the order the engine would have traded first
    std::int64_t  price;      // the executed order's price, in the book's own scale
    std::int64_t  best;       // the best price on its side
    std::size_t   ahead;      // the orders ahead of it at its price
};

struct Report {
    std::uint64_t packets  = 0;   // the feed's packets read
    std::uint64_t repeated = 0;   // of them, already applied: the other feed's copy
    std::uint64_t gaps     = 0;   // packets that began past the next message expected
    std::uint64_t sessions = 0;   // the feed's sessions, each a day of its own
    std::uint64_t applied  = 0;   // the messages that applied an operation
    std::uint64_t unknown  = 0;   // of them, for an order the engine's book does not hold
    std::array<Tally, 2> kinds{};
};

// The names of the kinds of execution, as the feed's model names the messages.
inline constexpr std::array<std::string_view, 2> kinds{ { "Order Executed Message", "Order Executed With Price Message" } };

class PriorityCheck {
public:
    // One packet of the feed as it was sent, walked as 'Packet' lays it out. A
    // packet of another session begins another day; one already applied - the other feed's copy -
    // is passed over.
    void packet(const std::byte* data, std::size_t size) {
        if (size < sizeof(feed::packet_header)) return;
        const auto* head = feed::packet_header::parse(data);
        ++report_.packets;
        const std::string_view named = head->session.get().value();
        if (named != session_) {
            session_.assign(named.data(), named.size());
            engines_.clear();
            held_.clear();
            next_ = 0;
            ++report_.sessions;
        }
        const std::uint64_t first = head->sequence_number.get().value();
        const std::size_t messages = head->message_count.get().value();
        if (messages == 65535) return;   // the session's end, said by the count: no message follows
        if (messages == 0) return;
        if (next_ != 0 && first < next_) { ++report_.repeated; return; }
        if (next_ != 0 && first > next_) ++report_.gaps;
        next_ = first + messages;
        std::size_t at = sizeof(feed::packet_header);
        for (std::size_t i = 0; i < messages; ++i) {
            if (at + sizeof(feed::message_header) > size) return;
            const auto* framed = feed::message_header::parse(data + at);
            const std::size_t block = 0 + sizeof(framed->message_length) + framed->message_length.get().value();
            if (at + block > size) return;
            apply(static_cast<char>(framed->message_type.get().value()), data + at);
            at += block;
        }
    }

    const Report& report() const { return report_; }
    const std::vector<Difference>& differences() const { return differences_; }

private:
    // One order as the engine's book holds it: where it stands, and what is left of it.
    struct Held {
        book::Key key;
        matching::Side side;
        std::int64_t price;
        std::uint64_t shares;
    };

    // One message: the operation the book model binds it to, or nothing where it binds none.
    void apply(char kind, const std::byte* bytes) {
        switch (kind) {
            case 'A': {   // Add Order No Mpid Attribution Message: OrderAdd
                const auto& m = *feed::add_order_no_mpid_attribution_message::parse(bytes);
                if (!order_add(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Key>(m.fields.stock_locate.get().value()), static_cast<char>(m.fields.buy_sell_indicator.get().value()) == 'B' ? book::Side::Bid : book::Side::Ask, static_cast<book::Price>(m.fields.price.get().value()) * book::price_scale(4), static_cast<book::Quantity>(m.fields.shares.get().value()))) ++report_.unknown;   // for an order the engine's book does not hold
                ++report_.applied;
                break;
            }
            case 'F': {   // Add Order With Mpid Attribution Message: OrderAdd
                const auto& m = *feed::add_order_with_mpid_attribution_message::parse(bytes);
                if (!order_add(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Key>(m.fields.stock_locate.get().value()), static_cast<char>(m.fields.buy_sell_indicator.get().value()) == 'B' ? book::Side::Bid : book::Side::Ask, static_cast<book::Price>(m.fields.price.get().value()) * book::price_scale(4), static_cast<book::Quantity>(m.fields.shares.get().value()))) ++report_.unknown;   // for an order the engine's book does not hold
                ++report_.applied;
                break;
            }
            case 'E': {   // Order Executed Message: OrderExecute
                const auto& m = *feed::order_executed_message::parse(bytes);
                checked(0, static_cast<book::OrderId>(m.fields.order_reference_number.get().value()));
                if (!order_execute(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Quantity>(m.fields.executed_shares.get().value()))) ++report_.unknown;   // for an order the engine's book does not hold
                ++report_.applied;
                break;
            }
            case 'C': {   // Order Executed With Price Message: OrderExecute
                const auto& m = *feed::order_executed_with_price_message::parse(bytes);
                checked(1, static_cast<book::OrderId>(m.fields.order_reference_number.get().value()));
                if (!order_execute(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Quantity>(m.fields.executed_shares.get().value()))) ++report_.unknown;   // for an order the engine's book does not hold
                ++report_.applied;
                break;
            }
            case 'X': {   // Order Cancel Message: OrderExecute
                const auto& m = *feed::order_cancel_message::parse(bytes);
                if (!order_execute(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()), static_cast<book::Quantity>(m.fields.canceled_shares.get().value()))) ++report_.unknown;   // for an order the engine's book does not hold
                ++report_.applied;
                break;
            }
            case 'D': {   // Order Delete Message: OrderDelete
                const auto& m = *feed::order_delete_message::parse(bytes);
                if (!order_delete(static_cast<book::OrderId>(m.fields.order_reference_number.get().value()))) ++report_.unknown;   // for an order the engine's book does not hold
                ++report_.applied;
                break;
            }
            case 'U': {   // Order Replace Message: OrderReplace
                const auto& m = *feed::order_replace_message::parse(bytes);
                if (!order_replace(static_cast<book::OrderId>(m.fields.original_order_reference_number.get().value()), static_cast<book::OrderId>(m.fields.new_order_reference_number.get().value()), static_cast<book::Price>(m.fields.price.get().value()) * book::price_scale(4), static_cast<book::Quantity>(m.fields.shares.get().value()))) ++report_.unknown;   // for an order the engine's book does not hold
                ++report_.applied;
                break;
            }
            default: break;   // a message the book model binds to no operation
        }
    }

    // The engine of the security a key names, made as the feed first shows an order in it.
    Engine& engine_of(book::Key key) {
        auto& engine = engines_[key];
        if (!engine) engine = std::make_unique<Engine>(Unheard{}, Unheard{});
        return *engine;
    }

    // An execution of a resting order, set beside the order the engine would have traded first on its side.
    void checked(std::size_t kind, book::OrderId id) {
        Tally& tally = report_.kinds[kind];
        ++tally.executions;
        const auto held = held_.find(id);
        if (held == held_.end()) { ++tally.absent; return; }
        const Engine& engine = engine_of(held->second.key);
        const matching::Order* first = engine.front(held->second.side);
        if (first != nullptr && first->id == id) { ++tally.first; return; }
        const auto best = engine.best(held->second.side);
        const std::size_t ahead = engine.ahead(held->second.side, held->second.price, id);
        if (best && *best == held->second.price) { ++tally.behind; tally.ahead += ahead; }
        else ++tally.worse;
        if (differences_.size() < 40) {
            differences_.push_back(Difference{ kind, held->second.key, id, first != nullptr ? first->id : 0, held->second.price, best ? *best : 0, ahead });
        }
    }

    // An order the venue showed, placed in its security's engine as the engine places a shown order.
    bool order_add(book::OrderId id, book::Key key, book::Side side, book::Price price, book::Quantity shares) {
        matching::Order order{};
        order.id = id;
        order.side = side == book::Side::Bid ? matching::Side::Buy : matching::Side::Sell;
        order.price = price;
        order.qty = shares;
        if (!held_.emplace(id, Held{ key, order.side, price, shares }).second) return false;
        engine_of(key).place_shown(order);
        return true;
    }
    // Shares the venue took of an order - executed or canceled - where it stands, and gone once none are left.
    bool order_execute(book::OrderId id, book::Quantity shares) {
        const auto held = held_.find(id);
        if (held == held_.end()) return false;
        engine_of(held->second.key).take_shown(held->second.side, held->second.price, id, shares);
        if (held->second.shares <= shares) held_.erase(held);
        else held->second.shares -= shares;
        return true;
    }
    // An order the venue took out whole.
    bool order_delete(book::OrderId id) {
        const auto held = held_.find(id);
        if (held == held_.end()) return false;
        engine_of(held->second.key).take_shown(held->second.side, held->second.price, id, held->second.shares);
        held_.erase(held);
        return true;
    }
    // An order the venue replaced: the old one out, the new one in at the back of its price, as a new order.
    bool order_replace(book::OrderId id, book::OrderId renamed, book::Price price, book::Quantity shares) {
        const auto held = held_.find(id);
        if (held == held_.end()) return false;
        const Held was = held->second;
        engine_of(was.key).take_shown(was.side, was.price, id, was.shares);
        held_.erase(held);
        matching::Order order{};
        order.id = renamed;
        order.side = was.side;
        order.price = price;
        order.qty = shares;
        if (!held_.emplace(renamed, Held{ was.key, was.side, price, shares }).second) return false;
        engine_of(was.key).place_shown(order);
        return true;
    }
    std::string session_;
    std::uint64_t next_ = 0;   // the sequence number the next message is expected under
    std::unordered_map<book::Key, std::unique_ptr<Engine>> engines_;
    std::unordered_map<book::OrderId, Held> held_;
    Report report_;
    std::vector<Difference> differences_;
};

}
