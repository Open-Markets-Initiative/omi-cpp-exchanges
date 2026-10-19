#pragma once

#include "MarketStatisticsReportMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


inline sbe_var_data read_report_body(const market_statistics_report_message& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

}
