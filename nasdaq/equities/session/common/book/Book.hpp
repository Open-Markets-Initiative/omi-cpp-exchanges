#pragma once

#include "State.hpp"

namespace book {

// books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}, below the side.
// Templated because the sides are DIFFERENT types - the comparator differs - so one
// body serves both, and the branch above says which it is called with.
template <typename Held>
inline Order* Equities::rest_order_into(Held& held, Price price, Quantity size) {
    Level& level = held.levels_open(price);
    Order* placed = order_pool_.take();
    level.orders().rest(placed);
    placed->size(size);
    level.total(static_cast<Quantity>(level.total() + size));
    level.count(level.count() + 1);

    return placed;
}

// books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: rest a new member at the tail of its queue
inline Order* Equities::rest_order(Key key, Side side, Price price, Quantity size) {
    Book& book = books_open(key);

    // the sides are different types, so this BRANCHES rather than selecting one
    if (side == Side::Bid) {
        return rest_order_into(book.bids(), price, size);
    }
    if (side == Side::Ask) {
        return rest_order_into(book.asks(), price, size);
    }

    // a value the enum does not name reaches no side. Reading it as the last mapping
    // instead is what put one venue's implied bids and book resets into the offers
    return nullptr;
}

// books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}, below the side.
// Templated because the sides are DIFFERENT types - the comparator differs - so one
// body serves both, and the branch above says which it is called with.
template <typename Held>
inline void Equities::discard_order_from(Held& held, Price price, Order* node) {
    Level* level = held.levels_locate(price);

    if (level == nullptr) {
        return;
    }

    level->total(static_cast<Quantity>(level->total() - node->size()));
    level->count(level->count() - 1);
    level->orders().remove(node);
    order_pool_.give(node);

    // the step the queue hung off goes with it: a ladder derives its best price from
    // its first entry, so one holding nothing would answer as the touch
    if (level->orders().empty()) {
        held.levels_close(price);
    }
}

// books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: take the member a routing index resolved out of its queue
inline void Equities::discard_order(const Routing& routed) {
    Book* book = books_locate(routed.key());

    if (book == nullptr) {
        return;
    }

    // the sides are different types, so this BRANCHES rather than selecting one
    if (routed.side() == Side::Bid) {
        discard_order_from(book->bids(), routed.price(), routed.order());
        return;
    }
    if (routed.side() == Side::Ask) {
        discard_order_from(book->asks(), routed.price(), routed.order());
        return;
    }
}

// books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}, below the side.
// Templated because the sides are DIFFERENT types - the comparator differs - so one
// body serves both, and the branch above says which it is called with.
template <typename Held>
inline void Equities::draw_order_in(Held& held, Price price, Order* node, Quantity by) {
    Level* level = held.levels_locate(price);

    if (level == nullptr) {
        return;
    }

    node->size(static_cast<Quantity>(node->size() - by));
    level->total(static_cast<Quantity>(level->total() - by));
}

// books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: draw a located member down, and the total its holder keeps with it
inline void Equities::draw_order(const Routing& routed, Quantity by) {
    Book* book = books_locate(routed.key());

    if (book == nullptr) {
        return;
    }

    // the sides are different types, so this BRANCHES rather than selecting one
    if (routed.side() == Side::Bid) {
        draw_order_in(book->bids(), routed.price(), routed.order(), by);
        return;
    }
    if (routed.side() == Side::Ask) {
        draw_order_in(book->asks(), routed.price(), routed.order(), by);
        return;
    }
}

// OrderAdd: 4 statement(s)
inline void Equities::order_add(OrderId orderId, Key key, Side side, Price price, Quantity qty) {
    // books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: rest a new member at the tail of its queue
    Order* order_placed = rest_order(key, side, price, qty);

    // routing/{OrderId}: the id now routes to the member just rested
    Routing& routing_entry = routing_open(orderId);
    routing_entry.order(order_placed);
    routing_entry.key(key);
    routing_entry.side(side);
    routing_entry.price(price);
}

// OrderDelete: 7 statement(s)
inline OrderDeleteReport Equities::order_delete(OrderId orderId) {
    // what this reports, filled in where each output's statement stands
    OrderDeleteReport report;

    // routing/{OrderId}: the subject, resolved by the id the message carries
    Routing* routed = routing_locate(orderId);

    if (routed == nullptr) {
        return report;   // Ignore="true": routes nowhere
    }

    if (routed->order() == nullptr) {
        return report;   // the entry points at no member, so it resolves to nothing
    }

    // bound: Key, copied out before any statement can discard the entry
    const Key key_held = routed->key();

    // bound: Side, copied out before any statement can discard the entry
    const Side side_held = routed->side();

    // bound: Price, copied out before any statement can discard the entry
    const Price price_held = routed->price();

    // bound: Quantity, the member's resting quantity, copied out before any statement can change it
    const Quantity quantity_held = routed->order()->size();

    // Key: reported as it stands here
    report.key = key_held;

    // OrderSide: reported as it stands here
    report.order_side = side_held;

    // OrderPrice: reported as it stands here
    report.order_price = price_held;

    // OrderQty: reported as it stands here
    report.order_qty = quantity_held;

    // books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: the member the subject resolved to leaves its queue
    discard_order(*routed);

    // routing/{OrderId}: the id no longer routes
    routing_close(orderId);

    report.reported = true;

    return report;
}

// OrderExecute: 6 statement(s)
inline OrderExecuteReport Equities::order_execute(OrderId orderId, Quantity qty) {
    // what this reports, filled in where each output's statement stands
    OrderExecuteReport report;

    // routing/{OrderId}: the subject, resolved by the id the message carries
    Routing* routed = routing_locate(orderId);

    if (routed == nullptr) {
        return report;   // Ignore="true": routes nowhere
    }

    if (routed->order() == nullptr) {
        return report;   // the entry points at no member, so it resolves to nothing
    }

    // bound: Key, copied out before any statement can discard the entry
    const Key key_held = routed->key();

    // bound: Side, copied out before any statement can discard the entry
    const Side side_held = routed->side();

    // bound: Price, copied out before any statement can discard the entry
    const Price price_held = routed->price();

    // bound: Quantity, the member's resting quantity, copied out before any statement can change it
    const Quantity quantity_held = routed->order()->size();

    // Key: reported as it stands here
    report.key = key_held;

    // OrderSide: reported as it stands here
    report.order_side = side_held;

    // OrderPrice: reported as it stands here
    report.order_price = price_held;

    // OrderQty: reported as it stands here
    report.order_qty = quantity_held;

    // books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: draw the member down by Qty
    Order* order = routed->order();

    if (order->size() > qty) {
        draw_order(*routed, qty);
    } else {
        // Exhausted="Discard": a fill for at least the resting size takes the whole member,
        // and no delete follows it on the wire
        discard_order(*routed);

        // and the id no longer routes: the member it resolved to is gone, and the
        // declaration has no verb for cascading one discard into its index
        routing_close(orderId);
    }

    report.reported = true;

    return report;
}

// OrderReplace: 9 statement(s)
inline OrderReplaceReport Equities::order_replace(OrderId orderId, OrderId newOrderId, Price price, Quantity qty) {
    // what this reports, filled in where each output's statement stands
    OrderReplaceReport report;

    // routing/{OrderId}: the subject, resolved by the id the message carries
    Routing* routed = routing_locate(orderId);

    if (routed == nullptr) {
        return report;   // Ignore="true": routes nowhere
    }

    if (routed->order() == nullptr) {
        return report;   // the entry points at no member, so it resolves to nothing
    }

    // bound: Key, copied out before any statement can discard the entry
    const Key key_held = routed->key();

    // bound: Side, copied out before any statement can discard the entry
    const Side side_held = routed->side();

    // Key: reported as it stands here
    report.key = key_held;

    // OrderSide: reported as it stands here
    report.order_side = side_held;

    // books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: the member the subject resolved to leaves its queue
    discard_order(*routed);

    // routing/{OrderId}: the id no longer routes
    routing_close(orderId);

    // books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}: rest a new member at the tail of its queue
    Order* order_placed = rest_order(key_held, side_held, price, qty);

    // routing/{OrderId}: the id now routes to the member just rested
    Routing& routing_entry = routing_open(newOrderId);
    routing_entry.order(order_placed);
    routing_entry.key(key_held);
    routing_entry.side(side_held);
    routing_entry.price(price);
    report.reported = true;

    return report;
}

// ClearBook: 1 statement(s)
inline void Equities::clear_book(Key key) {
    // books/{Key}: everything resting in this member is void
    // walked off the index, the one place that knows every member and where it rests
    for (auto entry = routing_.begin(); entry != routing_.end();) {
        const auto& swept = entry->second;

        if (!(swept.key() == key)) {
            ++entry;
            continue;
        }

        // an entry that points at no member has nothing to take out of a queue
        if (swept.order() != nullptr) {
            discard_order(swept);
        }

        entry = routing_.erase(entry);
    }
}

// ClearBooks: 1 statement(s)
inline void Equities::clear_books() {
    // books/{Key}: everything resting in every member is void
    // walked off the index, the one place that knows every member and where it rests
    for (auto entry = routing_.begin(); entry != routing_.end();) {
        const auto& swept = entry->second;

        // an entry that points at no member has nothing to take out of a queue
        if (swept.order() != nullptr) {
            discard_order(swept);
        }

        entry = routing_.erase(entry);
    }
}

// AuctionCross: 1 statement(s)
inline void Equities::auction_cross(Key key, Price price) {
    // books/{Key}/sides/{Side}/levels/{Price}: every level better than Price is gone, on every Side
    // walked off the index, the one place that knows every member and where it rests
    for (auto entry = routing_.begin(); entry != routing_.end();) {
        const auto& swept = entry->second;

        if (!(swept.key() == key && (swept.side() == Side::Bid ? swept.price() > price : swept.price() < price))) {
            ++entry;
            continue;
        }

        // an entry that points at no member has nothing to take out of a queue
        if (swept.order() != nullptr) {
            discard_order(swept);
        }

        entry = routing_.erase(entry);
    }
}

}
