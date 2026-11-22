#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/MessageType.hpp"
#include "../structs/ReportHeaderBody.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Market Statistics Report Message
struct market_statistics_report_report_header_body {

    struct fields_type {
        nnf_orderentry::message_type message_type;
        nnf_orderentry::report_header_body report_header_body;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::market_statistics_report_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static market_statistics_report_report_header_body* parse(std::byte* buffer) {
        return reinterpret_cast<market_statistics_report_report_header_body*>(buffer);
    }

    // parse method const
    static const market_statistics_report_report_header_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const market_statistics_report_report_header_body*>(buffer);
    }

};

// layout verification
static_assert(offsetof(market_statistics_report_report_header_body::fields_type, message_type) == 0, "unexpected offset of market_statistics_report_report_header_body::fields_type::message_type");
static_assert(offsetof(market_statistics_report_report_header_body::fields_type, report_header_body) == 1, "unexpected offset of market_statistics_report_report_header_body::fields_type::report_header_body");
static_assert(sizeof(market_statistics_report_report_header_body::fields_type) == 68, "unexpected sizeof market_statistics_report_report_header_body::fields_type");
static_assert(sizeof(market_statistics_report_report_header_body) == sizeof(nnf_orderentry::message_header) + 68, "unexpected sizeof market_statistics_report_report_header_body");

#pragma pack(pop)
}
