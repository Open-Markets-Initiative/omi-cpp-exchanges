#pragma once

#include <cstdint>
#include <vector>

namespace sim::traders {

// Drives the `Orders` concept the generated adapter provides:
//   std::uint64_t buy (std::int64_t price, std::uint64_t quantity);   // -> the trader's reference
//   std::uint64_t sell(std::int64_t price, std::uint64_t quantity);   // -> the trader's reference
//   void          cancel(std::uint64_t reference);

// Simple maker: a two-sided quote around a reference mid, N levels a tick apart, refreshed each step.
template <typename Orders>
class SimpleMaker {
public:
    SimpleMaker(std::int64_t mid, std::int64_t spread, std::uint64_t size, int levels, std::int64_t tick)
        : mid_(mid), spread_(spread), size_(size), levels_(levels), tick_(tick) {}

    void set_mid(std::int64_t mid) { mid_ = mid; }

    void step(Orders& orders) {
        for (std::uint64_t reference : quotes_) orders.cancel(reference);
        quotes_.clear();
        const std::int64_t half = spread_ / 2;
        for (int level = 0; level < levels_; ++level) {
            quotes_.push_back(orders.buy (mid_ - half - level * tick_, size_));
            quotes_.push_back(orders.sell(mid_ + half + level * tick_, size_));
        }
    }

private:
    std::int64_t  mid_;
    std::int64_t  spread_;
    std::uint64_t size_;
    int           levels_;
    std::int64_t  tick_;   // how far apart its levels are, in the venue's price units
    std::vector<std::uint64_t> quotes_;
};

}