#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/MessageType.hpp"
#include "../structs/SpreadReportStatisticsBody.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Spread Bhavcopy Message
struct spread_bhavcopy_spread_report_statistics_body {

    struct fields_type {
        nnf_orderentry::message_type message_type;
        nnf_orderentry::spread_report_statistics_body spread_report_statistics_body;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::spread_bhavcopy_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static spread_bhavcopy_spread_report_statistics_body* parse(std::byte* buffer) {
        return reinterpret_cast<spread_bhavcopy_spread_report_statistics_body*>(buffer);
    }

    // parse method const
    static const spread_bhavcopy_spread_report_statistics_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_bhavcopy_spread_report_statistics_body*>(buffer);
    }

};

// layout verification
static_assert(offsetof(spread_bhavcopy_spread_report_statistics_body::fields_type, message_type) == 0, "unexpected offset of spread_bhavcopy_spread_report_statistics_body::fields_type::message_type");
static_assert(offsetof(spread_bhavcopy_spread_report_statistics_body::fields_type, spread_report_statistics_body) == 1, "unexpected offset of spread_bhavcopy_spread_report_statistics_body::fields_type::spread_report_statistics_body");
static_assert(sizeof(spread_bhavcopy_spread_report_statistics_body::fields_type) == 238, "unexpected sizeof spread_bhavcopy_spread_report_statistics_body::fields_type");
static_assert(sizeof(spread_bhavcopy_spread_report_statistics_body) == sizeof(nnf_orderentry::message_header) + 238, "unexpected sizeof spread_bhavcopy_spread_report_statistics_body");

#pragma pack(pop)
}
