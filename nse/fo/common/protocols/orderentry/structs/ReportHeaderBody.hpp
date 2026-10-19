#pragma once

#include <cstddef>
#include "../types/OrgScope.hpp"
#include "../types/ReportDate.hpp"
#include "../types/UserType.hpp"
#include "../types/BrokerId.hpp"
#include "../types/FirmName.hpp"
#include "../types/TraderNumber.hpp"
#include "../types/TraderName.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct report_header_body {

    nnf_orderentry::org_scope org_scope;
    nnf_orderentry::report_date report_date;
    nnf_orderentry::user_type user_type;
    nnf_orderentry::broker_id broker_id;
    nnf_orderentry::firm_name firm_name;
    nnf_orderentry::trader_number trader_number;
    nnf_orderentry::trader_name trader_name;

    // parse method
    static report_header_body* parse(std::byte* buffer) {
        return reinterpret_cast<report_header_body*>(buffer);
    }

    // parse method const
    static const report_header_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const report_header_body*>(buffer);
    }
};

// layout verification
static_assert(offsetof(report_header_body, org_scope) == 0, "unexpected offset of report_header_body::org_scope");
static_assert(offsetof(report_header_body, report_date) == 1, "unexpected offset of report_header_body::report_date");
static_assert(offsetof(report_header_body, user_type) == 5, "unexpected offset of report_header_body::user_type");
static_assert(offsetof(report_header_body, broker_id) == 7, "unexpected offset of report_header_body::broker_id");
static_assert(offsetof(report_header_body, firm_name) == 12, "unexpected offset of report_header_body::firm_name");
static_assert(offsetof(report_header_body, trader_number) == 37, "unexpected offset of report_header_body::trader_number");
static_assert(offsetof(report_header_body, trader_name) == 41, "unexpected offset of report_header_body::trader_name");
static_assert(sizeof(report_header_body) == 67, "unexpected sizeof report_header_body");

#pragma pack(pop)
}
