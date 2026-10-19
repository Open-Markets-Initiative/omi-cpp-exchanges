#pragma once

#include <cstddef>
#include <cstdint>
#include <charconv>
#include <string_view>
#include "FixWire.hpp"

namespace iexiexequities::order_entry {

// Encodes strategy actions into FIX order messages and forwards the body to `Send`.
// Assumes standard FIX order-entry fields (cl_ord_id / side / price / order_qty and,
// for cancels, orig_cl_ord_id) — present in every order-entry model we target.
template <typename Send>
class Orders {
public:
    explicit Orders(Send send) : send_(send) {}

    std::uint64_t buy (std::int64_t price, std::uint64_t qty) { return submit('1', price, qty); }
    std::uint64_t sell(std::int64_t price, std::uint64_t qty) { return submit('2', price, qty); }

    void cancel(std::uint64_t cl_ord_id) {
        OrderCancelRequest c{};
        set_id(c.orig_cl_ord_id, cl_ord_id);
        set_id(c.cl_ord_id, next_id_++);
        char body[512];
        std::size_t n = encode(c, body);
        send_('F', std::string_view(body, n));
    }

private:
    std::uint64_t submit(char side, std::int64_t price, std::uint64_t qty) {
        std::uint64_t id = next_id_++;
        NewOrderSingle o{};
        set_id(o.cl_ord_id, id);
        o.side      = side;
        o.price     = static_cast<double>(price);
        o.order_qty = static_cast<double>(qty);
        char body[512];
        std::size_t n = encode(o, body);
        send_('D', std::string_view(body, n));
        return id;
    }

    // Write a decimal id into a fixed char buffer, NUL-terminated.
    template <std::size_t N>
    static void set_id(char (&dst)[N], std::uint64_t id) {
        auto r = std::to_chars(dst, dst + N - 1, id);
        *r.ptr = '\0';
    }

    Send send_;
    std::uint64_t next_id_ = 1;
};}
