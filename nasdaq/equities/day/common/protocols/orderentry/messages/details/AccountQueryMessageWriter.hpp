#pragma once

#include "../AccountQueryMessage.hpp"
#include <span>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;

class account_query_message_group_writer;
class account_query_message_account_query_appendage_writer;

class account_query_message_account_query_appendage_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    uint16_t count_ = 0;

public:
    account_query_message_account_query_appendage_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    account_query_message_account_query_appendage_writer& add_entry(const ouch_orders::account_query_message_account_query_appendage_entry& entry) {
        if (pos_ + sizeof(entry) > end_)
            throw std::runtime_error("buffer overrun writing external-count group entry");
        std::memcpy(pos_, &entry, sizeof(entry));
        pos_ += sizeof(entry);
        ++count_;
        return *this;
    }

    std::span<std::byte> finish() {
        reinterpret_cast<ouch_orders::account_query_message*>(msg_start_)->fields.appendage_length.set(count_);
        reinterpret_cast<ouch_orders::client_packet_header*>(msg_start_)->packet_length.set(static_cast<uint16_t>(pos_ - msg_start_ - 2));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class account_query_message_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    account_query_message_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    account_query_message_account_query_appendage_writer start_account_query_appendage() {
        return { msg_start_, pos_, end_ };
    }
};


inline account_query_message_group_writer start_group_write(account_query_message& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + account_query_message::tail_capacity };
}
}
