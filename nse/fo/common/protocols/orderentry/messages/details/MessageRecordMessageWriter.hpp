#pragma once

#include "../MessageRecordMessage.hpp"
#include <span>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

class message_record_message_group_writer;

class message_record_message_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    message_record_message_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    std::span<std::byte> message_download_data_and_finish(std::string_view v) {
        if (pos_ + v.size() > end_) throw std::runtime_error("buffer overrun writing external-length var-data");
        std::memcpy(pos_, v.data(), v.size());
        auto* next_pos = pos_ + v.size();
        reinterpret_cast<nnf_orderentry::message_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(next_pos - msg_start_));
        return { msg_start_, static_cast<size_t>(next_pos - msg_start_) };
    }
};


inline message_record_message_group_writer start_group_write(message_record_message& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + message_record_message::tail_capacity };
}
}
