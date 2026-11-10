#pragma once

#include "../OrderMassActionReport.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class order_mass_action_report_group_writer;
class order_mass_action_report_affected_orders_groups_builder;

class order_mass_action_report_affected_orders_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    order_mass_action_report_affected_orders_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(order_mass_action_report_affected_orders_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(order_mass_action_report_affected_orders_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) order_mass_action_report_affected_orders_groups_entry{};
    }

    order_mass_action_report_affected_orders_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    order_mass_action_report_affected_orders_groups_builder& origclordid(std::string_view v) {
        if (pos_ + sizeof(order_mass_action_report_affected_orders_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<order_mass_action_report_affected_orders_groups_entry*>(pos_);
        entry->origclordid.set(v);
        return *this;
    }

    order_mass_action_report_affected_orders_groups_builder& affected_order_id(std::uint64_t v) {
        if (pos_ + sizeof(order_mass_action_report_affected_orders_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<order_mass_action_report_affected_orders_groups_entry*>(pos_);
        entry->affected_order_id.set(v);
        return *this;
    }

    order_mass_action_report_affected_orders_groups_builder& cxl_quantity(std::uint32_t v) {
        if (pos_ + sizeof(order_mass_action_report_affected_orders_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<order_mass_action_report_affected_orders_groups_entry*>(pos_);
        entry->cxl_quantity.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(order_mass_action_report_affected_orders_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) order_mass_action_report_affected_orders_groups_entry{};
        return *this;
    }

    std::span<std::byte> end_affected_orders() {
        header_->num_in_group.set(count_);
        reinterpret_cast<sbe_ilink3::simple_open_framing_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(pos_ - msg_start_));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class order_mass_action_report_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    order_mass_action_report_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    order_mass_action_report_affected_orders_groups_builder start_affected_orders() {
        return { msg_start_, pos_, end_ };
    }
};


inline order_mass_action_report_group_writer start_group_write(order_mass_action_report& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + order_mass_action_report::tail_capacity };
}
}
