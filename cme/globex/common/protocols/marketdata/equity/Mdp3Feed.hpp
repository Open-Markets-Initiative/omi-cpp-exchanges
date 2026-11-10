#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <map>
#include <unordered_map>
#include "messages/MdIncrementalRefreshBook.hpp"
#include "messages/MdIncrementalRefreshOrderBook.hpp"
#include "messages/MdIncrementalRefreshTradeSummary.hpp"
#include "PacketPublisher.hpp"

namespace sim::equity {

namespace mdp3 = ::cme::mdp3::v13;

// Cme Globex's market data for one channel: the book of each instrument it carries, kept from the
// engine's events, and each change to it written as MDP 3.0 incremental refreshes through the
// channel's packet publisher. An event's messages go out in one packet, the last of them marked as
// ending the event.
//
// Not yet published: implied prices, statistics, security status, and the instrument definitions
// and snapshots a late joiner recovers by. Every order shows all it has: there is no display
// quantity to hide the rest of.
template <typename Publisher>
class Mdp3Feed {
public:
    explicit Mdp3Feed(Publisher& publisher) : publisher_(publisher) {}

    // How far the book is published: MDP 3.0 publishes ten price levels a side.
    static constexpr std::size_t depth = 10;

    // An order rested: its level gains it, and it goes out as the level's update and its own.
    void rested(std::int32_t security, std::uint64_t order, bool buy, std::int64_t price, std::uint64_t qty, std::uint64_t now) {
        auto& book = books_[security];
        orders_[order] = Resting{ security, buy, price, qty };
        const auto join = [&](auto& levels) {
            const auto [at, opened] = levels.try_emplace(price);
            at->second.qty += qty;
            at->second.orders += 1;
            update(book, security, buy, price, at->second, opened ? Action::New : Action::Change, order, qty, Action::New, now, true);
        };
        if (buy) join(book.bids); else join(book.asks);
        publisher_.flush();
    }

    // An order left the book: its level loses what it had, and is deleted when nothing is left at it.
    void canceled(std::uint64_t order, std::uint64_t now) {
        const auto found = orders_.find(order);
        if (found == orders_.end()) return;   // an order this feed never published
        const Resting resting = found->second;
        orders_.erase(found);
        auto& book = books_[resting.security];
        lower(book, resting, resting.qty, order, now, true);
        publisher_.flush();
    }

    // A fill: the trade, the resting order it filled, and the book as the fill leaves it - one event,
    // in that order. The aggressor's side is the one the resting order is not on.
    void traded(std::uint64_t resting_order, std::uint64_t execution, std::int64_t price, std::uint64_t qty, std::uint64_t now) {
        const auto found = orders_.find(resting_order);
        if (found == orders_.end()) return;   // an order this feed never published
        const Resting resting = found->second;
        auto& book = books_[resting.security];

        mdp3::md_incremental_refresh_trade_summary out{};
        out.fields.transact_time.set(now);
        out.fields.match_event_indicator.set_last_trade_msg(true);
        const auto frame = mdp3::start_group_write(out).start_incremental_refresh_trade_summary().md_entry_px_ex(static_cast<std::int64_t>(price)).md_entry_size_short(static_cast<std::int32_t>(qty)).security_id(static_cast<std::int32_t>(resting.security)).rpt_seq(static_cast<std::uint32_t>(++book.rpt_seq)).number_of_orders(static_cast<std::int32_t>(2)).aggressor_side(aggressor_side_of(!resting.buy)).md_update_action(md_update_action_of(Action::New)).md_trade_entry_optional(static_cast<typename mdp3::md_trade_entry_optional::result_type>(execution)).padding_2({}).end_incremental_refresh_trade_summary().start_incremental_refresh_trade_summary_order_id().order_id(static_cast<std::uint64_t>(resting_order)).last_qty(static_cast<std::int32_t>(qty)).padding_4({}).end_incremental_refresh_trade_summary_order_id();
        publisher_.publish_message(reinterpret_cast<const char*>(frame.data()), frame.size());

        if (qty >= resting.qty) orders_.erase(found);
        else found->second.qty -= qty;
        lower(book, resting, qty, resting_order, now, true);
        publisher_.flush();
    }

private:
    struct Level {
        std::uint64_t qty = 0;
        std::uint32_t orders = 0;
    };

    struct Book {
        std::map<std::int64_t, Level, std::greater<std::int64_t>> bids;   // best first
        std::map<std::int64_t, Level>                            asks;   // best first
        std::uint32_t rpt_seq = 0;                                       // numbers every update to it
    };

    struct Resting {
        std::int32_t  security = 0;
        bool          buy = false;
        std::int64_t  price = 0;
        std::uint64_t qty = 0;
    };

    // A level's place among its side's levels, best first and counted from 1.
    std::size_t place(const Book& book, bool buy, std::int64_t price) const {
        return 1 + static_cast<std::size_t>(buy
            ? std::distance(book.bids.begin(), book.bids.find(price))
            : std::distance(book.asks.begin(), book.asks.find(price)));
    }

    // What MDP 3.0 calls an update: a level or an order new, changed or deleted.
    enum class Action { New, Change, Delete };

