#pragma once

#include <cstdint>
#include <vector>

namespace iex::order_entry {

// The order interface a strategy drives. Any `Orders` provides:
//   std::uint64_t buy (std::int64_t price, std::uint64_t qty);   // -> cl_ord_id
//   std::uint64_t sell(std::int64_t price, std::uint64_t qty);   // -> cl_ord_id
//   void          cancel(std::uint64_t cl_ord_id);
// The sim binds this to a bot's session (encode NewOrderSingle -> send).

// Simple maker: two-sided quote around a reference mid, N levels, refreshed each step.
template <typename Orders>
class SimpleMaker {
public:
    SimpleMaker(std::int64_t mid, std::int64_t spread, std::uint64_t size, int levels = 1)
        : mid_(mid), spread_(spread), size_(size), levels_(levels) {}

    void set_mid(std::int64_t mid) { mid_ = mid; }

    void step(Orders& orders) {
        for (std::uint64_t id : quotes_) orders.cancel(id);
        quotes_.clear();
        std::int64_t half = spread_ / 2;
        for (int l = 0; l < levels_; ++l) {
            quotes_.push_back(orders.buy (mid_ - half - l, size_));
            quotes_.push_back(orders.sell(mid_ + half + l, size_));
        }
    }

private:
    std::int64_t  mid_;
    std::int64_t  spread_;
    std::uint64_t size_;
    int           levels_;
    std::vector<std::uint64_t> quotes_;
};

// SimpleTaker: crosses the spread every `period` steps, alternating side, to trade.
template <typename Orders>
class SimpleTaker {
public:
    SimpleTaker(std::int64_t mid, std::int64_t cross, std::uint64_t size, int period = 5)
        : mid_(mid), cross_(cross), size_(size), period_(period) {}

    void set_mid(std::int64_t mid) { mid_ = mid; }

    void step(Orders& orders) {
        if (++n_ % period_ != 0) return;
        if (buy_next_) orders.buy (mid_ + cross_, size_);   // marketable buy
        else           orders.sell(mid_ - cross_, size_);   // marketable sell
        buy_next_ = !buy_next_;
    }

private:
    std::int64_t  mid_;
    std::int64_t  cross_;
    std::uint64_t size_;
    int           period_;
    int           n_        = 0;
    bool          buy_next_ = true;
};}
