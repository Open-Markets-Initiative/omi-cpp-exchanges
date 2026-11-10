#pragma once

#include "SecurityDefinitionRequestWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class security_definition_request_request_legs_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    security_definition_request_request_legs_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const security_definition_request_request_legs_groups_entry& entry() const {
        return *reinterpret_cast<const security_definition_request_request_legs_groups_entry*>(pos_);
    }

    std::optional<std::int64_t> leg_price() const {
        auto* entry = reinterpret_cast<const security_definition_request_request_legs_groups_entry*>(pos_);
        return entry->leg_price.get();
    }

    std::int32_t leg_security_id() const {
        auto* entry = reinterpret_cast<const security_definition_request_request_legs_groups_entry*>(pos_);
        return entry->leg_security_id.get().value();
    }

    sbe_ilink3::leg_side::enum_type leg_side() const {
        auto* entry = reinterpret_cast<const security_definition_request_request_legs_groups_entry*>(pos_);
        return entry->leg_side.get().value();
    }

    std::optional<std::uint8_t> leg_ratio_qty() const {
        auto* entry = reinterpret_cast<const security_definition_request_request_legs_groups_entry*>(pos_);
        return entry->leg_ratio_qty.get();
    }

    const sbe_ilink3::leg_option_delta& leg_option_delta() const {
        auto* entry = reinterpret_cast<const security_definition_request_request_legs_groups_entry*>(pos_);
        return entry->leg_option_delta;
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class security_definition_request_request_legs_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    security_definition_request_request_legs_groups_reader(const std::byte* data, const std::byte* end)
        : end_(end) {
        if (data + sizeof(sbe_ilink3::group_size) > end_)
            throw std::runtime_error("buffer overrun reading group header");
        auto* hdr = reinterpret_cast<const sbe_ilink3::group_size*>(data);
        block_length_ = hdr->block_length.get().value();
        count_ = hdr->num_in_group.get().value();
        pos_ = data + sizeof(sbe_ilink3::group_size);
        index_ = 0;
    }

    uint16_t count() const { return count_; }

    bool has_next() const { return index_ < count_; }

    security_definition_request_request_legs_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        security_definition_request_request_legs_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};

class security_definition_request_broken_dates_request_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    security_definition_request_broken_dates_request_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const security_definition_request_broken_dates_request_groups_entry& entry() const {
        return *reinterpret_cast<const security_definition_request_broken_dates_request_groups_entry*>(pos_);
    }

    std::optional<std::uint16_t> broken_date_start() const {
        auto* entry = reinterpret_cast<const security_definition_request_broken_dates_request_groups_entry*>(pos_);
        return entry->broken_date_start.get();
    }

    std::optional<std::uint16_t> broken_date_end() const {
        auto* entry = reinterpret_cast<const security_definition_request_broken_dates_request_groups_entry*>(pos_);
        return entry->broken_date_end.get();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class security_definition_request_broken_dates_request_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    security_definition_request_broken_dates_request_groups_reader(const std::byte* data, const std::byte* end)
        : end_(end) {
        if (data + sizeof(sbe_ilink3::group_size) > end_)
            throw std::runtime_error("buffer overrun reading group header");
        auto* hdr = reinterpret_cast<const sbe_ilink3::group_size*>(data);
        block_length_ = hdr->block_length.get().value();
        count_ = hdr->num_in_group.get().value();
        pos_ = data + sizeof(sbe_ilink3::group_size);
        index_ = 0;
    }

    uint16_t count() const { return count_; }

    bool has_next() const { return index_ < count_; }

    security_definition_request_broken_dates_request_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        security_definition_request_broken_dates_request_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};


inline security_definition_request_request_legs_groups_reader read_request_legs(const security_definition_request& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

inline security_definition_request_broken_dates_request_groups_reader read_broken_dates_request(const std::byte* pos, const security_definition_request& msg) {
    return { pos, msg.tail_end() };
}

}
