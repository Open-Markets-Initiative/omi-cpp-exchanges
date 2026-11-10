#pragma once

#include "../PartyDetailsListRequest.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class party_details_list_request_group_writer;
class party_details_list_request_requesting_party_ids_groups_builder;
class party_details_list_request_after_requesting_party_ids;
class party_details_list_request_party_ids_groups_builder;

class party_details_list_request_requesting_party_ids_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    party_details_list_request_requesting_party_ids_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(party_details_list_request_requesting_party_ids_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(party_details_list_request_requesting_party_ids_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) party_details_list_request_requesting_party_ids_groups_entry{};
    }

    party_details_list_request_requesting_party_ids_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    party_details_list_request_requesting_party_ids_groups_builder& requesting_party_id(std::string_view v) {
        if (pos_ + sizeof(party_details_list_request_requesting_party_ids_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<party_details_list_request_requesting_party_ids_groups_entry*>(pos_);
        entry->requesting_party_id.set(v);
        return *this;
    }

    party_details_list_request_requesting_party_ids_groups_builder& requesting_party_id_source(char v) {
        if (pos_ + sizeof(party_details_list_request_requesting_party_ids_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<party_details_list_request_requesting_party_ids_groups_entry*>(pos_);
        entry->requesting_party_id_source.set(v);
        return *this;
    }

    party_details_list_request_requesting_party_ids_groups_builder& requesting_party_role(char v) {
        if (pos_ + sizeof(party_details_list_request_requesting_party_ids_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<party_details_list_request_requesting_party_ids_groups_entry*>(pos_);
        entry->requesting_party_role.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(party_details_list_request_requesting_party_ids_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) party_details_list_request_requesting_party_ids_groups_entry{};
        return *this;
    }

    party_details_list_request_after_requesting_party_ids end_requesting_party_ids();
};

class party_details_list_request_party_ids_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    party_details_list_request_party_ids_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(party_details_list_request_party_ids_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(party_details_list_request_party_ids_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) party_details_list_request_party_ids_groups_entry{};
    }

    party_details_list_request_party_ids_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    party_details_list_request_party_ids_groups_builder& party_id(std::uint64_t v) {
        if (pos_ + sizeof(party_details_list_request_party_ids_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<party_details_list_request_party_ids_groups_entry*>(pos_);
        entry->party_id.set(v);
        return *this;
    }

    party_details_list_request_party_ids_groups_builder& party_id_source(char v) {
        if (pos_ + sizeof(party_details_list_request_party_ids_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<party_details_list_request_party_ids_groups_entry*>(pos_);
        entry->party_id_source.set(v);
        return *this;
    }

    party_details_list_request_party_ids_groups_builder& party_role(std::uint16_t v) {
        if (pos_ + sizeof(party_details_list_request_party_ids_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<party_details_list_request_party_ids_groups_entry*>(pos_);
        entry->party_role.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(party_details_list_request_party_ids_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) party_details_list_request_party_ids_groups_entry{};
        return *this;
    }

    std::span<std::byte> end_party_ids() {
        header_->num_in_group.set(count_);
        reinterpret_cast<sbe_ilink3::simple_open_framing_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(pos_ - msg_start_));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class party_details_list_request_after_requesting_party_ids {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    party_details_list_request_after_requesting_party_ids(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    party_details_list_request_party_ids_groups_builder start_party_ids() {
        return { msg_start_, pos_, end_ };
    }
};

class party_details_list_request_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    party_details_list_request_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    party_details_list_request_requesting_party_ids_groups_builder start_requesting_party_ids() {
        return { msg_start_, pos_, end_ };
    }
};

inline party_details_list_request_after_requesting_party_ids party_details_list_request_requesting_party_ids_groups_builder::end_requesting_party_ids() {
    header_->num_in_group.set(count_);
    return { msg_start_, pos_, end_ };
}



inline party_details_list_request_group_writer start_group_write(party_details_list_request& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + party_details_list_request::tail_capacity };
}
}
