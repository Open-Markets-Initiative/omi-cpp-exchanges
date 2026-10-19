#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/MessageType.hpp"
#include "../types/Reserved1.hpp"
#include "../types/NumberOfRecords.hpp"
#include "../structs/EnhncdMktStatsData.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Enhanced Market Statistics Report Message
struct enhanced_market_statistics_report_message {

    struct fields_type {
        nnf_orderentry::message_type message_type;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::number_of_records number_of_records;
        nnf_orderentry::enhncd_mkt_stats_data enhncd_mkt_stats_data[4];
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::enhanced_market_statistics_report_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static enhanced_market_statistics_report_message* parse(std::byte* buffer) {
        return reinterpret_cast<enhanced_market_statistics_report_message*>(buffer);
    }

    // parse method const
    static const enhanced_market_statistics_report_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const enhanced_market_statistics_report_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(enhanced_market_statistics_report_message::fields_type, message_type) == 0, "unexpected offset of enhanced_market_statistics_report_message::fields_type::message_type");
static_assert(offsetof(enhanced_market_statistics_report_message::fields_type, reserved_1) == 1, "unexpected offset of enhanced_market_statistics_report_message::fields_type::reserved_1");
static_assert(offsetof(enhanced_market_statistics_report_message::fields_type, number_of_records) == 2, "unexpected offset of enhanced_market_statistics_report_message::fields_type::number_of_records");
static_assert(offsetof(enhanced_market_statistics_report_message::fields_type, enhncd_mkt_stats_data) == 4, "unexpected offset of enhanced_market_statistics_report_message::fields_type::enhncd_mkt_stats_data");
static_assert(sizeof(enhanced_market_statistics_report_message::fields_type) == 332, "unexpected sizeof enhanced_market_statistics_report_message::fields_type");
static_assert(sizeof(enhanced_market_statistics_report_message) == sizeof(nnf_orderentry::message_header) + 332, "unexpected sizeof enhanced_market_statistics_report_message");

#pragma pack(pop)
}
