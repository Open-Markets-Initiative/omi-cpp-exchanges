#pragma once

#include <cstdint>

namespace nse::nsefo::orderentry::nnf::v9_50 {

// Trailer Record Message
struct trailer_record_message {

    static constexpr auto name = "Trailer Record Message";

    // selected when transaction_code == 7031
    static constexpr auto field = "transaction_code";
    static constexpr std::int16_t value = 7031;
};

}
