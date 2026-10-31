#pragma once

#include "../DisableOrderEntryRequestMessage.hpp"
#include <span>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;

class disable_order_entry_request_message_group_writer;
class disable_order_entry_request_message_disable_order_entry_request_appendage_writer;

class disable_order_entry_request_message_disable_order_entry_request_appendage_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    uint16_t count_ = 0;

public:
    disable_order_entry_request_message_disable_order_entry_request_appendage_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    disable_order_entry_request_message_disable_order_entry_request_appendage_writer& add_entry(const ouch_orders::disable_order_entry_request_message_disable_order_entry_request_appendage_entry& entry) {
        if (pos_ + sizeof(entry) > end_)
            throw std::runtime_error("buffer overrun writing external-count group entry");
        std::memcpy(pos_, &entry, sizeof(entry));
        pos_ += sizeof(entry);
        ++count_;
        return *this;
    }

    std::span<std::byte> finish() {
        reinterpret_cast<ouch_orders::disable_order_entry_request_message*>(msg_start_)->fields.appendage_length.set(count_);
        reinterpret_cast<ouch_orders::client_packet_header*>(msg_start_)->packet_length.set(static_cast<uint16_t>(pos_ - msg_start_ - 2));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class disable_order_entry_request_message_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    disable_order_entry_request_message_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    disable_order_entry_request_message_disable_order_entry_request_appendage_writer start_disable_order_entry_request_appendage() {
        return { msg_start_, pos_, end_ };
    }
};


inline disable_order_entry_request_message_group_writer start_group_write(disable_order_entry_request_message& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + disable_order_entry_request_message::tail_capacity };
}
}
