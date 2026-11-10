#pragma once

#include "PartyDetailsListRequestWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class party_details_list_request_requesting_party_ids_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    party_details_list_request_requesting_party_ids_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const party_details_list_request_requesting_party_ids_groups_entry& entry() const {
        return *reinterpret_cast<const party_details_list_request_requesting_party_ids_groups_entry*>(pos_);
    }

    std::string_view requesting_party_id() const {
        auto* entry = reinterpret_cast<const party_details_list_request_requesting_party_ids_groups_entry*>(pos_);
        return entry->requesting_party_id.get().value();
    }

    char requesting_party_id_source() const {
        auto* entry = reinterpret_cast<const party_details_list_request_requesting_party_ids_groups_entry*>(pos_);
        return entry->requesting_party_id_source.get().value();
    }

    char requesting_party_role() const {
        auto* entry = reinterpret_cast<const party_details_list_request_requesting_party_ids_groups_entry*>(pos_);
        return entry->requesting_party_role.get().value();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class party_details_list_request_requesting_party_ids_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    party_details_list_request_requesting_party_ids_groups_reader(const std::byte* data, const std::byte* end)
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

    party_details_list_request_requesting_party_ids_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        party_details_list_request_requesting_party_ids_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};

class party_details_list_request_party_ids_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    party_details_list_request_party_ids_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const party_details_list_request_party_ids_groups_entry& entry() const {
        return *reinterpret_cast<const party_details_list_request_party_ids_groups_entry*>(pos_);
    }

    std::uint64_t party_id() const {
        auto* entry = reinterpret_cast<const party_details_list_request_party_ids_groups_entry*>(pos_);
        return entry->party_id.get().value();
    }

    char party_id_source() const {
        auto* entry = reinterpret_cast<const party_details_list_request_party_ids_groups_entry*>(pos_);
        return entry->party_id_source.get().value();
    }

    std::uint16_t party_role() const {
        auto* entry = reinterpret_cast<const party_details_list_request_party_ids_groups_entry*>(pos_);
        return entry->party_role.get().value();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class party_details_list_request_party_ids_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    party_details_list_request_party_ids_groups_reader(const std::byte* data, const std::byte* end)
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

    party_details_list_request_party_ids_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        party_details_list_request_party_ids_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};


inline party_details_list_request_requesting_party_ids_groups_reader read_requesting_party_ids(const party_details_list_request& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

inline party_details_list_request_party_ids_groups_reader read_party_ids(const std::byte* pos, const party_details_list_request& msg) {
    return { pos, msg.tail_end() };
}

}
