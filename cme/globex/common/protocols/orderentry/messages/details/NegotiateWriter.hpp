#pragma once

#include "../Negotiate.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class negotiate_group_writer;

class negotiate_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    negotiate_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    std::span<std::byte> credentials_and_finish(std::string_view v) {
        if (pos_ + sizeof(std::uint16_t) + v.size() > end_) throw std::runtime_error("buffer overrun writing var-data");
        *reinterpret_cast<std::uint16_t*>(pos_) = static_cast<std::uint16_t>(v.size());
        std::memcpy(pos_ + sizeof(std::uint16_t), v.data(), v.size());
        auto* next_pos = pos_ + sizeof(std::uint16_t) + v.size();
        reinterpret_cast<sbe_ilink3::simple_open_framing_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(next_pos - msg_start_));
        return { msg_start_, static_cast<size_t>(next_pos - msg_start_) };
    }
};


inline negotiate_group_writer start_group_write(negotiate& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + negotiate::tail_capacity };
}
}
