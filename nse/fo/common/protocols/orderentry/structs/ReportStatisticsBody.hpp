#pragma once

#include <cstddef>
#include "../types/Reserved1.hpp"
#include "../types/NumberOfRecords.hpp"
#include "../structs/MktStatsData.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct report_statistics_body {

    nnf_orderentry::reserved_1 reserved_1;
    nnf_orderentry::number_of_records number_of_records;
    nnf_orderentry::mkt_stats_data mkt_stats_data[4];

    // parse method
    static report_statistics_body* parse(std::byte* buffer) {
        return reinterpret_cast<report_statistics_body*>(buffer);
    }

    // parse method const
    static const report_statistics_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const report_statistics_body*>(buffer);
    }
};

// layout verification
static_assert(offsetof(report_statistics_body, reserved_1) == 0, "unexpected offset of report_statistics_body::reserved_1");
static_assert(offsetof(report_statistics_body, number_of_records) == 1, "unexpected offset of report_statistics_body::number_of_records");
static_assert(offsetof(report_statistics_body, mkt_stats_data) == 3, "unexpected offset of report_statistics_body::mkt_stats_data");
static_assert(sizeof(report_statistics_body) == 347, "unexpected sizeof report_statistics_body");

#pragma pack(pop)
}
