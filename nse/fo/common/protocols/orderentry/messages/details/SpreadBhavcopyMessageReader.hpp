#pragma once

#include "SpreadBhavcopyMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


inline sbe_var_data read_spread_report_body(const spread_bhavcopy_message& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

}
