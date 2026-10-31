#pragma once

#include "../ReplaceOrderMessage.hpp"
#include <span>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;

class replace_order_message_group_writer;
class replace_order_message_replace_order_appendage_writer;

class replace_order_message_replace_order_appendage_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    uint16_t count_ = 0;

public:
    replace_order_message_replace_order_appendage_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    replace_order_message_replace_order_appendage_writer& add_entry(const ouch_orders::replace_order_message_replace_order_appendage_entry& entry) {
        if (pos_ + sizeof(entry) > end_)
            throw std::runtime_error("buffer overrun writing external-count group entry");
        std::memcpy(pos_, &entry, sizeof(entry));
        pos_ += sizeof(entry);
        ++count_;
        return *this;
    }

    std::span<std::byte> finish() {
        reinterpret_cast<ouch_orders::replace_order_message*>(msg_start_)->fields.appendage_length.set(count_);
        reinterpret_cast<ouch_orders::client_packet_header*>(msg_start_)->packet_length.set(static_cast<uint16_t>(pos_ - msg_start_ - 2));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class replace_order_message_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    replace_order_message_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    replace_order_message_replace_order_appendage_writer start_replace_order_appendage() {
        return { msg_start_, pos_, end_ };
    }
};


inline replace_order_message_group_writer start_group_write(replace_order_message& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + replace_order_message::tail_capacity };
}
}
