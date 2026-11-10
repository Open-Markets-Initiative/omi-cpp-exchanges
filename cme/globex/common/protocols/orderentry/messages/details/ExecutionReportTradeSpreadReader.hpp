#pragma once

#include "ExecutionReportTradeSpreadWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

class execution_report_trade_spread_fills_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    execution_report_trade_spread_fills_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const execution_report_trade_spread_fills_groups_entry& entry() const {
        return *reinterpret_cast<const execution_report_trade_spread_fills_groups_entry*>(pos_);
    }

    std::int64_t fill_px() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_fills_groups_entry*>(pos_);
        return entry->fill_px.get().value();
    }

    std::uint32_t fill_qty() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_fills_groups_entry*>(pos_);
        return entry->fill_qty.get().value();
    }

    std::string_view fill_exec_id() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_fills_groups_entry*>(pos_);
        return entry->fill_exec_id.get().value();
    }

    std::uint8_t fill_yield_type() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_fills_groups_entry*>(pos_);
        return entry->fill_yield_type.get().value();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class execution_report_trade_spread_fills_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    execution_report_trade_spread_fills_groups_reader(const std::byte* data, const std::byte* end)
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

    execution_report_trade_spread_fills_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        execution_report_trade_spread_fills_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};

class execution_report_trade_spread_trade_legs_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    execution_report_trade_spread_trade_legs_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const execution_report_trade_spread_trade_legs_groups_entry& entry() const {
        return *reinterpret_cast<const execution_report_trade_spread_trade_legs_groups_entry*>(pos_);
    }

    std::uint64_t leg_exec_id() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_trade_legs_groups_entry*>(pos_);
        return entry->leg_exec_id.get().value();
    }

    std::int64_t leg_last_px() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_trade_legs_groups_entry*>(pos_);
        return entry->leg_last_px.get().value();
    }

    std::int32_t leg_security_id() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_trade_legs_groups_entry*>(pos_);
        return entry->leg_security_id.get().value();
    }

    std::uint32_t leg_trade_id() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_trade_legs_groups_entry*>(pos_);
        return entry->leg_trade_id.get().value();
    }

    std::uint32_t leg_last_qty() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_trade_legs_groups_entry*>(pos_);
        return entry->leg_last_qty.get().value();
    }

    sbe_ilink3::leg_side::enum_type leg_side() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_trade_legs_groups_entry*>(pos_);
        return entry->leg_side.get().value();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class execution_report_trade_spread_trade_legs_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    execution_report_trade_spread_trade_legs_groups_reader(const std::byte* data, const std::byte* end)
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

    execution_report_trade_spread_trade_legs_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        execution_report_trade_spread_trade_legs_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};

class execution_report_trade_spread_spread_order_events_groups_entry_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;

public:
    execution_report_trade_spread_spread_order_events_groups_entry_reader(const std::byte* pos, const std::byte* end, uint16_t block_length)
        : pos_(pos), end_(end), block_length_(block_length) {
    }

    const execution_report_trade_spread_spread_order_events_groups_entry& entry() const {
        return *reinterpret_cast<const execution_report_trade_spread_spread_order_events_groups_entry*>(pos_);
    }

    std::int64_t order_event_px() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_spread_order_events_groups_entry*>(pos_);
        return entry->order_event_px.get().value();
    }

    std::string_view order_event_text() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_spread_order_events_groups_entry*>(pos_);
        return entry->order_event_text.get().value();
    }

    std::uint32_t order_event_exec_id() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_spread_order_events_groups_entry*>(pos_);
        return entry->order_event_exec_id.get().value();
    }

    std::uint32_t order_event_qty() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_spread_order_events_groups_entry*>(pos_);
        return entry->order_event_qty.get().value();
    }

    sbe_ilink3::order_event_type::enum_type order_event_type() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_spread_order_events_groups_entry*>(pos_);
        return entry->order_event_type.get().value();
    }

    std::uint8_t order_event_reason() const {
        auto* entry = reinterpret_cast<const execution_report_trade_spread_spread_order_events_groups_entry*>(pos_);
        return entry->order_event_reason.get().value();
    }

    const std::byte* end_position() const { return pos_ + block_length_; }
};

class execution_report_trade_spread_spread_order_events_groups_reader {
    const std::byte* pos_;
    const std::byte* end_;
    uint16_t block_length_;
    uint16_t count_;
    uint16_t index_;

public:
    execution_report_trade_spread_spread_order_events_groups_reader(const std::byte* data, const std::byte* end)
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

    execution_report_trade_spread_spread_order_events_groups_entry_reader next() {
        if (!has_next()) throw std::runtime_error("no more entries in group");
        execution_report_trade_spread_spread_order_events_groups_entry_reader entry(pos_, end_, block_length_);
        pos_ = entry.end_position();
        ++index_;
        return entry;
    }

    void skip_remaining() {
        while (has_next()) { next(); }
    }

    const std::byte* end_position() const { return pos_; }
};


inline execution_report_trade_spread_fills_groups_reader read_fills(const execution_report_trade_spread& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

inline execution_report_trade_spread_trade_legs_groups_reader read_trade_legs(const std::byte* pos, const execution_report_trade_spread& msg) {
    return { pos, msg.tail_end() };
}

inline execution_report_trade_spread_spread_order_events_groups_reader read_spread_order_events(const std::byte* pos, const execution_report_trade_spread& msg) {
    return { pos, msg.tail_end() };
}

}
