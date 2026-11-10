#pragma once

#include "../NewOrderCross.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class new_order_cross_group_writer;
class new_order_cross_sides_groups_builder;

class new_order_cross_sides_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    new_order_cross_sides_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(new_order_cross_sides_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(new_order_cross_sides_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) new_order_cross_sides_groups_entry{};
    }

    new_order_cross_sides_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    new_order_cross_sides_groups_builder& clordid(std::string_view v) {
        if (pos_ + sizeof(new_order_cross_sides_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<new_order_cross_sides_groups_entry*>(pos_);
        entry->clordid.set(v);
        return *this;
    }

    new_order_cross_sides_groups_builder& party_details_list_req_id(std::uint64_t v) {
        if (pos_ + sizeof(new_order_cross_sides_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<new_order_cross_sides_groups_entry*>(pos_);
        entry->party_details_list_req_id.set(v);
        return *this;
    }

    new_order_cross_sides_groups_builder& order_qty(std::uint32_t v) {
        if (pos_ + sizeof(new_order_cross_sides_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<new_order_cross_sides_groups_entry*>(pos_);
        entry->order_qty.set(v);
        return *this;
    }

    new_order_cross_sides_groups_builder& side(sbe_ilink3::side::enum_type v) {
        if (pos_ + sizeof(new_order_cross_sides_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<new_order_cross_sides_groups_entry*>(pos_);
        entry->side.set(v);
        return *this;
    }

    new_order_cross_sides_groups_builder& side_time_in_force(sbe_ilink3::side_time_in_force::enum_type v) {
        if (pos_ + sizeof(new_order_cross_sides_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<new_order_cross_sides_groups_entry*>(pos_);
        entry->side_time_in_force.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(new_order_cross_sides_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) new_order_cross_sides_groups_entry{};
        return *this;
    }

    std::span<std::byte> end_sides() {
        header_->num_in_group.set(count_);
        reinterpret_cast<sbe_ilink3::simple_open_framing_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(pos_ - msg_start_));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class new_order_cross_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    new_order_cross_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    new_order_cross_sides_groups_builder start_sides() {
        return { msg_start_, pos_, end_ };
    }
};


inline new_order_cross_group_writer start_group_write(new_order_cross& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + new_order_cross::tail_capacity };
}
}