    // What the book calls New, Change and Delete, as md_update_action codes each.
    static constexpr mdp3::md_update_action::enum_type md_update_action_of(Action action) {
        if (action == Action::New) return mdp3::md_update_action::enum_type::new_value;
        if (action == Action::Change) return mdp3::md_update_action::enum_type::change;
        return mdp3::md_update_action::enum_type::delete_value;
    }

    // An order's side, as md_entry_type_book codes it.
    static constexpr mdp3::md_entry_type_book::enum_type md_entry_type_book_of(bool buy) {
        if (buy) return mdp3::md_entry_type_book::enum_type::bid;
        return mdp3::md_entry_type_book::enum_type::offer;
    }

    // What the book calls New, Change and Delete, as order_update_action codes each.
    static constexpr mdp3::order_update_action::enum_type order_update_action_of(Action action) {
        if (action == Action::New) return mdp3::order_update_action::enum_type::new_value;
        if (action == Action::Change) return mdp3::order_update_action::enum_type::update;
        return mdp3::order_update_action::enum_type::delete_value;
    }

    // An order's side, as aggressor_side codes it.
    static constexpr mdp3::aggressor_side::enum_type aggressor_side_of(bool buy) {
        if (buy) return mdp3::aggressor_side::enum_type::buy;
        return mdp3::aggressor_side::enum_type::sell;
    }

    // Quantity off an order where it rests: its level is changed, or deleted when it is empty, and
    // the order is changed, or deleted when nothing of it is left.
    void lower(Book& book, const Resting& resting, std::uint64_t qty, std::uint64_t order, std::uint64_t now, bool end) {
        const auto take = [&](auto& levels) {
            const auto at = levels.find(resting.price);
            if (at == levels.end()) return;
            const bool gone = qty >= resting.qty;
            at->second.qty -= std::min(qty, at->second.qty);
            if (gone && at->second.orders > 0) at->second.orders -= 1;
            const Level left = at->second;
            const bool empty = left.orders == 0;
            // Its place is where it stood before it went.
            update(book, resting.security, resting.buy, resting.price, left, empty ? Action::Delete : Action::Change,
                   order, gone ? 0 : resting.qty - qty, gone ? Action::Delete : Action::Change, now, end);
            if (empty) levels.erase(at);
        };
        if (resting.buy) take(book.bids); else take(book.asks);
    }

    // One update, within the published depth as the level's update and the order's together
    // (linked by its reference), beyond it as the order's alone.
    void update(Book& book, std::int32_t security, bool buy, std::int64_t price, const Level& level, Action action,
                std::uint64_t order, std::uint64_t shown, Action order_action, std::uint64_t now, bool end) {
        const std::size_t at = place(book, buy, price);
        if (at <= depth) {
            mdp3::md_incremental_refresh_book out{};
            out.fields.transact_time.set(now);
            out.fields.match_event_indicator.set_last_quote_msg(true);
            out.fields.match_event_indicator.set_end_of_event(end);
            const auto frame = mdp3::start_group_write(out).start_incremental_refresh_book().md_entry_px_optional_ex(static_cast<typename mdp3::md_entry_px_optional_ex::result_type>(price)).md_entry_size_short_optional(static_cast<typename mdp3::md_entry_size_short_optional::result_type>(level.qty)).security_id(static_cast<std::int32_t>(security)).rpt_seq(static_cast<std::uint32_t>(++book.rpt_seq)).number_of_orders_optional(static_cast<typename mdp3::number_of_orders_optional::result_type>(level.orders)).md_price_level(static_cast<std::uint8_t>(at)).md_update_action(md_update_action_of(action)).md_entry_type_book(md_entry_type_book_of(buy)).padding_1({}).end_incremental_refresh_book().start_incremental_refresh_book_order_id().order_id(static_cast<std::uint64_t>(order)).md_order_priority_optional(static_cast<typename mdp3::md_order_priority_optional::result_type>(order)).md_display_qty_optional(static_cast<typename mdp3::md_display_qty_optional::result_type>(shown)).reference_id(static_cast<typename mdp3::reference_id::result_type>(1)).order_update_action(order_update_action_of(order_action)).padding_2({}).end_incremental_refresh_book_order_id();
            publisher_.publish_message(reinterpret_cast<const char*>(frame.data()), frame.size());
            return;
        }
        mdp3::md_incremental_refresh_order_book out{};
        out.fields.transact_time.set(now);
        out.fields.match_event_indicator.set_last_quote_msg(true);
        out.fields.match_event_indicator.set_end_of_event(end);
        const auto frame = mdp3::start_group_write(out).start_incremental_refresh_order_book().order_id_optional(static_cast<typename mdp3::order_id_optional::result_type>(order)).md_order_priority_optional(static_cast<typename mdp3::md_order_priority_optional::result_type>(order)).md_entry_px_optional_ex(static_cast<typename mdp3::md_entry_px_optional_ex::result_type>(price)).md_display_qty_optional(static_cast<typename mdp3::md_display_qty_optional::result_type>(shown)).security_id(static_cast<std::int32_t>(security)).md_update_action(md_update_action_of(order_action)).md_entry_type_book(md_entry_type_book_of(buy)).padding_6({}).end_incremental_refresh_order_book();
        publisher_.publish_message(reinterpret_cast<const char*>(frame.data()), frame.size());
        (void)level;
    }

    Publisher& publisher_;
    std::unordered_map<std::int32_t, Book> books_;
    std::unordered_map<std::uint64_t, Resting> orders_;
};
}
