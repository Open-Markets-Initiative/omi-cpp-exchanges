#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <map>
#include <unordered_map>

#include "Types.hpp"

namespace book {

// Declared ahead of the definitions: the definitions nest innermost-first, and a
// routing index points across the tree rather than down it.
class Book;
template <typename Compare> class BookSide;
class Level;
class Order;
class Routing;
class Equities;

// Where a queue's members live. ONE pool for every queue of a kind, held by the book,
// so a level that appears and empties costs no allocation: a member is taken from
// here as it rests and given back as it leaves. A std::deque because a reference
// into one survives a push_back, which is what makes a routing index safe to point
// into it.
template <typename Member>
class Pool {
public:
    // storage for one member, reused from one given back before the pool grows
    Member* take() {
        if (recycled_ != nullptr) {
            Member* node = recycled_;
            recycled_ = node->next_;
            *node = Member{};
            return node;
        }

        storage_.emplace_back();
        return &storage_.back();
    }

    // a member that has left its queue, kept for the next take()
    void give(Member* node) {
        node->prior_ = nullptr;
        node->next_ = recycled_;
        recycled_ = node;
    }

    // how much storage is held, which a test reads to see that giving members back
    // bounds it rather than growing by every member that ever rested
    std::size_t held() const { return storage_.size(); }

private:
    std::deque<Member> storage_;
    Member* recycled_ = nullptr;
};

// An intrusive FIFO, allocating Fifo.
// The members hold each other, so the queue itself is two pointers and a queued
// member costs no allocation of its own.
template <typename Member>
class Queue {
public:
    // rest a member at the tail
    void rest(Member* node) {
        node->prior_ = tail_;
        node->next_ = nullptr;

        if (tail_ != nullptr) {
            tail_->next_ = node;
        } else {
            head_ = node;
        }

        tail_ = node;
    }

    // take one member out of the queue
    void remove(Member* node) {
        if (node == nullptr) {
            return;
        }

        if (node->prior_ != nullptr) {
            node->prior_->next_ = node->next_;
        } else {
            head_ = node->next_;
        }

        if (node->next_ != nullptr) {
            node->next_->prior_ = node->prior_;
        } else {
            tail_ = node->prior_;
        }
    }

    bool empty() const { return head_ == nullptr; }

    // the member at the front, which is the one a Fifo allocation fills first
    const Member* front() const { return head_; }

private:
    Member* head_ = nullptr;
    Member* tail_ = nullptr;
};

// routing/{OrderId}
// addressed Key on OrderId
class Routing {
public:
    // the member this index resolves to: routing is Of="Order", as orders is
    Order* order() const { return order_; }
    void order(Order* value) { order_ = value; }

    // key: cleared with the book
    Key key() const { return key_; }
    void key(Key value) { key_ = value; }

    // side: cleared with the book
    Side side() const { return side_; }
    void side(Side value) { side_ = value; }

    // price: cleared with the book
    Price price() const { return price_; }
    void price(Price value) { price_ = value; }

private:
    Order* order_ = nullptr;
    Key key_ = {};
    Side side_ = {};
    Price price_ = {};
};

// books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}
// addressed Insertion, allocating Fifo
class Order {
public:
    // the declaration states no fields for a Order; the quantity it
    // rests with is all a book can hold without them
    Quantity size() const { return size_; }
    void size(Quantity value) { size_ = value; }

    // the member queued behind this one, for a reader walking in arrival order
    const Order* behind() const { return next_; }

private:
    Quantity size_ = 0;

    // the queue's links, the container's rather than the declaration's. Private to
    // the member and reachable only by the queue that owns it, so nothing can leave
    // the chain walking correctly forwards and losing a member backwards
    Order* next_ = nullptr;
    Order* prior_ = nullptr;

    template <typename Member> friend class Queue;
    template <typename Member> friend class Pool;
};

// books/{Key}/sides/{Side}/levels/{Price}
// addressed Key on Price, walked BySide
class Level {
public:
    // total: the total its members rest with, kept as they rest, fill and leave
    Quantity total() const { return total_; }
    void total(Quantity value) { total_ = value; }

    // count: how many members rest here, kept as they rest and leave
    std::size_t count() const { return count_; }
    void count(std::size_t value) { count_ = value; }

