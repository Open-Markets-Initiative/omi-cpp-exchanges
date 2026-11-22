#pragma once

#include <cstddef>
#include "../types/Reserved1.hpp"
#include "../types/NoOfRecords.hpp"
#include "../structs/SpdStatsData.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct spread_report_statistics_body {

    nnf_orderentry::reserved_1 reserved_1;
    nnf_orderentry::no_of_records no_of_records;
    nnf_orderentry::spd_stats_data spd_stats_data[3];

    // parse method
    static spread_report_statistics_body* parse(std::byte* buffer) {
        return reinterpret_cast<spread_report_statistics_body*>(buffer);
    }

    // parse method const
    static const spread_report_statistics_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_report_statistics_body*>(buffer);
    }
};

// layout verification
static_assert(offsetof(spread_report_statistics_body, reserved_1) == 0, "unexpected offset of spread_report_statistics_body::reserved_1");
static_assert(offsetof(spread_report_statistics_body, no_of_records) == 1, "unexpected offset of spread_report_statistics_body::no_of_records");
static_assert(offsetof(spread_report_statistics_body, spd_stats_data) == 3, "unexpected offset of spread_report_statistics_body::spd_stats_data");
static_assert(sizeof(spread_report_statistics_body) == 237, "unexpected sizeof spread_report_statistics_body");

#pragma pack(pop)
}
