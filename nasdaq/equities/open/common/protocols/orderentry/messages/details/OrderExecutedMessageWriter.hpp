#pragma once

#include "../OrderExecutedMessage.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;

class order_executed_message_group_writer;
class order_executed_message_order_executed_appendage_writer;

class order_executed_message_order_executed_appendage_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    uint16_t count_ = 0;

public:
    order_executed_message_order_executed_appendage_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    order_executed_message_order_executed_appendage_writer& add_entry(const ouch_orders::order_executed_message_order_executed_appendage_entry& entry) {
        if (pos_ + sizeof(entry) > end_)
            throw std::runtime_error("buffer overrun writing external-count group entry");
        std::memcpy(pos_, &entry, sizeof(entry));
        pos_ += sizeof(entry);
        ++count_;
        return *this;
    }

    std::span<std::byte> finish() {
        reinterpret_cast<ouch_orders::order_executed_message*>(msg_start_)->fields.appendage_length.set(count_);
        reinterpret_cast<ouch_orders::server_packet_header*>(msg_start_)->packet_length.set(static_cast<uint16_t>(pos_ - msg_start_ - 2));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class order_executed_message_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    order_executed_message_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    order_executed_message_order_executed_appendage_writer start_order_executed_appendage() {
        return { msg_start_, pos_, end_ };
    }
};


inline order_executed_message_group_writer start_group_write(order_executed_message& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + order_executed_message::tail_capacity };
}
}
