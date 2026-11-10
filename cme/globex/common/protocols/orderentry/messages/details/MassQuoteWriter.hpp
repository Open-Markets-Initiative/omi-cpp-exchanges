#pragma once

#include "../MassQuote.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class mass_quote_group_writer;
class mass_quote_quote_entries_groups_builder;

class mass_quote_quote_entries_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    mass_quote_quote_entries_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(mass_quote_quote_entries_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) mass_quote_quote_entries_groups_entry{};
    }

    mass_quote_quote_entries_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    mass_quote_quote_entries_groups_builder& bid_px(typename sbe_ilink3::bid_px::result_type v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->bid_px.set(v);
        return *this;
    }

    mass_quote_quote_entries_groups_builder& offer_px(typename sbe_ilink3::offer_px::result_type v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->offer_px.set(v);
        return *this;
    }

    mass_quote_quote_entries_groups_builder& quote_entry_id(std::uint32_t v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->quote_entry_id.set(v);
        return *this;
    }

    mass_quote_quote_entries_groups_builder& security_id(std::int32_t v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->security_id.set(v);
        return *this;
    }

    mass_quote_quote_entries_groups_builder& bid_size(typename sbe_ilink3::bid_size::result_type v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->bid_size.set(v);
        return *this;
    }

    mass_quote_quote_entries_groups_builder& offer_size(typename sbe_ilink3::offer_size::result_type v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->offer_size.set(v);
        return *this;
    }

    mass_quote_quote_entries_groups_builder& underlying_security_id(typename sbe_ilink3::underlying_security_id::result_type v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->underlying_security_id.set(v);
        return *this;
    }

    mass_quote_quote_entries_groups_builder& quote_set_id(std::uint16_t v) {
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<mass_quote_quote_entries_groups_entry*>(pos_);
        entry->quote_set_id.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(mass_quote_quote_entries_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) mass_quote_quote_entries_groups_entry{};
        return *this;
    }

    std::span<std::byte> end_quote_entries() {
        header_->num_in_group.set(count_);
        reinterpret_cast<sbe_ilink3::simple_open_framing_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(pos_ - msg_start_));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class mass_quote_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    mass_quote_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    mass_quote_quote_entries_groups_builder start_quote_entries() {
        return { msg_start_, pos_, end_ };
    }
};


inline mass_quote_group_writer start_group_write(mass_quote& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + mass_quote::tail_capacity };
}
}