    // books/{Key}/sides/{Side}/levels/{Price}/orders/{Arrival}
    Queue<Order>& orders() { return orders_; }
    const Queue<Order>& orders() const { return orders_; }

private:
    Quantity total_ = 0;
    std::size_t count_ = 0;
    Queue<Order> orders_;
};

// books/{Key}/sides/{Side}
// addressed Key on Side
template <typename Compare>
class BookSide {
public:
    // books/{Key}/sides/{Side}/levels/{Price}
    const std::map<Price, Level, Compare>& levels() const { return levels_; }

    Level& levels_open(Price at) { return levels_[at]; }
    Level* levels_locate(Price at) {
        const auto found = levels_.find(at);
        return found == levels_.end() ? nullptr : &found->second;
    }
    void levels_close(Price at) { levels_.erase(at); }

private:
    std::map<Price, Level, Compare> levels_;
};

// books/{Key}
// addressed Key on Key
class Book {
public:
    // books/{Key}/sides/{Side}
    BookSide<std::greater<Price>>& bids() { return bids_; }
    const BookSide<std::greater<Price>>& bids() const { return bids_; }
    BookSide<std::less<Price>>& asks() { return asks_; }
    const BookSide<std::less<Price>>& asks() const { return asks_; }

private:
    BookSide<std::greater<Price>> bids_;
    BookSide<std::less<Price>> asks_;
};

// What order_delete reports: the outputs OrderDelete declares, each read where its
// statement stands. reported is false where the operation ended before it reached them -
// an id the book never registered routes nowhere and reports nothing.
struct OrderDeleteReport {
    bool reported = false;
    Key key{};   // Key
    Side order_side{};   // OrderSide
    Price order_price{};   // OrderPrice
    Quantity order_qty{};   // OrderQty
};

// What order_execute reports: the outputs OrderExecute declares, each read where its
// statement stands. reported is false where the operation ended before it reached them -
// an id the book never registered routes nowhere and reports nothing.
struct OrderExecuteReport {
    bool reported = false;
    Key key{};   // Key
    Side order_side{};   // OrderSide
    Price order_price{};   // OrderPrice
    Quantity order_qty{};   // OrderQty
};

// What order_replace reports: the outputs OrderReplace declares, each read where its
// statement stands. reported is false where the operation ended before it reached them -
// an id the book never registered routes nowhere and reports nothing.
struct OrderReplaceReport {
    bool reported = false;
    Key key{};   // Key
    Side order_side{};   // OrderSide
};

// The book itself: every collection the model hangs off its root, every scalar it
// holds per listing, and the operations a feed may ask of it.
class Equities {
public:
    // What a message does to the book. Defined in Book.hpp, which is generated from
    // the statements each operation declares.
    void order_add(OrderId orderId, Key key, Side side, Price price, Quantity qty);
    OrderDeleteReport order_delete(OrderId orderId);
    OrderExecuteReport order_execute(OrderId orderId, Quantity qty);
    OrderReplaceReport order_replace(OrderId orderId, OrderId newOrderId, Price price, Quantity qty);
    void clear_book(Key key);
    void clear_books();
    void auction_cross(Key key, Price price);

    // books/{Key}
    const std::unordered_map<Key, Book>& books() const { return books_; }

    // routing/{OrderId}
    const std::unordered_map<OrderId, Routing>& routing() const { return routing_; }

    // how much storage the book holds for orders, bounded by members being given back
    std::size_t order_pooled() const { return order_pool_.held(); }

private:
    std::unordered_map<Key, Book> books_;
    std::unordered_map<OrderId, Routing> routing_;
    Pool<Order> order_pool_;

    // The ways in. Private, because an operation changes the book and nothing else
    // does: open creates where the address says nothing is, locate finds without
    // creating, and close takes a member out.
    Book& books_open(Key at) { return books_[at]; }
    Book* books_locate(Key at) {
        const auto found = books_.find(at);
        return found == books_.end() ? nullptr : &found->second;
    }

    Routing& routing_open(OrderId at) { return routing_[at]; }
    Routing* routing_locate(OrderId at) {
        const auto found = routing_.find(at);
        return found == routing_.end() ? nullptr : &found->second;
    }
    void routing_close(OrderId at) { routing_.erase(at); }

    // The routed paths, written once each rather than inlined into every operation
    // that walks them. Defined in Book.hpp.
    template <typename Held> Order* rest_order_into(Held& held, Price price, Quantity size);
    Order* rest_order(Key key, Side side, Price price, Quantity size);
    template <typename Held> void discard_order_from(Held& held, Price price, Order* node);
    void discard_order(const Routing& routed);
    template <typename Held> void draw_order_in(Held& held, Price price, Order* node, Quantity by);
    void draw_order(const Routing& routed, Quantity by);
};

}
