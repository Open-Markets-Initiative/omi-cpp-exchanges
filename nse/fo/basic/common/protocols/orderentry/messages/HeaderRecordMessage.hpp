#pragma once

#include <cstdint>

namespace nse::nsefo::orderentry::nnf::v9_50 {

// Header Record Message
struct header_record_message {

    static constexpr auto name = "Header Record Message";

    // selected when transaction_code == 7011
    static constexpr auto field = "transaction_code";
    static constexpr std::int16_t value = 7011;
};

}
