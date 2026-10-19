#pragma once

#include <cstddef>
#include "../types/OrgScope.hpp"
#include "../types/ReportDate.hpp"
#include "../types/UserType.hpp"
#include "../types/Brokernumber.hpp"
#include "../types/BrokerName.hpp"
#include "../types/TraderNumber.hpp"
#include "../types/TraderName.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct spread_report_header_body {

    nnf_orderentry::org_scope org_scope;
    nnf_orderentry::report_date report_date;
    nnf_orderentry::user_type user_type;
    nnf_orderentry::brokernumber brokernumber;
    nnf_orderentry::broker_name broker_name;
    nnf_orderentry::trader_number trader_number;
    nnf_orderentry::trader_name trader_name;

    // parse method
    static spread_report_header_body* parse(std::byte* buffer) {
        return reinterpret_cast<spread_report_header_body*>(buffer);
    }

    // parse method const
    static const spread_report_header_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_report_header_body*>(buffer);
    }
};

// layout verification
static_assert(offsetof(spread_report_header_body, org_scope) == 0, "unexpected offset of spread_report_header_body::org_scope");
static_assert(offsetof(spread_report_header_body, report_date) == 1, "unexpected offset of spread_report_header_body::report_date");
static_assert(offsetof(spread_report_header_body, user_type) == 5, "unexpected offset of spread_report_header_body::user_type");
static_assert(offsetof(spread_report_header_body, brokernumber) == 7, "unexpected offset of spread_report_header_body::brokernumber");
static_assert(offsetof(spread_report_header_body, broker_name) == 12, "unexpected offset of spread_report_header_body::broker_name");
static_assert(offsetof(spread_report_header_body, trader_number) == 37, "unexpected offset of spread_report_header_body::trader_number");
static_assert(offsetof(spread_report_header_body, trader_name) == 41, "unexpected offset of spread_report_header_body::trader_name");
static_assert(sizeof(spread_report_header_body) == 67, "unexpected sizeof spread_report_header_body");

#pragma pack(pop)
}
