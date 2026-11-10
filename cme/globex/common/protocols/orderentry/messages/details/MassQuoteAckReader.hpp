#pragma once

#include "MassQuoteAckWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class mass_quote_ack_quote_ack_entries_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    mass_quote_ack_quote_ack_entries_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const mass_quote_ack_quote_ack_entries_groups_entry& entry() const {
        return *reinterpret_cast<const mass_quote_ack_quote_ack_entries_groups_entry*>(pos_);
    }

    std::uint32_t quote_entry_id() const {
        auto* entry = reinterpret_cast<const mass_quote_ack_quote_ack_entries_groups_entry*>(pos_);
        return entry->quote_entry_id.get().value();
    }

    std::int32_t security_id() const {
        auto* entry = reinterpret_cast<const mass_quote_ack_quote_ack_entries_groups_entry*>(pos_);
        return entry->security_id.get().value();
    }

    std::uint16_t quote_set_id() const {
        auto* entry = reinterpret_cast<const mass_quote_ack_quote_ack_entries_groups_entry*>(pos_);
        return entry->quote_set_id.get().value();
    }

    std::uint8_t quote_entry_reject_reason() const {
        auto* entry = reinterpret_cast<const mass_quote_ack_quote_ack_entries_groups_entry*>(pos_);
        return entry->quote_entry_reject_reason.get().value();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class mass_quote_ack_quote_ack_entries_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    mass_quote_ack_quote_ack_entries_groups_reader(const std::byte* data, const std::byte* end)
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

    mass_quote_ack_quote_ack_entries_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        mass_quote_ack_quote_ack_entries_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};


inline mass_quote_ack_quote_ack_entries_groups_reader read_quote_ack_entries(const mass_quote_ack& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

}
