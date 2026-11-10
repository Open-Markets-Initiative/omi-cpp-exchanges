#pragma once

#include "../SecurityDefinitionRequest.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class security_definition_request_group_writer;
class security_definition_request_request_legs_groups_builder;
class security_definition_request_after_request_legs;
class security_definition_request_broken_dates_request_groups_builder;

class security_definition_request_request_legs_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    security_definition_request_request_legs_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(security_definition_request_request_legs_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(security_definition_request_request_legs_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) security_definition_request_request_legs_groups_entry{};
    }

    security_definition_request_request_legs_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    security_definition_request_request_legs_groups_builder& leg_price(typename sbe_ilink3::leg_price::result_type v) {
        if (pos_ + sizeof(security_definition_request_request_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<security_definition_request_request_legs_groups_entry*>(pos_);
        entry->leg_price.set(v);
        return *this;
    }

    security_definition_request_request_legs_groups_builder& leg_security_id(std::int32_t v) {
        if (pos_ + sizeof(security_definition_request_request_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<security_definition_request_request_legs_groups_entry*>(pos_);
        entry->leg_security_id.set(v);
        return *this;
    }

    security_definition_request_request_legs_groups_builder& leg_option_delta(const sbe_ilink3::leg_option_delta& v) {
        if (pos_ + sizeof(security_definition_request_request_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<security_definition_request_request_legs_groups_entry*>(pos_);
        entry->leg_option_delta = v;
        return *this;
    }

    security_definition_request_request_legs_groups_builder& leg_side(sbe_ilink3::leg_side::enum_type v) {
        if (pos_ + sizeof(security_definition_request_request_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<security_definition_request_request_legs_groups_entry*>(pos_);
        entry->leg_side.set(v);
        return *this;
    }

    security_definition_request_request_legs_groups_builder& leg_ratio_qty(typename sbe_ilink3::leg_ratio_qty::result_type v) {
        if (pos_ + sizeof(security_definition_request_request_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<security_definition_request_request_legs_groups_entry*>(pos_);
        entry->leg_ratio_qty.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(security_definition_request_request_legs_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) security_definition_request_request_legs_groups_entry{};
        return *this;
    }

    security_definition_request_after_request_legs end_request_legs();
};

class security_definition_request_broken_dates_request_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    security_definition_request_broken_dates_request_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(security_definition_request_broken_dates_request_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(security_definition_request_broken_dates_request_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) security_definition_request_broken_dates_request_groups_entry{};
    }

    security_definition_request_broken_dates_request_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    security_definition_request_broken_dates_request_groups_builder& broken_date_start(typename sbe_ilink3::broken_date_start::result_type v) {
        if (pos_ + sizeof(security_definition_request_broken_dates_request_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<security_definition_request_broken_dates_request_groups_entry*>(pos_);
        entry->broken_date_start.set(v);
        return *this;
    }

    security_definition_request_broken_dates_request_groups_builder& broken_date_end(typename sbe_ilink3::broken_date_end::result_type v) {
        if (pos_ + sizeof(security_definition_request_broken_dates_request_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<security_definition_request_broken_dates_request_groups_entry*>(pos_);
        entry->broken_date_end.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(security_definition_request_broken_dates_request_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) security_definition_request_broken_dates_request_groups_entry{};
        return *this;
    }

    std::span<std::byte> end_broken_dates_request() {
        header_->num_in_group.set(count_);
        reinterpret_cast<sbe_ilink3::simple_open_framing_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(pos_ - msg_start_));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class security_definition_request_after_request_legs {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    security_definition_request_after_request_legs(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    security_definition_request_broken_dates_request_groups_builder start_broken_dates_request() {
        return { msg_start_, pos_, end_ };
    }
};

class security_definition_request_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    security_definition_request_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    security_definition_request_request_legs_groups_builder start_request_legs() {
        return { msg_start_, pos_, end_ };
    }
};

inline security_definition_request_after_request_legs security_definition_request_request_legs_groups_builder::end_request_legs() {
    header_->num_in_group.set(count_);
    return { msg_start_, pos_, end_ };
}



inline security_definition_request_group_writer start_group_write(security_definition_request& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + security_definition_request::tail_capacity };
}
}
