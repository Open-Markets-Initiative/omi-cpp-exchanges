#pragma once

#include <cstdint>

namespace sim::traders {

// Drives the `Orders` concept the generated adapter provides:
//   std::uint64_t buy (std::int64_t price, std::uint64_t quantity);   // -> the trader's reference
//   std::uint64_t sell(std::int64_t price, std::uint64_t quantity);   // -> the trader's reference
//   void          cancel(std::uint64_t reference);

// Simple taker: crosses the spread every `period` steps, alternating side, to trade.
template <typename Orders>
class SimpleTaker {
public:
    SimpleTaker(std::int64_t mid, std::int64_t cross, std::uint64_t size, int period = 5)
        : mid_(mid), cross_(cross), size_(size), period_(period) {}

    void set_mid(std::int64_t mid) { mid_ = mid; }

    void step(Orders& orders) {
        if (++steps_ % period_ != 0) return;
        if (buy_next_) orders.buy (mid_ + cross_, size_);   // marketable buy
        else           orders.sell(mid_ - cross_, size_);   // marketable sell
        buy_next_ = !buy_next_;
    }

private:
    std::int64_t  mid_;
    std::int64_t  cross_;
    std::uint64_t size_;
    int           period_;
    int           steps_    = 0;
    bool          buy_next_ = true;
};

}