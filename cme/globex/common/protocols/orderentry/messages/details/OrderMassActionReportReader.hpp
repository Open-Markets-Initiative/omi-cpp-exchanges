#pragma once

#include "OrderMassActionReportWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class order_mass_action_report_affected_orders_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    order_mass_action_report_affected_orders_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const order_mass_action_report_affected_orders_groups_entry& entry() const {
        return *reinterpret_cast<const order_mass_action_report_affected_orders_groups_entry*>(pos_);
    }

    std::string_view origclordid() const {
        auto* entry = reinterpret_cast<const order_mass_action_report_affected_orders_groups_entry*>(pos_);
        return entry->origclordid.get().value();
    }

    std::uint64_t affected_order_id() const {
        auto* entry = reinterpret_cast<const order_mass_action_report_affected_orders_groups_entry*>(pos_);
        return entry->affected_order_id.get().value();
    }

    std::uint32_t cxl_quantity() const {
        auto* entry = reinterpret_cast<const order_mass_action_report_affected_orders_groups_entry*>(pos_);
        return entry->cxl_quantity.get().value();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class order_mass_action_report_affected_orders_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    order_mass_action_report_affected_orders_groups_reader(const std::byte* data, const std::byte* end)
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

    order_mass_action_report_affected_orders_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        order_mass_action_report_affected_orders_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};


inline order_mass_action_report_affected_orders_groups_reader read_affected_orders(const order_mass_action_report& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

}
