#pragma once

#include "../DisableOrderEntryRequestMessage.hpp"
#include <span>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;

class disable_order_entry_request_message_group_writer;

class disable_order_entry_request_message_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    disable_order_entry_request_message_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    std::span<std::byte> disable_order_entry_request_appendage_and_finish(std::string_view v) {
        if (pos_ + v.size() > end_) throw std::runtime_error("buffer overrun writing external-length var-data");
        reinterpret_cast<ouch_orders::disable_order_entry_request_message*>(msg_start_)->fields.appendage_length.set(static_cast<uint16_t>(v.size()));
        std::memcpy(pos_, v.data(), v.size());
        auto* next_pos = pos_ + v.size();
        reinterpret_cast<ouch_orders::client_packet_header*>(msg_start_)->packet_length.set(static_cast<uint16_t>(next_pos - msg_start_ - 2));
        return { msg_start_, static_cast<size_t>(next_pos - msg_start_) };
    }
};


inline disable_order_entry_request_message_group_writer start_group_write(disable_order_entry_request_message& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + disable_order_entry_request_message::tail_capacity };
}
}
