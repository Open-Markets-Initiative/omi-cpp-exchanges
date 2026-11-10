#pragma once

#include "../ExecutionReportTradeAddendumSpread.hpp"
#include <span>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class execution_report_trade_addendum_spread_group_writer;
class execution_report_trade_addendum_spread_fills_groups_builder;
class execution_report_trade_addendum_spread_after_fills;
class execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder;
class execution_report_trade_addendum_spread_after_trade_addendum_legs;
class execution_report_trade_addendum_spread_spread_trade_events_groups_builder;

class execution_report_trade_addendum_spread_fills_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    execution_report_trade_addendum_spread_fills_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(execution_report_trade_addendum_spread_fills_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(execution_report_trade_addendum_spread_fills_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) execution_report_trade_addendum_spread_fills_groups_entry{};
    }

    execution_report_trade_addendum_spread_fills_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    execution_report_trade_addendum_spread_fills_groups_builder& fill_px(std::int64_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_fills_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_fills_groups_entry*>(pos_);
        entry->fill_px.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_fills_groups_builder& fill_qty(std::uint32_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_fills_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_fills_groups_entry*>(pos_);
        entry->fill_qty.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_fills_groups_builder& fill_exec_id(std::string_view v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_fills_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_fills_groups_entry*>(pos_);
        entry->fill_exec_id.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_fills_groups_builder& fill_yield_type(std::uint8_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_fills_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_fills_groups_entry*>(pos_);
        entry->fill_yield_type.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(execution_report_trade_addendum_spread_fills_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) execution_report_trade_addendum_spread_fills_groups_entry{};
        return *this;
    }

    execution_report_trade_addendum_spread_after_fills end_fills();
};

class execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry{};
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_exec_id(std::uint64_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_exec_id.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_last_px(std::int64_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_last_px.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_exec_ref_id(typename sbe_ilink3::leg_exec_ref_id::result_type v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_exec_ref_id.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_trade_id(std::uint32_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_trade_id.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_trade_ref_id(typename sbe_ilink3::leg_trade_ref_id::result_type v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_trade_ref_id.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_security_id(std::int32_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_security_id.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_last_qty(std::uint32_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_last_qty.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder& leg_side(sbe_ilink3::leg_side::enum_type v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry*>(pos_);
        entry->leg_side.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) execution_report_trade_addendum_spread_trade_addendum_legs_groups_entry{};
        return *this;
    }

    execution_report_trade_addendum_spread_after_trade_addendum_legs end_trade_addendum_legs();
};

class execution_report_trade_addendum_spread_spread_trade_events_groups_builder {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;
    sbe_ilink3::group_size* header_;
    uint16_t count_ = 0;

public:
    execution_report_trade_addendum_spread_spread_trade_events_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {
        if (pos_ + sizeof(sbe_ilink3::group_size) > end_) throw std::runtime_error("buffer overrun writing group header");
        header_ = reinterpret_cast<sbe_ilink3::group_size*>(pos_);
        header_->block_length.set(sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry));
        header_->num_in_group.set(0);
        pos_ += sizeof(sbe_ilink3::group_size);
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) execution_report_trade_addendum_spread_spread_trade_events_groups_entry{};
    }

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder(std::byte* msg_start, std::byte* pos, std::byte* end, sbe_ilink3::group_size* header, uint16_t count)
        : msg_start_(msg_start), pos_(pos), end_(end), header_(header), count_(count) {}

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder& order_event_px(std::int64_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_spread_trade_events_groups_entry*>(pos_);
        entry->order_event_px.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder& order_event_text(std::string_view v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_spread_trade_events_groups_entry*>(pos_);
        entry->order_event_text.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder& order_event_exec_id(std::uint32_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_spread_trade_events_groups_entry*>(pos_);
        entry->order_event_exec_id.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder& order_event_qty(std::uint32_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_spread_trade_events_groups_entry*>(pos_);
        entry->order_event_qty.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder& trade_addendum(sbe_ilink3::trade_addendum::enum_type v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_spread_trade_events_groups_entry*>(pos_);
        entry->trade_addendum.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder& order_event_reason(std::uint8_t v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_spread_trade_events_groups_entry*>(pos_);
        entry->order_event_reason.set(v);
        return *this;
    }

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder& original_order_event_exec_id(typename sbe_ilink3::original_order_event_exec_id::result_type v) {
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) > end_) throw std::runtime_error("buffer overrun writing group entry");
        auto* entry = reinterpret_cast<execution_report_trade_addendum_spread_spread_trade_events_groups_entry*>(pos_);
        entry->original_order_event_exec_id.set(v);
        pos_ += header_->block_length.get().value();
        count_++;
        if (pos_ + sizeof(execution_report_trade_addendum_spread_spread_trade_events_groups_entry) <= end_) ::new (static_cast<void*>(pos_)) execution_report_trade_addendum_spread_spread_trade_events_groups_entry{};
        return *this;
    }

    std::span<std::byte> end_spread_trade_events() {
        header_->num_in_group.set(count_);
        reinterpret_cast<sbe_ilink3::simple_open_framing_header*>(msg_start_)->message_length.set(static_cast<uint16_t>(pos_ - msg_start_));
        return { msg_start_, static_cast<size_t>(pos_ - msg_start_) };
    }
};

class execution_report_trade_addendum_spread_after_fills {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    execution_report_trade_addendum_spread_after_fills(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder start_trade_addendum_legs() {
        return { msg_start_, pos_, end_ };
    }
};

class execution_report_trade_addendum_spread_after_trade_addendum_legs {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    execution_report_trade_addendum_spread_after_trade_addendum_legs(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    execution_report_trade_addendum_spread_spread_trade_events_groups_builder start_spread_trade_events() {
        return { msg_start_, pos_, end_ };
    }
};

class execution_report_trade_addendum_spread_group_writer {
    std::byte* msg_start_;
    std::byte* pos_;
    std::byte* end_;

public:
    execution_report_trade_addendum_spread_group_writer(std::byte* msg_start, std::byte* pos, std::byte* end)
        : msg_start_(msg_start), pos_(pos), end_(end) {}

    execution_report_trade_addendum_spread_fills_groups_builder start_fills() {
        return { msg_start_, pos_, end_ };
    }
};

inline execution_report_trade_addendum_spread_after_fills execution_report_trade_addendum_spread_fills_groups_builder::end_fills() {
    header_->num_in_group.set(count_);
    return { msg_start_, pos_, end_ };
}

inline execution_report_trade_addendum_spread_after_trade_addendum_legs execution_report_trade_addendum_spread_trade_addendum_legs_groups_builder::end_trade_addendum_legs() {
    header_->num_in_group.set(count_);
    return { msg_start_, pos_, end_ };
}



inline execution_report_trade_addendum_spread_group_writer start_group_write(execution_report_trade_addendum_spread& msg) {
    return { reinterpret_cast<std::byte*>(&msg), msg.tail, msg.tail + execution_report_trade_addendum_spread::tail_capacity };
}
}
