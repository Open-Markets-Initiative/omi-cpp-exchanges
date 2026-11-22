#pragma once

#include <cstdint>

namespace nse::nsefo::orderentry::nnf::v9_50 {

// Heartbeat Message
struct heartbeat_message {

    static constexpr auto name = "Heartbeat Message";

    // selected when transaction_code == 23506
    static constexpr auto field = "transaction_code";
    static constexpr std::int16_t value = 23506;

    // action properties
    static constexpr auto session = "Heartbeat";
    static constexpr auto origin = "Firm";
};

}
