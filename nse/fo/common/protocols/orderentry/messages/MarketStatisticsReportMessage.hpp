#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/MessageType.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


#pragma pack(push, 1)

// Market Statistics Report Message
struct market_statistics_report_message {

    struct fields_type {
        nnf_orderentry::message_type message_type;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::market_statistics_report_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(nnf_orderentry::message_header) - sizeof(fields_type);

    fields_type fields;
    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.message_length.get().value();
        return sz == sizeof(nnf_orderentry::message_header) + sizeof(fields_type)
            ? tail + tail_capacity
            : tail + (sz - sizeof(nnf_orderentry::message_header) - sizeof(fields_type));
    }

    // sequential access to variable-length regions
    sbe_var_data report_body() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static market_statistics_report_message* parse(std::byte* buffer) {
        return reinterpret_cast<market_statistics_report_message*>(buffer);
    }

    // parse method const
    static const market_statistics_report_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const market_statistics_report_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(market_statistics_report_message::fields_type, message_type) == 0, "unexpected offset of market_statistics_report_message::fields_type::message_type");
static_assert(sizeof(market_statistics_report_message::fields_type) == 1, "unexpected sizeof market_statistics_report_message::fields_type");

#pragma pack(pop)
}

#include "details/MarketStatisticsReportMessageWriter.hpp"
#include "details/MarketStatisticsReportMessageReader.hpp"
